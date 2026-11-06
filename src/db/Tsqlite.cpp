/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "db/Tsqlite.h"
#include <sqlite3.h>
#include <iostream>
#include <string>

#include "core/Tob.hpp"

#include <nlohmann/json.hpp>
#include <sqlite3.h>

#include "ms/TFilterBuilder.h"
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

///#include <windows.h>

namespace ai::tambo::tmodeler {

TsqliteDriver::TsqliteDriver() : db(nullptr) {}

TsqliteDriver::~TsqliteDriver() {
    if (db) {
        sqlite3_close(db);
    }
}

// Fonction de callback silencieuse
static void sqliteLogSilencer(void* /*unused*/, int /*errorCode*/, const char* /*msg*/) {
    // Silence total
}

bool TsqliteDriver::open(const std::string& dbName) {
    // À appeler une fois avant tout sqlite3_open()
    sqlite3_config(SQLITE_CONFIG_LOG, sqliteLogSilencer, nullptr);

    int rc = sqlite3_open(dbName.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}

// Fermer la base de données
void TsqliteDriver::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}


bool testSpatiaLiteFeatures(sqlite3* db) {
    char* errMsg = nullptr;
    int rc;

    auto exec = [&](const std::string& sql) -> bool {
        std::cout << ">>> " << sql << std::endl;
        rc = sqlite3_exec(db, sql.c_str(), [](void*, int argc, char** argv, char** colName) {
            for (int i = 0; i < argc; ++i)
                std::cout << colName[i] << ": " << (argv[i] ? argv[i] : "NULL") << "\n";
            std::cout << "----------------------------\n";
            return 0;
            }, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Erreur: " << (errMsg ? errMsg : "inconnue") << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    };

    // 1. Vérifier les versions
    exec("SELECT sqlite_version();");
    exec("SELECT spatialite_version();");

    // 1.2. Meta
    exec("SELECT * FROM pragma_module_list;");

    // 2. Charger l'extension mod_spatialite
    std::cout << "Chargement de l'extension mod_spatialite...\n";
    rc = sqlite3_enable_load_extension(db, 1);  // Autorise le chargement dynamique
    if (rc != SQLITE_OK) {
        std::cerr << "Erreur sqlite3_enable_load_extension\n";
        return false;
    }

    rc = sqlite3_load_extension(db, "mod_spatialite", nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Erreur lors du chargement de mod_spatialite: " << (errMsg ? errMsg : "inconnue") << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    std::cout << "Extension mod_spatialite chargée avec succès.\n";

    // 3. Créer une table temporaire avec une colonne géométrique
    exec("DROP TABLE IF EXISTS temp_test;");
    exec("CREATE TABLE temp_test(id INTEGER PRIMARY KEY AUTOINCREMENT);");
    exec("SELECT AddGeometryColumn('temp_test', 'geom', 4326, 'POINT', 'XY');");

    // 4. Tester CreateSpatialIndex
    if (!exec("SELECT CreateSpatialIndex('temp_test', 'geom');")) {
        std::cerr << "❌ L'index spatial n'a pas pu être créé. Le module R*Tree est probablement absent." << std::endl;
        return false;
    }
    else {
        std::cout << "✅ Index spatial créé avec succès.\n";
    }

    // Nettoyage
    exec("DROP TABLE temp_test;");
    return true;
}


// Créer une table avec un script SQL
bool TsqliteDriver::createTable(const std::string& tableScript) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, tableScript.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// Supprimer une table par son nom
bool TsqliteDriver::dropTable(const std::string& tableName) {
    std::string query = "DROP TABLE IF EXISTS " + tableName;
    return executeSQL(query);
}

// Vérifier si une table existe
bool TsqliteDriver::tableExists(const std::string& tableName) {
    std::string query = "SELECT name FROM sqlite_master WHERE type='table' AND name='" + tableName + "';";
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), [](void* data, int argc, char** argv, char** colName) -> int {
        if (argc > 0 && argv[0]) {
            return 1; // Table exists
        }
        return 0; // Table does not exist
    }, nullptr, &errMsg);

    if (rc == 0) {
        //std::cerr << "SQL error: " << errMsg << std::endl;
        //sqlite3_free(errMsg);
        return false;
    }

    return true;
}

/*
// Exécuter une requête SQL générique
bool TsqliteDriver::executeSQL(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}
*/

static int saved_stderr_fd = -1;

static bool save_stderr() {
#ifdef _WIN32
    saved_stderr_fd = _dup(_fileno(stderr));
    if (saved_stderr_fd == -1)
        return false;

    int devnull = _open("NUL", _O_WRONLY);
    if (devnull == -1)
        return false;

    if (_dup2(devnull, _fileno(stderr)) == -1) {
        _close(devnull);
        return false;
    }

    _close(devnull);
#else
    saved_stderr_fd = dup(STDERR_FILENO);
    if (saved_stderr_fd == -1)
        return false;

    int devnull = open("/dev/null", O_WRONLY);
    if (devnull == -1)
        return false;

    if (dup2(devnull, STDERR_FILENO) == -1) {
        close(devnull);
        return false;
    }

    close(devnull);
#endif
    return true;
}

static bool restore_stderr() {
    if (saved_stderr_fd == -1)
        return false; // pas de sauvegarde préalable

#ifdef _WIN32
    if (_dup2(saved_stderr_fd, _fileno(stderr)) == -1)
        return false;
    _close(saved_stderr_fd);
#else
    if (dup2(saved_stderr_fd, STDERR_FILENO) == -1)
        return false;
    close(saved_stderr_fd);
#endif

    saved_stderr_fd = -1; // marquer comme restauré
    return true;
}

bool TsqliteDriver::executeSQL(const std::string& sql) {
    save_stderr();

    // Exécution SQL
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    restore_stderr();

    // Gestion des erreurs SQLite
    if (rc != SQLITE_OK) {
        if (errMsg) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
        else {
            std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        }
        return false;
    }
    return true;
}


int64_t TsqliteDriver::last_insert_rowid() {
    return sqlite3_last_insert_rowid(db);
}

// Structure du contexte passé à sqlite3_exec
struct CallbackContext {
    std::vector<Json>* groupedRowsPtr;
    std::any* builder;
};

// Version simple : appelle la version complète avec un vecteur vide
std::string TsqliteDriver::executeQuery(const std::string& query) {
    return executeQuery(query, {});
}

// Version principale : exécute la requête et retourne un JSON bien formatté
std::string TsqliteDriver::executeQuery(const std::string& query, std::any builder) {
    auto realBuilder = std::any_cast<std::shared_ptr<TFilterBuilder>>(builder);

    Json result = Json::array();
    char* errMsg = nullptr;
    std::vector<Json> groupedRows;
    CallbackContext context{ &groupedRows, &builder }; // ca passe pas ici

    // Lambda sans capture, compatible avec sqlite3_exec
    auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
        auto* ctx = static_cast<CallbackContext*>(data);
        auto* groupedRowsPtr = ctx->groupedRowsPtr;
        //const auto& fieldsKeys = *(ctx->fieldsKeysPtr);
        const auto& builder_ = *(ctx->builder);
        auto realBuilder = std::any_cast<std::shared_ptr<TFilterBuilder>>(builder_);
        //const auto& fieldsKeys = realBuilder->fieldsKeys;
        const auto& tableFields = realBuilder->tableFields;

        std::map<std::string, int> colCount;
        std::vector<Json> rowVariants;
        rowVariants.emplace_back(Json::object());

        for (int i = 0; i < argc; ++i) {
            const char* val = argv[i];
            const char* col = colName[i];
            std::string colStr = col ? col : "";

            if (colStr.empty())
                continue;

            int index = 0;
            int j = 0;
            int someFields = 0;
            if (tableFields.size() > 1) {
                for (size_t g = 0; g < tableFields.size(); ++g) {
                    someFields += tableFields[g].size();
                    if (i < someFields) {
                        index = g;
                        someFields -= tableFields[g].size();
                        break;
                    }
                }
            }
            j = i - someFields;
            /*int index = 0;
            if (fieldsKeys.size() > 1) {
                int someFields = 0;
                for (size_t g = 0; g < fieldsKeys.size(); ++g) {
                    someFields += fieldsKeys[g].size();
                    if (i < someFields) {
                        index = g;
                        break;
                    }
                }
            }*/

            if (index >= rowVariants.size()) {
                rowVariants.resize(index + 1, Json::object());
            }

            Json& targetRow = rowVariants[index];

            if (!val) {
                // targetRow[colStr] = nullptr;
            }
            else {
                std::string strVal = val;

                auto v = tableFields[index][j].toScript();

                if(tableFields[index][j].isTextField()) targetRow[colStr] = strVal;
                else if (strVal == "true") targetRow[colStr] = true;
                else if (strVal == "false") targetRow[colStr] = false;
                else if (strVal == "null") targetRow[colStr] = nullptr;
                else {
                    std::istringstream iss(strVal);
                    long intVal;
                    if ((iss >> intVal) && iss.eof()) {
                        targetRow[colStr] = intVal;
                    }
                    else {
                        iss.clear();
                        iss.str(strVal);
                        double doubleVal;
                        if ((iss >> doubleVal) && iss.eof()) {
                            targetRow[colStr] = doubleVal;
                        }
                        else {
                            targetRow[colStr] = strVal;
                        }
                    }
                }
            }
        }

        if (rowVariants.size() == 1) {
            groupedRowsPtr->push_back(rowVariants[0]);
        }
        else {
            Json group = Json::array();
            for (auto& r : rowVariants) {
                group.push_back(r);
            }
            groupedRowsPtr->push_back(group);
        }

        return 0;
    };

    int rc = sqlite3_exec(db, query.c_str(), callback, static_cast<void*>(&context), &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << (errMsg ? errMsg : "Unknown error") << std::endl;
        if (errMsg) sqlite3_free(errMsg);
        return "{}";
    }

    Json finalResult = Json::array();
    for (const auto& item : groupedRows) {
        finalResult.push_back(item);
    }

    return finalResult.dump(4);
}

// Supprimer une base de données
bool TsqliteDriver::deleteDatabase(const std::string& dbName) {
    if (remove(dbName.c_str()) != 0) {
        std::cerr << "Error deleting database: " << dbName << std::endl;
        return false;
    }
    return true;
}


std::string TsqliteDriver::tableSchema(const std::string& tableName) {
    std::string query = "PRAGMA table_info(" + tableName + ");";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare PRAGMA table_info: " << sqlite3_errmsg(db) << std::endl;
        return "";
    }

    std::ostringstream result;
    result << "Schema for table '" << tableName << "':\n";
    result << "cid | name | type | notnull | dflt_value | pk\n";
    result << "----------------------------------------------\n";

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int cid = sqlite3_column_int(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        const unsigned char* type = sqlite3_column_text(stmt, 2);
        int notnull = sqlite3_column_int(stmt, 3);
        const unsigned char* dflt_value = sqlite3_column_text(stmt, 4);
        int pk = sqlite3_column_int(stmt, 5);

        result << cid << " | "
            << (name ? reinterpret_cast<const char*>(name) : "NULL") << " | "
            << (type ? reinterpret_cast<const char*>(type) : "NULL") << " | "
            << notnull << " | "
            << (dflt_value ? reinterpret_cast<const char*>(dflt_value) : "NULL") << " | "
            << pk << "\n";
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Error while stepping through results: " << sqlite3_errmsg(db) << std::endl;
        return "";
    }

    return result.str();
}

} // namespace ai::tambo::tmodeler
