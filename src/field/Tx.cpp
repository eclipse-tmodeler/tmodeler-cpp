/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include <iostream>

#include "field/Geo.hpp"
#include "field/GeoField.hpp"
#include "field/TBaseField.h"


namespace ai::tambo::tmodeler {

Tx::Tx(BaseField* f, Operator o) : field(f), op(o) {}
Tx::Tx() {}


bool Tx::empty() const
{
    return op == Operator::NONE;
}

Tx Tx::operator&&(Tx& other) {
    Tx result;
    result.op = Operator::And;
    result.v1 = this;
    result.v2 = &other;
    return result;
}

Tx Tx::operator||(Tx& other) {
    Tx result;
    result.op = Operator::Or;
    result.v1 = this;
    result.v2 = &other;
    return result;
}

Tx Tx::operator&&(Tx&& other) {
    Tx result;
    result.op = Operator::And;
    result.v1 = this;
    result.v2 = &other;
    return result;
}

Tx Tx::operator||(Tx&& other) {
    Tx result;
    result.op = Operator::Or;
    result.v1 = this;
    result.v2 = &other;
    return result;
}

Tx Tx::operator!() {
    Tx result;
    result.op = Operator::Not;
    result.v1 = this;
    return result;
}

bool Tx::isJoining() const
{
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value).find('.') != std::string::npos;
    }
    else {
        return false;
    }
}

bool Tx::isAggr() const
{
    auto result =  (field != nullptr && field->hasAggr()) || (v1 != nullptr && v1->isAggr()) || (v2 != nullptr && v2->isAggr());
    return result;
}

bool Tx::hasIntegratedFunc() const
{
    return field->hasFunction();
}

bool Tx::isGeoOp() const
{
    return geoValue!=nullptr && (opToString().compare(0, 3, "ST_") == 0 || op==Operator::MbrIntersects);
}

std::string Tx::toString() const {
    std::ostringstream expr;
    if (op == Operator::And || op == Operator::Or || op == Operator::Not)
    {
        if (op == Operator::Not)
        {
            expr << opToString() << "(" << v1->toString() << ")";
        }
        else
        {
            expr << "(" << v1->toString() << opToString() << v2->toString() << ")";
        }
    }
    else if (isGeoOp())
    {
        switch (op) {
        case Operator::ST_Equals:
        case Operator::ST_Intersects:
        case Operator::MbrIntersects:
        case Operator::ST_Within:
        case Operator::ST_Contains:
        case Operator::ST_Touches:
        case Operator::ST_Crosses:
        case Operator::ST_Overlaps:
        case Operator::ST_Disjoint:
            expr << " " << opToString() << "(" << fieldKey << ", " ;
            expr << field->applyFunc((geoValue->hasFunction() ? "+" : ""), geoValue->toString()) << ") ";
            break;

        /*case Operator::ST_Distance:
            expr << " " << opToString() << "(" << fieldKey << ", "
                << geoValue->filterFunction << "('" << geoValue->toString() << "', " << geoValue->srid << ")) ";
            if (geoValue->distanceThreshold >= 0)
                expr << " <= " << geoValue->distanceThreshold << " ";
            break;

        case Operator::ST_Buffer:
            expr << opToString() << "(" << fieldKey << ", " << geoValue->bufferDistance << ") ";
            break;

        default:
            expr << " " << opToString() << "(" << fieldKey << ", "
                << geoValue->filterFunction << "('" << geoValue->toString() << "', " << geoValue->srid << ")) ";
            break;*/
        }
    }
    else
    {
        if (field != nullptr && field->hasAggr())
        {
            expr << field->aggr.opToString() << "(" << fieldKey << ")";

            if (field != nullptr && field->hasAggr())
            {
                field->initTmpAggr();
            }
        }
        else
        {
            expr << (field->hasFunction() && op!=Operator::IsNull ? field->applyFunc(fieldKey, "") : fieldKey) ;
        }

        if (op != Operator::GroupBy)
        {
            expr << " " << opToString() << " ";
        }

        if (op != Operator::IsNull && op != Operator::Asc && op != Operator::Desc && op != Operator::GroupBy)
        {
            std::visit([this, &expr](auto&& arg) {
                if (op != Operator::In &&
                    std::is_same_v<std::decay_t<decltype(arg)>, std::string> &&
                    !isJoining()) {
                    expr << "\"" << arg << "\"";
                }
                else {
                    expr << arg;
                }
                }, value);
        }
    }

    return expr.str();
}


