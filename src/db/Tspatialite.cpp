#include "db/Tspatialite.h"
#include <sqlite3.h>
#include <iostream>
#include <string>

#include "core/Tob.hpp"

#include <nlohmann/json.hpp>

//#include <windows.h>





//-----------------------------------------------------------------------------------------------------------

void test_sqlite_and_spatialite() {
    sqlite3* db = nullptr;
    char* errMsg = nullptr;

    // --- Ouvrir BDD mémoire ---
    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        std::cerr << "[SQLite] Échec ouverture: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    std::cout << "[SQLite] ✅ Ouverture réussie" << std::endl;

    // --- Initialiser SpatiaLite ---
    //spatialite_init(0); // indispensable si tu linkes directement la lib SpatiaLite
    std::cout << "[SpatiaLite] ✅ spatialite_init OK" << std::endl;

    // --- Vérif support RTREE ---
    const char* rtree_sql = R"(
        CREATE VIRTUAL TABLE my_index USING rtree(id, minX, maxX, minY, maxY);
        INSERT INTO my_index VALUES (1, 0.0, 5.0, 0.0, 5.0);
    )";

    if (sqlite3_exec(db, rtree_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "[SQLite] ❌ RTREE non supporté : " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }
    std::cout << "[SQLite] ✅ RTREE fonctionne" << std::endl;

    // --- Init SpatiaLite ---
    /*if (sqlite3_exec(db, "SELECT InitSpatialMetaData();", nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "[SpatiaLite] ❌ InitSpatialMetaData échouée : " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }*/
    std::cout << "[SpatiaLite] ✅ InitSpatialMetaData OK" << std::endl;

    // --- Création d'une table spatiale ---
    const char* create_table_sql = R"(
        CREATE TABLE villes (id INTEGER PRIMARY KEY, nom TEXT);
        SELECT AddGeometryColumn('villes', 'geom', 4326, 'POINT', 'XY');
    )";

    if (sqlite3_exec(db, create_table_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "[SpatiaLite] ❌ Création table spatiale échouée : " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

    // --- Insertion avec géométrie ---
    const char* insert_sql = R"(
        INSERT INTO villes (id, nom, geom)
        VALUES (1, 'Tana', ST_GeomFromText('POINT(47.5 -18.9)', 4326));
    )";

    if (sqlite3_exec(db, insert_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "[SpatiaLite] ❌ Insertion échouée : " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

    // --- Création index spatial ---
    const char* index_sql = R"(
        SELECT CreateSpatialIndex('villes', 'geom');
    )";

    if (sqlite3_exec(db, index_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "[SpatiaLite] ❌ Index spatial échoué : " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

    std::cout << "[SpatiaLite] ✅ Insertion + Index spatial OK" << std::endl;

    // --- Requête spatiale simple ---
    const char* spatial_query = R"(
        SELECT nom FROM villes
        WHERE ST_Within(geom, BuildMbr(47.0, -19.0, 48.0, -18.0, 4326));
    )";

    auto callback = [](void*, int argc, char** argv, char**) -> int {
        std::cout << "[SpatiaLite] 🔍 Ville trouvée : " << (argc > 0 ? argv[0] : "inconnue") << std::endl;
        return 0;
    };

    if (sqlite3_exec(db, spatial_query, callback, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "[SpatiaLite] ❌ Requête spatiale échouée : " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    else {
        std::cout << "[SpatiaLite] ✅ Requête spatiale OK" << std::endl;
    }

    sqlite3_close(db);
}

//-----------------------------------------------------------------------------------------------------------

bool TspatialiteDriver::open(const std::string& dbName) {
    // Ajouter le dossier contenant les DLL nécessaires à la recherche de Windows
    // Ou simplement copier les dll corespondant pres de l'exe
    //SetDllDirectoryW(L"C:\\Users\\lfotsing\\Documents\\MyDocs\\Training\\projects\\TLibs\\TM\\cpp\\TModeler\\bin\\msvc");
    //+++SetDllDirectoryW(L"D:\\Pro\\TLibs\\TM\\cpp\\TModeler\\bin\\msvc"); // just copie TModeler/bin near your file.exe or use this on windows

    if (!TsqliteDriver::open(dbName)) {
        return false;
    }

    // Activer le chargement des extensions
    int rc = sqlite3_enable_load_extension(db, 1);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to enable extension loading: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    char* errorMsg = nullptr;
    rc = sqlite3_load_extension(
        db,
        "mod_spatialite.dll",
        nullptr,
        &errorMsg
    );
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to load SpatiaLite: " << (errorMsg ? errorMsg : "Unknown error") << std::endl;
        sqlite3_free(errorMsg);
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    if (!ensureSpatialMetadataInitialized())
    {
        bool result = executeSQL("SELECT InitSpatialMetadata(1);");
        if (!result)
        {
            std::cerr << "Failed to load SpatiaLite: " << std::endl;
            return false;
        }
    }

    return true;
}

// Fermer la base de données
void TspatialiteDriver::close() {
    TsqliteDriver::close();

    // Nettoyer le chemin spécial de recherche des DLLs
    ///+++SetDllDirectoryW(nullptr);  // just copie TModeler/bin near your file.exe or use this on windows
}

void printGeometryColumns(sqlite3* db) {
    std::string query = "SELECT f_table_name, f_geometry_column, geometry_type FROM geometry_columns;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare SELECT: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "geometry_columns content:\n";
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::string table = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string column = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int geomType = sqlite3_column_int(stmt, 2);

        std::cout << "  table: " << table
            << ", column: " << column
            << ", type: " << geomType << "\n";
    }

    sqlite3_finalize(stmt);
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool TspatialiteDriver::dropTable(const std::string& tableName) {
    std::string normalizedTable = toLower(tableName);

    // Étape 1 : Récupérer les noms des colonnes géométriques associées
    std::string selectGeoCols =
        "SELECT f_geometry_column FROM geometry_columns WHERE f_table_name = '" + normalizedTable + "';";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, selectGeoCols.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare SELECT: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    std::vector<std::string> geometryColumns;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char* geomCol = sqlite3_column_text(stmt, 0);
        if (geomCol)
            geometryColumns.emplace_back(reinterpret_cast<const char*>(geomCol));
    }

    sqlite3_finalize(stmt); // ✅ On libère AVANT les suppressions !

    // Étape 2 : Supprimer les indexes/triggers/colonnes
    for (const std::string& columnName : geometryColumns) {
        // Essayer de désactiver l’index spatial (si CreateSpatialIndex a été utilisé)
        std::string disableIndexSQL =
            "SELECT DisableSpatialIndex('" + tableName + "', '" + columnName + "');";
        executeSQL(disableIndexSQL); // ignore les erreurs (fonction manquante ou inexistante)

        // Supprimer manuellement l’index spatial si la ligne précédente ne suffit pas
        std::string dropIndexSQL = "DROP TABLE IF EXISTS idx_" + tableName + "_" + columnName + ";";
        executeSQL(dropIndexSQL);

        // Supprimer les triggers liés aux indexes spatiaux
        std::vector<std::string> triggers = {
            "rtree_" + tableName + "_" + columnName + "_insert",
            "rtree_" + tableName + "_" + columnName + "_update1",
            "rtree_" + tableName + "_" + columnName + "_update2",
            "rtree_" + tableName + "_" + columnName + "_delete"
        };
        for (const std::string& trig : triggers) {
            std::string dropTriggerSQL = "DROP TRIGGER IF EXISTS " + trig + ";";
            executeSQL(dropTriggerSQL);
        }

        // Supprimer la colonne géométrique (le registre, pas physiquement la colonne)
        std::string discardGeomSQL =
            "SELECT DiscardGeometryColumn('" + tableName + "', '" + columnName + "');";
        executeSQL(discardGeomSQL);
    }

    // Étape 3 : Supprimer la table elle-même
    return TsqliteDriver::dropTable(tableName);
}

bool TspatialiteDriver::ensureSpatialMetadataInitialized() {
    // Vérifie si la table spatial_ref_sys existe
    std::string checkQuery =
        "SELECT name FROM sqlite_master WHERE type='table' AND name='spatial_ref_sys';";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, checkQuery.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare check query: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_step(stmt);
    bool alreadyInitialized = (rc == SQLITE_ROW); // spatial_ref_sys existe déjà

    sqlite3_finalize(stmt);

    if (!alreadyInitialized) {
        // Initialiser si ce n'est pas encore fait
        std::string initQuery = "SELECT InitSpatialMetadata();";
        if (!executeSQL(initQuery)) {
            std::cerr << "Failed to initialize SpatiaLite metadata" << std::endl;
            return false;
        }
    }

    return true;
}
