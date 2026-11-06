/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "core/Tclass.h"

#include <iostream>

#include <nlohmann/json.hpp>

#include "core/Tob.hpp"
#include "field/TField.hpp"

namespace ai::tambo::tmodeler {

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

} // namespace ai::tambo::tmodeler
