/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#pragma once

#include <any>
#include <string>
#include <chrono>
#include <vector>

#include "field/TField.hpp"
#include "field/TTextField.hpp"

#include <nlohmann/json.hpp>

namespace ai::tambo::tmodeler {

template <typename Derived>
class JsonFieldBase : public TextFieldBase<Derived> {
public:
    JsonFieldBase() : TextFieldBase<Derived>() {}

    std::string str(bool pretty) const {
        return str(4);
    }

    std::string str(int indent = 0) const {
        try
        {
            if (indent > 0)
                return json().dump(indent);
        }
        catch (...) {}

        return TextFieldBase<Derived>::get();
    }

    Json json() const {
        try {
            return Json::parse(TextFieldBase<Derived>::get());
        }
        catch (...) {
            return Json{};
        }
    }

    void set(Json v) {
        TextFieldBase<Derived>::set(v.dump());
    }

    virtual Derived& defaults(Json v) {
        set(v);
        return static_cast<Derived&>(*this);
    }

    // Type checking
    bool isObject() const { return json().is_object(); }
    bool isArray()  const { return json().is_array(); }
    bool isNull()   const { return json().is_null(); }
    bool isString() const { return json().is_string(); }
    bool isBool()   const { return json().is_boolean(); }
    bool isNumber() const { return json().is_number(); }

    // Accès à une clé (lecture seule)
    Json get(const std::string& key) const {
        Json j = json();
        return j.is_object() && j.contains(key) ? j[key] : Json{};
    }

    // Affectation d'une clé
    void put(const std::string& key, const Json& val) {
        Json j = json();
        if (!j.is_object()) j = Json::object();
        j[key] = val;
        set(j);
    }

    // ✅ Accès index (array)
    Json get(size_t index) const {
        Json j = json();
        if (j.is_array() && index < j.size()) return j.at(index);
        return Json{};
    }

    void add(const Json& value) {
        Json j = json();
        if (!j.is_array()) j = Json::array(); // conversion si pas un array
        j.push_back(value);
        set(j);
    }

    // Taille
    size_t size() const {
        return json().size();
    }

    bool empty() const {
        return json().empty();
    }

    // Suppression d'une clé
    void remove(const std::string& key) {
        Json j = json();
        if (j.is_object() && j.contains(key)) {
            j.erase(key);
            set(j);
        }
    }
    void remove(size_t index) {
        Json j = json();
        if (j.is_array() && index < j.size()) {
            j.erase(j.begin() + index);
            set(j);
        }
    }
    void remove(const Json& value) {
        Json j = json();
        if (j.is_array()) {
            auto it = std::find(j.begin(), j.end(), value);
            if (it != j.end()) {
                j.erase(it);
                set(j);
            }
        }
    }

    void clear() {
        Json j = json();
        j.clear();
        set(j);
    }

    // cheker
    bool accept(const std::string& key) const {
        Json j = json();
        return j.is_object() && j.contains(key);
    }
    bool contains(const Json& value) const {
        Json j = json();
        if (j.is_array()) {
            return std::find(j.begin(), j.end(), value) != j.end();
        }
        return false;
    }

    // Récupère les clés présentes
    std::vector<std::string> keys() const {
        std::vector<std::string> result;
        for (auto& el : json().items()) {
            result.push_back(el.key());
        }
        return result;
    }

    std::vector<Json> values() const {
        std::vector<Json> result;
        Json j = json();
        if (j.is_array()) {
            for (const auto& el : j) result.push_back(el);
        }
        else if (j.is_object()) {
            for (const auto& [k, v] : j.items()) result.push_back(v);
        }
        return result;
    }

    // Merge avec un autre JsonField
    void merge(const Derived& other, bool overwrite = true) {
        Json j = json();
        Json o = other.json();
        j.merge_patch(o);
        set(j);
    }

    // Renvoie la string JSON formatée
    std::string pretty(int indent = 4) const {
        return json().dump(indent);
    }

    // operators
    operator bool() const {
        return json() != Json{};
    }
    operator Json() const {
        return json();
    }
    Derived& operator=(const Json& v) {
        set(v);
        return static_cast<Derived&>(*this);
    }
    Derived& operator=(Json&& v) {
        set(v);
        return static_cast<Derived&>(*this);
    }
    Derived& operator+=(const Json& value) {
        add(value);
        return static_cast<Derived&>(*this);
    }
    Derived& operator+=(std::initializer_list<std::pair<std::string, Json>> list) {
        for (const auto& kv : list) {
            put(kv.first, kv.second);
        }
        return static_cast<Derived&>(*this);
    }
    Derived& operator+=(std::initializer_list<int> list) {
        for (const auto& json : list) {
            add(json);
        }
        return static_cast<Derived&>(*this);
    }
};

class JsonField : public JsonFieldBase<JsonField>
{
public:
    using JsonFieldBase::operator bool;
    using JsonFieldBase::operator Json;
    using JsonFieldBase::operator=;
    using JsonFieldBase::operator+=;
};

} // namespace ai::tambo::tmodeler
