/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "db/Ttable.h"

#include "field/TBaseField.h"
#include "ms/Tlist.h"

namespace ai::tambo::tmodeler {

std::string TtableField::toScript() const {
    std::ostringstream script;
    script << _name << " " << _type;

    if (_maxlen > 0) {
        script << "(" << _maxlen << ")";
    }

    if (!_constraints.empty()) {
        script << " " << _constraints;
    }

    if (_defaultValue.has_value()) {
        script << " DEFAULT " << formatDefaultValue(_defaultValue);
    }

    return script.str();
}

/*
 * Dans son adaptation (GeoField) doit tenir compte des variation spatial d'un base à l'autres
 *
 */
std::string TtableField::toGeoScript(std::string tname) const { // sqlite dedicated
    std::ostringstream script;

    std::string _dim = "";
    if (_geoDim==2)
    {
        _dim = "XY";
    }
    else if (_geoDim==3)
    {
        _dim = "XYZ";
    }
    if (_geoMeasure)
    {
        _dim += "M";
    }

    script << "SELECT AddGeometryColumn(" << "'" << tname << "', '" << _name << "', " << _geoSrid << ", '" << _geoType << "', '" << _dim << "');\n";

    return script.str();
}

std::string TtableField::toGeoSpatialIndexScript(std::string tname) const { // sqlite dedicated
    std::ostringstream script;

    script << "SELECT CreateSpatialIndex(" << "'" << tname << "', '" << _name << "');\n";

    return script.str();
}

std::string TtableField::toForeignScript() const {
    std::ostringstream script;

    if (!_targetTable.empty() && !_targetField.empty()) {
        script << "   FOREIGN KEY (" << _name << ")"
            << " REFERENCES " << _targetTable << "(" << _targetField << ")";

        if (!_onDelete.empty()) {
            script << " ON DELETE " << _onDelete;
        }
    }

    return script.str();
}

bool TtableField::isGeo() const
{
    return _type == tf_str(TFtype::GEO);
}

bool TtableField::isSpatialIndex() const
{
    return _geoSpatialIndex;
}


TtableField& TtableField::name(const std::string& v) {
    _name = v;
    return *this;
}

TtableField& TtableField::type(const std::string& v) {
    _type = v;
    return *this;
}

TtableField& TtableField::defaultValue(const std::any& v) {
    _defaultValue = v;
    return *this;
}

TtableField& TtableField::constraints(const std::string& v) {
    _constraints = v;
    return *this;
}

TtableField& TtableField::maxlen(int v) {
    _maxlen = v;
    return *this;
}

TtableField& TtableField::targetTable(const std::string& v) {
    _targetTable = v;
    return *this;
}

TtableField& TtableField::targetField(const std::string& v) {
    _targetField = v;
    return *this;
}

TtableField& TtableField::onDelete(const std::string& v) {
    _onDelete = v;
    return *this;
}

TtableField& TtableField::geoType(const std::string& v) {
    _geoType = v;
    return *this;
}

TtableField& TtableField::geoSrid(int v) {
    _geoSrid = v;
    return *this;
}

TtableField& TtableField::geoDim(int v) {
    _geoDim = v;
    return *this;
}

TtableField& TtableField::geoMeasure(bool v) {
    _geoMeasure = v;
    return *this;
}

TtableField& TtableField::geoSpatialIndex(bool v) {
    _geoSpatialIndex = v;
    return *this;
}

std::string TtableField::formatDefaultValue(const std::any& value) const {
    if (value.type() == typeid(std::string)) {
        return "'" + std::any_cast<std::string>(value) + "'";
    }
    else if (value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(value));
    }
    else if (value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(value));
    }
    else if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value) ? "TRUE" : "FALSE";
    }
    return "";
}















TtableDriver::TtableDriver() {}

TtableDriver::TtableDriver(const std::string& name, const std::vector<TtableField>& fields, const std::vector<std::string>& primaryKeys)
    : name(name), fields(fields), primaryKeys(primaryKeys) {}

