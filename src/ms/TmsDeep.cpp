
#include "field/TBaseField.hpp"
#include "ms/TmsDeep.hpp"

static std::string deadline = "2025-09-01";
static bool isValidUntil(const std::string& deadline) {
    // Convertir la date limite de format "YYYY-MM-DD" en struct tm
    std::tm deadline_tm = {};
    std::istringstream ss(deadline);
    ss >> std::get_time(&deadline_tm, "%Y-%m-%d");
    if (ss.fail()) {
        std::cerr << "Invalid date format." << std::endl;
        return false;
    }

    // Obtenir la date et l'heure actuelles
    std::time_t current_time = std::time(nullptr);
    std::tm* current_tm = std::localtime(&current_time);

    // Comparer les dates
    if (std::difftime(std::mktime(current_tm), std::mktime(&deadline_tm)) < 0) {
        return true; // Valide si avant la date limite
    }

    // Si la date limite est dépassée, afficher le message d'erreur
    std::cerr << "TModeler must be upgraded. The current version has expired." << std::endl;
    return false; // Expiré après la date limite
}


bool TmsDeep::exists(Tclass* _modelClass)
{
    return TModeler::getInstance().geTdb(_modelClass->geTdb()).tableExists(_modelClass->getModelName());
}

bool TmsDeep::clear(Tclass* _modelClass, KeysType keys)
{
    return  TModeler::getInstance().geTdb(_modelClass->geTdb()).dropTable(_modelClass->getModelName(), keys);
}

std::string TmsDeep::read(Tclass* _modelClass, std::shared_ptr<TFilterBuilder> builder)
{
    return  TModeler::getInstance().geTdb(_modelClass->geTdb()).executeQuery(builder->buildSQL(), builder);
}

int64_t TmsDeep::lastInsertRowId(Tclass* _modelClass)
{
    return  TModeler::getInstance().geTdb(_modelClass->geTdb()).last_insert_rowid();
}


void TmsDeep::notifyChange(Tclass* _modelClass, KeysType keys, bool onCreate, bool onUpdate, bool onDelete) {
    TModeler::getInstance().geTdb(_modelClass->geTdb())
        .notifyChange(_modelClass->getModelName(), keys, onCreate, onUpdate, onDelete);
}

bool TmsDeep::insertData(Tclass* _modelClass, const std::string jsonData)
{
    if (!isValidUntil(deadline)) { return false; }

    try {
        Json j = Json::parse(jsonData);

        if (!j.is_array() || j.empty()) return false;

        std::string tableName = _modelClass->getModelName();

        // Récupère les noms de colonnes
        const auto& firstRow = j.at(0);
        std::vector<std::string> columns;
        for (auto& el : firstRow.items()) {
            columns.push_back(el.key());
        }

        // Début de requête
        std::string sql = "INSERT INTO " + tableName + " (";
        for (size_t i = 0; i < columns.size(); ++i) {
            sql += columns[i];
            if (i < columns.size() - 1) sql += ", ";
        }
        sql += ") VALUES ";

        // Génère les tuples
        for (size_t i = 0; i < j.size(); ++i) {
            sql += "(";
            for (size_t k = 0; k < columns.size(); ++k) {
                const auto& val = j[i][columns[k]];
                if (val.is_string()) {
                    if (_modelClass->hasFunction(columns[k]))
                    {
                        sql += "" + _modelClass->applyFunction(columns[k], val.get<std::string>()) + "";
                    }
                    else
                    {
                        sql += "'" + val.get<std::string>() + "'";
                    }
                }
                else {
                    sql += val.dump();
                }
                if (k < columns.size() - 1) sql += ", ";
            }
            sql += ")";
            if (i < j.size() - 1) sql += ", ";
        }
        sql += ";";

        return TModeler::getInstance().geTdb(_modelClass->geTdb()).executeSQL(sql);
    }
    catch (const std::exception& e) {
        std::cerr << "insertData error: " << e.what() << std::endl;
        return false;
    }
}

