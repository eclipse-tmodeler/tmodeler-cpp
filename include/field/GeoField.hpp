#pragma once

#include <any>
#include <string>
#include <chrono>
#include <vector>

#include "field/TField.hpp"
#include "field/TTextField.hpp"

#include "Geo.hpp"

template <typename Derived>
class GeoFieldBase : public TField<Derived, std::string> {

public:
    enum Type { POINT, LINESTRING, POLYGON, MULTIPOINT, MULTILINESTRING, MULTIPOLYGON, GEOMETRYCOLLECTION, GEOMETRY };

public:

    GeoFieldBase() : TField<Derived, std::string>(TFtype::GEO)
    {
        TField<Derived, std::string>::defaults(BaseField::NULL_STRING);
        BaseField::isGeo = true;
        prepare();
    }

    void prepare()
    {
        switch (_type)
        {
        case Type::GEOMETRY:
            _baseFunc = "ST_GeomFromText";
            break;
        case Type::POINT:
            _baseFunc = "ST_PointFromText";
            break;
        case Type::LINESTRING:
            _baseFunc = "ST_LineFromText";
            break;
        defaults:
            _baseFunc = "ST_GeomFromText";
            break;
        }

        Tx::prepare(this, _baseFunc, _srid);
    }


    std::string get() const override { return to<std::string>(TField<Derived, std::string>::getValue()); }
    void set(std::string v) override { TField<Derived, std::string>::setValue(v); }

    TGeo geo() const
    {
        std::string str = to<std::string>(TField<Derived, std::string>::getValue());
        return TGeo::fromString(str);
    }

    void set(TGeo& v)
    {
        TField<Derived, std::string>::setValue(v.toString());
    }


    virtual Derived& spatialIndex(bool v=true) {
        _spatialIndex = v;
        prepare();
        return static_cast<Derived&>(*this);
    }

    virtual Derived& srid(int v) {
        _srid = v;
        prepare();
        return static_cast<Derived&>(*this);
    }

    virtual Derived& type(Type v) {
        _type = v;
        prepare();
        return static_cast<Derived&>(*this);
    }

    virtual Derived& type(std::string v) {
        return type(typeFromStr(v));
    }

    virtual Derived& dim(int v) {
        if (v>=2)
        {
            _dim = v;
        }
        return static_cast<Derived&>(*this);
    }

    virtual Derived& measure(bool v) {
        _measure = v;
        return static_cast<Derived&>(*this);
    }

public:
    Type _type = GEOMETRY;
    int _srid = 4326;
    int _dim = 2;
    bool _measure = false;
    bool _spatialIndex = true;
    std::string _baseFunc = "ST_GeomFromText";

public:
    static std::string typeToStr(Type type) {
        switch (type) {
        case POINT: return "POINT";
        case LINESTRING: return "LINESTRING";
        case POLYGON: return "POLYGON";
        case MULTIPOINT: return "MULTIPOINT";
        case MULTILINESTRING: return "MULTILINESTRING";
        case MULTIPOLYGON: return "MULTIPOLYGON";
        case GEOMETRYCOLLECTION: return "GEOMETRYCOLLECTION";
        case GEOMETRY: return "GEOMETRY";
        default: return "GEOMETRY";
        }
    }