void Tx::prepare(const TGeo* val)
{
    BaseField::FuncType function;

    std::string strOp = opToString();
    if (auto* geo = dynamic_cast<GeoField*>(field)) { // une base class neutre doit etre tester ici : je crois ou mieux renomer les attr geo et les renvoyer à la base class 
        switch (op)
        {
        case Operator::ST_Distance:
        case Operator::ST_Azimuth:
        {
            function = [this, val, geo, strOp](const std::string& fieldKey, const std::string& wkt) {
                return strOp + "(" + fieldKey + ", " + geo->_baseFunc + "('" + val->toString() + "', " + std::to_string(geo->_srid) + "))";
            };
            break;
        }

        case Operator::ST_Length:
        case Operator::ST_Area:
        case Operator::ST_Perimeter:
        case Operator::ST_NumPoints:
        case Operator::ST_Dimension:
        case Operator::ST_GeometryType:
        case Operator::ST_IsClosed:
        case Operator::ST_X:
        case Operator::ST_Y:
        {
            function = [this, strOp](const std::string& fieldKey, const std::string&) {
                return strOp + "(" + fieldKey + ")";
            };
            break;
        }

        case Operator::NONE:
        {
            function = [geo](const std::string& key, const std::string& wkt) {
                if (key == "+")
                {
                    return wkt;
                }
                else {
                    return geo->_baseFunc + "('" + wkt + "', " + std::to_string(geo->_srid) + ")";
                }
            };
            break;
        }
        default:;
        }
    }

    field->addFunc(function);
}


void Tx::prepare(BaseField* field, const std::string baseFunc, const int srid)
{
    field->clearFunc();

    field->addFunc([baseFunc, srid](const std::string& key, const std::string& wkt) {
        if (key == "+")
        {
            return wkt;
        }
        else {
            return baseFunc + "('" + wkt + "', " + std::to_string(srid) + ")";
        }
    });
}

std::pair<std::string, std::string> Tx::splitOrderExpr() const {
    const std::string& str_order = toString();

    std::string trimmed = str_order;
    std::string dir = "ASC";

    // Nettoyage des espaces
    while (!trimmed.empty() && std::isspace(trimmed.back())) trimmed.pop_back();

    if (trimmed.size() >= 4 &&
        (trimmed.substr(trimmed.size() - 4) == " ASC" || trimmed.substr(trimmed.size() - 4) == " asc")) {
        dir = "ASC";
        trimmed = trimmed.substr(0, trimmed.size() - 4);
    }
    else if (trimmed.size() >= 5 &&
        (trimmed.substr(trimmed.size() - 5) == " DESC" || trimmed.substr(trimmed.size() - 5) == " desc")) {
        dir = "DESC";
        trimmed = trimmed.substr(0, trimmed.size() - 5);
    }

    return { trimmed, dir };
}


