/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "field/TJsonTools.h"
#include <algorithm>
#include <nlohmann/json.hpp>

namespace ai::tambo::tmodeler {

namespace json_utils {

    Json parse(const std::string& s) {
        try {
            return Json::parse(s);
        }
        catch (...) {
            return Json{};
        }
    }

    std::string dump(const Json& j) {
        return j.dump();
    }

    std::string str(const Json& j, int indent) {
        try {
            return j.dump(indent);
        }
        catch (...) {
            return {};
        }
    }

    std::string pretty(const Json& j, int indent) {
        return j.dump(indent);
    }

    // Type checks
    bool isObject(const Json& j) { return j.is_object(); }
    bool isArray(const Json& j) { return j.is_array(); }
    bool isNull(const Json& j) { return j.is_null(); }
    bool isString(const Json& j) { return j.is_string(); }
    bool isBool(const Json& j) { return j.is_boolean(); }
    bool isNumber(const Json& j) { return j.is_number(); }

    // Access
    Json get(const Json& j, const std::string& key) {
        return (j.is_object() && j.contains(key)) ? j[key] : Json{};
    }

    Json at(const Json& j, size_t index) {
        return (j.is_array() && index < j.size()) ? j.at(index) : Json{};
    }

    void put(Json& j, const std::string& key, const Json& val) {
        if (!j.is_object()) j = Json::object();
        j[key] = val;
    }

    void add(Json& j, const Json& val) {
        if (!j.is_array()) j = Json::array();
        j.push_back(val);
    }

    // Query
    size_t size(const Json& j) {
        return j.size();
    }

    bool empty(const Json& j) {
        return j.empty();
    }

    bool contains(const Json& j, const std::string& key) {
        return j.is_object() && j.contains(key);
    }

    bool contains(const Json& j, const Json& value) {
        return j.is_array() && std::find(j.begin(), j.end(), value) != j.end();
    }

    std::vector<std::string> keys(const Json& j) {
        std::vector<std::string> result;
        if (j.is_object()) {
            for (auto& el : j.items()) {
                result.push_back(el.key());
            }
        }
        return result;
    }

    std::vector<Json> values(const Json& j) {
        std::vector<Json> result;
        if (j.is_array()) {
            for (const auto& el : j) result.push_back(el);
        }
        else if (j.is_object()) {
            for (const auto& [k, v] : j.items()) result.push_back(v);
        }
        return result;
    }

    // Modification
    void remove(Json& j, const std::string& key) {
        if (j.is_object() && j.contains(key)) {
            j.erase(key);
        }
    }

    void remove(Json& j, size_t index) {
        if (j.is_array() && index < j.size()) {
            j.erase(j.begin() + index);
        }
    }

    void remove(Json& j, const Json& value) {
        if (j.is_array()) {
            auto it = std::find(j.begin(), j.end(), value);
            if (it != j.end()) {
                j.erase(it);
            }
        }
    }

    void merge(Json& j1, const Json& j2, bool /*overwrite*/) {
        j1.merge_patch(j2);
    }

} // namespace json_utils

} // namespace ai::tambo::tmodeler
