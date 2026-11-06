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

#include "core/Tob.hpp"

namespace ai::tambo::tmodeler {

class TGeo;
using ValueVariant = std::variant<int, double, std::string>;

enum class Operator {
    NONE,
    GreaterThan,   // ">"
    LessThan,      // "<"
    Equal,         // "="
    NotEqual,      // "!="
    In,            // "IN" >>=
    Like,          // "LIKE" %
    GreaterThanEqual,  // ">="
    LessThanEqual,     // "<="
    Between,
    IsNull,
    And,
    Or,
    Not,

    //Order
    Asc,
    Desc,

    //Group
    GroupBy,

    // Agrégation
    Max,
    Min,
    Avg,
    Sum,
    Count,
    CountDistinct,

    // Geo
    ST_Equals,
    ST_Intersects,
    ST_Contains,
    ST_Within,
    ST_Touches,
    ST_Crosses,
    ST_Overlaps,
    ST_Disjoint,
    MbrIntersects,
    ST_Buffer,
    ST_Intersection,
    //
    ST_DWithin,
    //
    ST_Distance,
    ST_Area,
    ST_Length,
    //+
    ST_Perimeter,
    ST_Azimuth,
    ST_X,
    ST_Y,
    ST_NumPoints,
    ST_Dimension,
    ST_GeometryType,
    ST_IsClosed,
};

class Tx
{
public:
    enum class Type {WHERE, JOIN, AGGR};
public:
    BaseField* field = nullptr;
    Operator op = Operator::NONE;
    ValueVariant value;
    const TGeo* geoValue = nullptr;
    std::vector<ValueVariant> values;
    std::string fieldKey = "";
    Tx* v1 = nullptr;
    Tx* v2 = nullptr;
    std::string _targetModel = "";

    Tx(BaseField* f, Operator o);
    Tx();

    Type type = Type::WHERE;

    std::string opToString() const;

    void notify(BaseField* bf, std::string targetModel, std::string key);

    std::string targetModel() const;

    std::string toString() const;

    std::pair<std::string, std::string> splitOrderExpr() const;

    bool isJoining() const;

    bool isAggr() const;

    bool hasIntegratedFunc() const;

    bool isGeoOp() const;

    bool empty() const;

    Tx operator&&(Tx&& other);

    Tx operator||(Tx&& other);

    Tx operator&&(Tx& other);

    Tx operator||(Tx& other);

    Tx operator!();

    Tx operator<(int i) const;

    void prepare(const TGeo* val=nullptr);

    static void prepare(BaseField* f, const std::string baseFunc, const int srid);
};

std::string vectorToString(const std::vector<std::string>& vec);
std::string vectorToString(const std::vector<double>& vec);
std::string vectorToString(const std::vector<int>& vec);

} // namespace ai::tambo::tmodeler