    static Type typeFromStr(const std::string& str) {
        if (str == "POINT") return POINT;
        if (str == "LINESTRING") return LINESTRING;
        if (str == "POLYGON") return POLYGON;
        if (str == "MULTIPOINT") return MULTIPOINT;
        if (str == "MULTILINESTRING") return MULTILINESTRING;
        if (str == "MULTIPOLYGON") return MULTIPOLYGON;
        if (str == "GEOMETRYCOLLECTION") return GEOMETRYCOLLECTION;
        if (str == "GEOMETRY") return GEOMETRY;

        return GEOMETRY;
    }

public:
    // ops
    Derived& operator=(TGeo& v)
    {
        set(v);
        return static_cast<Derived&>(*this);
    }
    Derived& operator=(TGeo&& v)
    {
        set(v);
        return static_cast<Derived&>(*this);
    }
    operator Spot() const {
        return geo().asSpot();
    }
    operator Line() const {
        return geo().asLine();
    }
    operator TPolygon() const {
        return geo().asPolygon();
    }
    operator Rect() const {
        return geo().asRect();
    }
    operator MultiPolygon() const {
        return geo().asMultiPolygon();
    }

public: // ops
    Tx operator==(const TGeo* val) {
        return TField<Derived, std::string>::operator==(nullptr);
    }
    Tx operator!=(const TGeo* val) {
        return TField<Derived, std::string>::operator!=(nullptr);
    }
    Tx operator==(const TGeo& val) {
        return equals(val);
    }
    Tx operator!=(const TGeo& val) {
        Derived& d = static_cast<Derived&>(*this);
        return !(d == val);
    }
public: // funtions
    Tx equals(const TGeo& val) {
        Tx x(this, Operator::ST_Equals);
        x.geoValue = &val;
        return x;
    }
    Tx contains(const TGeo& content) { // looking for containers
        Tx x(this, Operator::ST_Contains);
        x.geoValue = &content;
        return x;
    }
    Tx intersects(const TGeo& val) {
        Tx x(this, Operator::ST_Intersects);
        x.geoValue = &val;
        return x;
    }
    Tx index(const TGeo& val) { // obligatoire pour spatialite
        return intersectsMbr(val);
    }
    Tx intersectsMbr(const TGeo& val) { // seule fonction optimal de spatialite (using spatial index)
        Tx x(this, Operator::MbrIntersects);
        x.geoValue = &val;
        return x;
    }
    Tx intersectsOpt(const TGeo& val) {
        auto valCopy = std::make_shared<TGeo>(val);
        Tx t1 = index(*valCopy);
        Tx t2 = intersects(*valCopy);

        // Don't work

        return t1 && t2;
    }
    Tx inside(const TGeo& container) { // looking for contents
        Tx x(this, Operator::ST_Within);
        x.geoValue = &container;
        return x;
    }
    Tx dWithin(const TGeo& val) {
        Tx x(this, Operator::ST_DWithin);
        x.geoValue = &val;
        return x;
    }
    Tx touches(const TGeo& val) {
        Tx x(this, Operator::ST_Touches);
        x.geoValue = &val;
        return x;
    }
    Tx crosses(const TGeo& val) {
        Tx x(this, Operator::ST_Crosses);
        x.geoValue = &val;
        return x;
    }
    Tx overlaps(const TGeo& val) {
        Tx x(this, Operator::ST_Overlaps);
        x.geoValue = &val;
        return x;
    }
    Tx disjoint(const TGeo& val) {
        Tx x(this, Operator::ST_Disjoint);
        x.geoValue = &val;
        return x;
    }

public:// functions fields
    Derived& distance(const TGeo& val) {
        //BaseField::clearFunc();
        Tx x(this, Operator::ST_Distance);
        x.prepare(&val);
        return static_cast<Derived&>(*this);
    }
    Derived& length() {
        Tx x(this, Operator::ST_Length);
        x.prepare();
        return static_cast<Derived&>(*this);
    }
    Derived& area() {
        Tx x(this, Operator::ST_Area);
        x.prepare();
        return static_cast<Derived&>(*this);
    }
    Derived& perimeter() {
        Tx x(this, Operator::ST_Perimeter);
        x.prepare();
        return static_cast<Derived&>(*this);
    }
    Derived& azimuth(TGeo& val) {
        Tx x(this, Operator::ST_Azimuth);
        x.prepare(&val);
        return static_cast<Derived&>(*this);
    }
    Derived& xCoord() {
        Tx x(this, Operator::ST_X);
        x.prepare();
        return static_cast<Derived&>(*this);
    }
    Derived& yCoord() {
        Tx x(this, Operator::ST_Y);
        x.prepare();
        return static_cast<Derived&>(*this);
    }
    Derived& pointsCount() {
        Tx x(this, Operator::ST_NumPoints);
        x.prepare();
        return static_cast<Derived&>(*this);
    }
    Derived& dimension() {
        Tx x(this, Operator::ST_Dimension);
        x.prepare();
        return static_cast<Derived&>(*this);
    }
    Derived& geometryType() {
        Tx x(this, Operator::ST_GeometryType);
        x.prepare();
        return static_cast<Derived&>(*this);
    }
    Derived& isClosed() {
        Tx x(this, Operator::ST_IsClosed);
        x.prepare();
        return static_cast<Derived&>(*this);
    }



public:
    Tx operator>(const double val) {
        Tx x(this, Operator::GreaterThan);
        x.value = val;
        return x;
    }
    Tx operator<(const double val) {
        Tx x(this, Operator::LessThan);
        x.value = val;
        return x;
    }
    Tx operator==(const double val) {
        Tx x(this, Operator::Equal);
        x.value = val;
        return x;
    }
    Tx operator!=(const double val) {
        Tx x(this, Operator::NotEqual);
        x.value = val;
        return x;
    }
    Tx operator>=(const double val) {
        Tx x(this, Operator::GreaterThanEqual);
        x.value = val;
        return x;
    }
    Tx operator<=(const double val) {
        Tx x(this, Operator::LessThanEqual);
        x.value = val;
        return x;
    }
    Tx operator>>=(const std::vector<double>& values)
    {
        Tx x(this, Operator::In);
        x.value = vectorToString(values);
        return x;
    }
};

class GeoField : public GeoFieldBase<GeoField>
{
public:
    using GeoFieldBase<GeoField>::operator=;
};

class GeoPoint : public GeoFieldBase<GeoField>
{
public:
    using TField::operator=;
    using TField::operator std::string;
};
