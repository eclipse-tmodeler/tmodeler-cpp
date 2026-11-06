/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#ifndef Tserial_H
#define Tserial_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <variant>

#include <nlohmann/json_fwd.hpp>

namespace ai::tambo::tmodeler {

using Json = nlohmann::json;

class BaseField;
class Tob;
class Tclass;

class Tserial {
private:
    using FieldPair = std::pair<std::function<void(Json&)>, std::function<void(const Json&)>>;
    using FieldMap = std::unordered_map<std::string, FieldPair>;

    void deserialize(const Json& j, const char* fname, int& field);
    void deserialize(const Json& j, const char* fname, bool& field);
    void deserialize(const Json& j, const char* fname, double& field);
    void deserialize(const Json& j, const char* fname, std::string& field);
    void deserialize(const Json& j, const char* fname, Json& field);

    void serialize(Json& j, const char* fname, int& field);
    void serialize(Json& j, const char* fname, bool& field);
    void serialize(Json& j, const char* fname, double& field);
    void serialize(Json& j, const char* fname, std::string& field);
    void serialize(Json& j, const char* fname, Json& field);

public:

    // Désérialisation de std::any (type simple)
    virtual void deserialize(const Json& j, const char* key, std::any& field);

    // Sérialisation de std::any (type simple)
    virtual void serialize(Json& j, const char* key, std::any field);
};


} // namespace ai::tambo::tmodeler
#endif // Tserial_H
