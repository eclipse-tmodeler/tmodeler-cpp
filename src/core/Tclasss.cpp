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
#include <regex>

#include "core/Tob.hpp"

#include <string>

#include "field/TBaseField.hpp"

#ifdef __GNUG__
#include <cxxabi.h>
#include <cstdlib>
#endif

namespace ai::tambo::tmodeler {

Tob tob_instance;


AnyList Tclass::getFields(const bool includeSuper) {
    if (!includeSuper) {
        return _fields;
    }

    AnyList allFields = _fields;

    // Utiliser un pointeur pour permettre la réaffectation
    Tclass* current = this->super();

    while (current != nullptr) {
        try {
            const AnyList& superFields = current->_fields;

            allFields.insert(allFields.end(), superFields.begin(), superFields.end());

            current = current->super();  // réaffectation possible ici
        }
        catch (...) {}
    }

    return allFields;
}


std::string Tclass::name(const std::string& rawName) const
{
    std::string demangled;

#ifdef __GNUG__
    int status = -1;
    char* realname = abi::__cxa_demangle(rawName.c_str(), nullptr, nullptr, &status);
    demangled = (status == 0 && realname != nullptr) ? realname : rawName;
    std::free(realname);
#else
    demangled = rawName;
#endif

    // Supprime les mots-clés "class " ou "struct " (MSVC)
    size_t pos = demangled.find("class ");
    if (pos != std::string::npos) {
        demangled = demangled.substr(pos + 6);
    }

    pos = demangled.find("struct ");
    if (pos != std::string::npos) {
        demangled = demangled.substr(pos + 7);
    }

    // Ne garder que le dernier segment après "::" (enlève le namespace)
    pos = demangled.rfind("::");
    if (pos != std::string::npos) {
        demangled = demangled.substr(pos + 2);
    }

    return demangled;
}

std::string Tclass::name() const {
    return name(type().name());
}

std::string Tclass::getPackage() const {
    return _package;
}

std::string Tclass::geTdb() const {
    return _db;
}

std::string Tclass::getModelName() const {
    std::string fullname = (getPackage().empty() ? "" : getPackage()+".") + name();

    fullname = std::regex_replace(fullname, std::regex(R"([^a-zA-Z0-9_])"), "_");

    return fullname;
}

// Check if both classes refer to the same type
bool Tclass::equals(const Tclass& other) const {
    return this->type() == other.type();
}

std::vector<std::string> Tclass::fieldsKeys() {
    auto fields = getFields();
    std::vector<std::string> list;

    for (const auto& [key, field] : fields) {
        list.push_back(key);
    }

    return list;
}

std::vector<std::string> Tclass::geoFieldsKeys() {
    auto fields = getFields();
    std::vector<std::string> list;

    for (const auto& [key, field] : fields) {
        BaseField* base = baseField(field);
        if (base->isGeo) list.push_back(key);
    }

    return list;
}

bool Tclass::isFunctField(std::string v)
{
    for (auto field : geoFieldsKeys())
    {
        if (v==field)
        {
            return true;
        }
    }
    return false;
}



std::unordered_map<std::string, int> toMap(const std::vector<std::string>& vec) {
    std::unordered_map<std::string, int> map;
    for (size_t i = 0; i < vec.size(); ++i) {
        map[vec[i]] = static_cast<int>(i);
    }
    return map;
}

bool Tclass::hasFunction(std::string fkey)
{
    std::any field = _fieldsMap[fkey];
    if (field.has_value())
    {
        BaseField* base = baseField(field);
        return base->hasFunction();
    }
    return false;
}

std::string Tclass::applyFunction(std::string fkey, std::string value)
{
    std::any field = _fieldsMap[fkey];
    if (field.has_value())
    {
        BaseField* base = baseField(field);
        if (base->hasFunction()) {
            base->resetFuncIndex();
            return base->applyFunc(fkey, value);
        }
    }
    return "";
}

// Check if this class is a descendant of another class
int Tclass::descendsFrom(const Tclass& parent) const {
    Tclass* current = this->super();
    int generation = 0;

    while (current != nullptr) {
        ++generation;
        if (current->equals(parent)) {
            return generation; // Found matching ancestor
        }
        current = current->super();
    }

    return 0; 
}


Tclass::Tclass(Tob& inst, Tclass* sup)
    : _type(typeid(inst)),
    _superClass(sup)
{}
Tclass::Tclass(Tob& inst)
    : Tclass(inst, nullptr)
{}
Tclass Tclass::with(Tob& instance)
{
    Tclass t(instance, nullptr);
    return t;
}

/*
Tclass::Tclass(std::type_index type, std::shared_ptr<Tclass> super)
    : _type(type), _superClass(nullptr)
{}*/

Tclass::Tclass()
    : _type(typeid(tob_instance)), _superClass(nullptr) {}

const std::type_index& Tclass::type() const { return _type; }



Tclass* Tclass::super() const { return _superClass; }
Tclass& Tclass::super(Tclass* v) { _superClass = v; return *this; }

Tclass& Tclass::package(std::string v) {
    _package = v;
    return *this;
}

Tclass& Tclass::db(std::string v) {
    _db = v;
    return *this;
}

void Tclass::prepare()
{
    _fieldsMap = toMap(_fields);
}

Tclass Tclass::fields(std::initializer_list < std::pair < std::string, std::any>> fields) {
    _fields.clear();
    for (const auto& [name, field] : fields) {
        _fields.emplace_back(name, field);
    }

    prepare();

    return *this;
}

bool Tclass::equals(Tclass* cls) const {
    if (cls == nullptr)
    {
        return false;
    }
    return type() == cls->type();
}



std::map<std::string, std::any> Tclass::toMap(AnyList vec) {
    std::map<std::string, std::any> result;
    for (const auto& [key, value] : vec) {
        result[key] = value;
    }
    return result;
}


bool Tclass::operator==(Tclass& cls) const {
    return equals(&cls);
}

bool Tclass::operator!=(Tclass&& cls) const {
    return !equals(&cls);
}

} // namespace ai::tambo::tmodeler
