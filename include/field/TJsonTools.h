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

#include <vector>
#include <string>

#include "core/Tob.hpp"

namespace ai::tambo::tmodeler {

namespace json_utils {

    Json parse(const std::string& s);
    std::string dump(const Json& j);
    std::string str(const Json& j, int indent = 0);
    std::string pretty(const Json& j, int indent = 4);

    // Type checks
    bool isObject(const Json& j);
    bool isArray(const Json& j);
    bool isNull(const Json& j);
    bool isString(const Json& j);
    bool isBool(const Json& j);
    bool isNumber(const Json& j);

    // Object/array access
    Json get(const Json& j, const std::string& key);
    Json at(const Json& j, size_t index);
    void put(Json& j, const std::string& key, const Json& val);
    void add(Json& j, const Json& val);

    // Query
    size_t size(const Json& j);
    bool empty(const Json& j);
    bool contains(const Json& j, const std::string& key);
    bool contains(const Json& j, const Json& value);
    std::vector<std::string> keys(const Json& j);
    std::vector<Json> values(const Json& j);

    // Modification
    void remove(Json& j, const std::string& key);
    void remove(Json& j, size_t index);
    void remove(Json& j, const Json& value);
    void merge(Json& j1, const Json& j2, bool overwrite = true);

} // namespace json_utils

} // namespace ai::tambo::tmodeler
