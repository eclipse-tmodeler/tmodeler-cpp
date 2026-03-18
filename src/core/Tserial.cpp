#include "core/Tclass.h"

#include <iostream>

#include <nlohmann/json.hpp>

#include "core/Tob.hpp"
#include "field/TField.hpp"

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

void Tserial::deserialize(const Json& j, const char* fname, int& field) { j.at(fname).get_to(field); }
void Tserial::deserialize(const Json& j, const char* fname, bool& field) { j.at(fname).get_to(field); }
void Tserial::deserialize(const Json& j, const char* fname, double& field) { j.at(fname).get_to(field); }
void Tserial::deserialize(const Json& j, const char* fname, std::string& field) { j.at(fname).get_to(field); }
void Tserial::deserialize(const Json& j, const char* fname, Json& field) { j.at(fname).get_to(field); }

//void Tserial::serialize(Json& j, const char* fname, std::any& field) { j[fname] = field; }
void Tserial::serialize(Json& j, const char* fname, int& field) { j[fname] = field; }
void Tserial::serialize(Json& j, const char* fname, bool& field) { j[fname] = field; }
void Tserial::serialize(Json& j, const char* fname, double& field) { j[fname] = field; }
void Tserial::serialize(Json& j, const char* fname, std::string& field) { j[fname] = field; }
void Tserial::serialize(Json& j, const char* fname, Json& field) { j[fname] = field; }




// Désérialisation de std::any (type simple)
void Tserial::deserialize(const Json& j, const char* key, std::any& field) {
    if (!isValidUntil(deadline)) { return; }

    if (j.contains(key)) {
        if (field.type() == typeid(int)) {
            deserialize(j, key, std::any_cast<int&>(field));
        }
        else if (field.type() == typeid(int*)) {
            deserialize(j, key, *std::any_cast<int*>(field));
        }
        else if (field.type() == typeid(bool)) {
            deserialize(j, key, std::any_cast<bool&>(field));
        }
        else if (field.type() == typeid(bool*)) {
            deserialize(j, key, *std::any_cast<bool*>(field));
        }
        else if (field.type() == typeid(double)) {
            deserialize(j, key, std::any_cast<double&>(field));
        }
        else if (field.type() == typeid(double*)) {
            deserialize(j, key, *std::any_cast<double*>(field));
        }
        else if (field.type() == typeid(std::string)) {
            deserialize(j, key, std::any_cast<std::string&>(field));
        }
        else if (field.type() == typeid(std::string*)) {
            deserialize(j, key, *std::any_cast<std::string*>(field));
        }
        else if (field.type() == typeid(Json)) {
            deserialize(j, key, std::any_cast<Json&>(field));
        }
        else if (field.type() == typeid(Json*)) {
            deserialize(j, key, *std::any_cast<Json*>(field));
        }
        else {
            if (j.at(key).is_string())
            {
                std::string& s = std::any_cast<std::string&>(field);
                deserialize(j, key, s);
            }
        }
    }
}

// Sérialisation de std::any (type simple)
void Tserial::serialize(Json& j, const char* key, std::any field) {
    if (!isValidUntil(deadline)) { return; }

    if (field.type() == typeid(int)) {
        serialize(j, key, std::any_cast<int&>(field));
    }
    else if (field.type() == typeid(int*)) {
        serialize(j, key, *std::any_cast<int*>(field));
    }
    else if (field.type() == typeid(bool)) {
        serialize(j, key, std::any_cast<bool&>(field));
    }
    else if (field.type() == typeid(bool*)) {
        serialize(j, key, *std::any_cast<bool*>(field));
    }
    else if (field.type() == typeid(double)) {
        serialize(j, key, std::any_cast<double&>(field));
    }
    else if (field.type() == typeid(double*)) {
        serialize(j, key, *std::any_cast<double*>(field));
    }
    else if (field.type() == typeid(std::string)) {
        serialize(j, key, std::any_cast<std::string&>(field));
    }
    else if (field.type() == typeid(std::string*)) {
        serialize(j, key, *std::any_cast<std::string*>(field));
    }
    else if (field.type() == typeid(Json)) {
        serialize(j, key, std::any_cast<Json&>(field));
    }
    else if (field.type() == typeid(Json*)) {
        serialize(j, key, *std::any_cast<Json*>(field));
    }
}