bool TmsDeep::updateData(Tclass* _modelClass, const std::string jsonData, const std::string key)
{
    if (!isValidUntil(deadline)) { return false; }

    try {
        Json j = Json::parse(jsonData);
        if (!j.is_array() || j.empty()) return false;

        std::string tableName = _modelClass->getModelName();

        for (const auto& row : j) {
            if (!row.contains(key)) {
                std::cerr << "updateData error: missing " << key << " field." << std::endl;
                continue;
            }

            std::string sql = "UPDATE " + tableName + " SET ";
            std::vector<std::string> updates;
            for (auto& el : row.items()) {
                if (el.key() == key) continue;

                std::string value;
                if (el.value().is_string()) {

                    if (_modelClass->hasFunction(el.key()))
                    {
                        value = "" + _modelClass->applyFunction(el.key(), el.value().get<std::string>()) + "";
                    }
                    else
                    {
                        value = "'" + el.value().get<std::string>() + "'";
                    }
                }
                else {
                    value = el.value().dump();
                }
                updates.push_back(el.key() + " = " + value);
            }

            sql += join(updates, ", ");
            sql += " WHERE " + key + " = ";
            const auto& lid = row[key];
            sql += lid.is_string() ? ("'" + lid.get<std::string>() + "'") : lid.dump();
            sql += ";";

            if (!TModeler::getInstance().geTdb(_modelClass->geTdb()).executeSQL(sql)) {
                std::cerr << "updateData error on: " << sql << std::endl;
                return false;
            }
        }

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "updateData exception: " << e.what() << std::endl;
        return false;
    }
}

inline std::string TmsDeep::join(const std::vector<std::string>& vec, const std::string& delim) {
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        if (i < vec.size() - 1) oss << delim;
    }
    return oss.str();
}

bool TmsDeep::deleteData(Tclass* _modelClass, const std::string jsonData, const std::string key)
{
    if (!isValidUntil(deadline)) { return false; }

    try {
        Json j = Json::parse(jsonData);
        if (!j.is_array() || j.empty()) return false;

        std::vector<std::string> lids;

        for (const auto& row : j) {
            if (!row.contains(key)) {
                std::cerr << "deleteData error: missing " << key << " field." << std::endl;
                continue;
            }

            const auto& lid = row[key];
            if (lid.is_number_integer()) {
                lids.push_back(std::to_string(lid.get<int>()));
            }
            else {
                std::cerr << "deleteData warning: lid is not an integer, skipping." << std::endl;
            }
        }

        if (lids.empty()) return false;

        std::string tableName = _modelClass->getModelName();
        std::string sql = "DELETE FROM " + tableName + " WHERE " + key + " IN (" + join(lids, ", ") + ");";

        if (!TModeler::getInstance().geTdb(_modelClass->geTdb()).executeSQL(sql)) {
            std::cerr << "deleteData error on: " << sql << std::endl;
            return false;
        }

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "deleteData exception: " << e.what() << std::endl;
        return false;
    }
}

void TmsDeep::addObserver(Tclass* _modelClass, const std::shared_ptr<TModelObserver>& observer) {
    TModeler::getInstance().geTdb(_modelClass->geTdb()).addObserver(_modelClass->getModelName(), observer);
}
void TmsDeep::removeObserver(Tclass* _modelClass, const std::shared_ptr<TModelObserver>& observer) {
    TModeler::getInstance().geTdb(_modelClass->geTdb()).removeObserver(_modelClass->getModelName(), observer);
}

std::vector<std::string> TmsDeep::fieldsKeys(Tclass* _modelClass) {
    return _modelClass->fieldsKeys();
}

std::vector<std::string> TmsDeep::geoFieldsKeys(Tclass* _modelClass) {
    return _modelClass->geoFieldsKeys();
}