std::string Tx::opToString() const {
    switch (op) {
    //case Operator::NONE: return "";
    case Operator::GreaterThan: return ">";
    case Operator::LessThan: return "<";
    case Operator::Equal: return "=";
    case Operator::NotEqual: return "!=";
    case Operator::In: return "IN";
    case Operator::Like: return "LIKE";
    case Operator::GreaterThanEqual: return ">=";
    case Operator::LessThanEqual: return "<=";
    case Operator::Between: return "BETWEEN";
    case Operator::IsNull: return "IS NULL";
    case Operator::And: return " AND ";
    case Operator::Or: return " OR ";
    case Operator::Not: return " NOT ";

    case Operator::Asc: return "ASC";
    case Operator::Desc: return "DESC";

    case Operator::GroupBy: return " GROUP BY ";

    case Operator::Max: return "MAX";
    case Operator::Min: return "MIN";
    case Operator::Avg: return "AVG";
    case Operator::Sum: return "SUM";
    case Operator::Count: return "COUNT";
    case Operator::CountDistinct: return "COUNT(DISTINCT";

    // Opérateurs spatiaux - test (booléens)
    case Operator::ST_Equals: return "ST_Equals";
    case Operator::ST_Contains: return "ST_Contains";
    case Operator::ST_Within: return "ST_Within";
    case Operator::ST_Intersects: return "ST_Intersects";
    case Operator::MbrIntersects: return "MbrIntersects";
    case Operator::ST_Touches: return "ST_Touches";
    case Operator::ST_Overlaps: return "ST_Overlaps";
    case Operator::ST_Crosses: return "ST_Crosses";
    case Operator::ST_Disjoint: return "ST_Disjoint";

    // Opérateurs spatiaux - calculs / mesures
    case Operator::ST_Distance: return "ST_Distance";
    case Operator::ST_Length: return "ST_Length";
    case Operator::ST_Area: return "ST_Area";
    case Operator::ST_Perimeter: return "ST_Perimeter";
    case Operator::ST_NumPoints: return "ST_NumPoints";
    case Operator::ST_Dimension: return "ST_Dimension";
    case Operator::ST_GeometryType: return "ST_GeometryType";
    case Operator::ST_IsClosed: return "ST_IsClosed";
    case Operator::ST_X: return "ST_X";
    case Operator::ST_Y: return "ST_Y";
    case Operator::ST_Azimuth: return "ST_Azimuth";

    // Transformations (si utilisées comme expressions)
    case Operator::ST_Buffer: return "ST_Buffer";
    case Operator::ST_Intersection: return "ST_Intersection";

    default: return "";
    }
}

void Tx::notify(BaseField* bf, std::string targetModel, std::string key)
{
    bf->resetFuncIndex(1);

    _targetModel = targetModel;

    if (v1 != nullptr)
    {
        v1->notify(bf, targetModel, key);
    }
    if (v2 != nullptr)
    {
        v2->notify(bf, targetModel, key);
    }
    if (bf == field) {
        fieldKey = key;
    }
}

std::string Tx::targetModel() const
{
    if (_targetModel.empty())
    {
        if (v1 != nullptr)
        {
            return v1->targetModel();
        }
        if (v2 != nullptr)
        {
            return v2->targetModel();
        }
    }
    return _targetModel;
}

void BaseField::initTmpAggr()
{
    aggr.op = Operator::NONE;
}
bool BaseField::hasAggr()
{
    return !aggr.empty();
}
bool BaseField::hasFunction()
{
    return !_functions.empty() || _function?true:false;
}

std::string BaseField::targetField()
{
    return _targetField;
}

Tx BaseField::isNull() {
    Tx x(this, Operator::IsNull);
    return x;
}



void BaseField::clearFunc() {
    resetFuncIndex();
    _functions.clear();
}


void BaseField::addFunc(const FuncType& func) {
    resetFuncIndex();

    _functions.push_back(func);
}

std::string BaseField::applyFunc(const std::string& fieldKey, const std::string& wkt) {
    if (_functions.empty())
        throw std::logic_error("No function registered");

    // Applique la fonction à l'index courant
    std::string result = _functions[_currentIndex](fieldKey, wkt);

    // Passe à l'index suivant (cyclique ou bloquant)
    if (_currentIndex + 1 < _functions.size()) {
        ++_currentIndex;
    }
    else {
        // reset tmp functions
        if (_functions.size() > 1) {
            _functions.erase(_functions.begin() + 1, _functions.end());
        }
    }

    return result;
}

void BaseField::resetFuncIndex(int v) {
    if (v >= _functions.size() || _currentIndex == v) return;
    _currentIndex = v;
}



std::string vectorToString(const std::vector<std::string>& vec) {
    std::stringstream ss;
    ss << "(";
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << "'" << vec[i] << "'";
        if (i != vec.size() - 1) {
            ss << ", ";
        }
    }
    ss << ")";
    return ss.str();
}

std::string vectorToString(const std::vector<double>& vec) {
    std::stringstream ss;
    ss << "(";
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << vec[i];
        if (i != vec.size() - 1) {
            ss << ", ";
        }
    }
    ss << ")";
    return ss.str();
}

std::string vectorToString(const std::vector<int>& vec) {
    std::stringstream ss;
    ss << "(";
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << vec[i];
        if (i != vec.size() - 1) {
            ss << ", ";
        }
    }
    ss << ")";
    return ss.str();
}

} // namespace ai::tambo::tmodeler