std::string TtableDriver::toScript() const {
    std::ostringstream script;
    script << "CREATE TABLE " << name << " (\n";

    for (size_t i = 0; i < fields.size(); ++i) {
        if (!fields[i].isGeo())
        {
            script << "   " << fields[i].toScript();
            if (i != fields.size() - 1) {
                script << ",\n";
            }
        }
    }

    if (primaryKeys.size() > 1) {
        script << ",\n   PRIMARY KEY (" << join(primaryKeys, ", ") << ")";
    }

    std::vector<std::string> foreignKeys;
    for (const auto& field : fields) {
        std::string foreignScript = field.toForeignScript();
        if (!foreignScript.empty()) {
            foreignKeys.push_back(foreignScript);
        }
    }

    for (size_t i = 0; i < foreignKeys.size(); ++i) {
        script << ",\n" << foreignKeys[i];
    }

    script << "\n);";

    // Add Geo
    script << "\n";
    for (size_t i = 0; i < fields.size(); ++i) {
        if (fields[i].isGeo())
        {
            script << fields[i].toGeoScript(name);
        }
    }

    // Add spatial Index
    //script << "\n";
    for (size_t i = 0; i < fields.size(); ++i) {
        if (fields[i].isSpatialIndex())
        {
            script << fields[i].toGeoSpatialIndexScript(name);
        }
    }

    return script.str();
}

void TtableDriver::addField(const TtableField& field) {
    fields.push_back(field);
}

std::string TtableDriver::getName() const
{
    return name;
}

std::string TtableDriver::join(const std::vector<std::string>& elements, const std::string& delimiter) const {
    std::ostringstream os;
    for (size_t i = 0; i < elements.size(); ++i) {
        os << elements[i];
        if (i != elements.size() - 1) {
            os << delimiter;
        }
    }
    return os.str();
}










Ttable::Ttable(const std::string& name, const std::vector<TtableField>& fields,
    const std::vector<std::string>& primaryKeys)
{
    _driver = std::make_unique<TtableDriver>(name, fields, primaryKeys);
}

void Ttable::addObserver(const std::shared_ptr<TModelObserver>& observer) {
    _observers.push_back(observer);
}

void Ttable::removeObserver(const std::shared_ptr<TModelObserver>& observer) {
    /* T T T ::: IMPORTANT */
    //_observers.erase(std::remove(_observers.begin(), _observers.end(), observer),_observers.end());
}

void Ttable::notifyChange(KeysType keys) {
    for (int i = 0; i < _observers.size(); i++)
    {
        if (_observers.at(i) != nullptr)
        {
            _observers.at(i)->onModelChange(keys);
        }
    }
}

void Ttable::notifySave(KeysType keys) {
    for (int i = 0; i < _observers.size(); i++)
    {
        if (_observers.at(i) != nullptr)
        {
            _observers.at(i)->onSave(keys);
        }
    }
}

void Ttable::notifyCreate(KeysType keys) {
    for (int i=0;i<_observers.size();i++)
    {
        if (_observers.at(i)!=nullptr)
        {
            _observers.at(i)->onCreate(keys);
            _observers.at(i)->onSave(keys);
            _observers.at(i)->onModelChange(keys);
        }
    }
}

void Ttable::notifyUpdate(KeysType keys) {
    for (int i = 0; i < _observers.size(); i++)
    {
        if (_observers.at(i) != nullptr)
        {
            _observers.at(i)->onUpdate(keys);
            _observers.at(i)->onSave(keys);
            _observers.at(i)->onModelChange(keys);
        }
    }
}

void Ttable::notifyDelete(KeysType keys) {
    for (int i = 0; i < _observers.size(); i++)
    {
        if (_observers.at(i) != nullptr)
        {
            _observers.at(i)->onDelete(keys);
            _observers.at(i)->onModelChange(keys);
        }
    }
}


void Ttable::notifyChange(KeysType keys, bool onCreate, bool onUpdate, bool onDelete) {
    if (onCreate) { notifyCreate(keys); }
    if (onUpdate) { notifyUpdate(keys); }
    if (onDelete) { notifyDelete(keys); }
}

} // namespace ai::tambo::tmodeler
