/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#ifndef TCLASS_H
#define TCLASS_H

#include <functional>
#include <typeindex>
#include <string>
#include <unordered_map>
#include <iostream>
#include <variant>
#include <any>
#include <map>

namespace ai::tambo::tmodeler {

using AnyList = std::vector<std::pair<std::string, std::any>>;

class Tob;

class Tclass {
protected:
    AnyList _fields;
    std::string _package = "";
    std::string _db = "";

    std::map<std::string, std::any> _fieldsMap;

    std::type_index _type;
    /* for another time
    std::shared_ptr<Tclass> _super = nullptr;
    */
    Tclass* _superClass = nullptr;

public:
    /*
    Tclass(std::type_index type, std::shared_ptr<Tclass> super = nullptr);*/

    Tclass();
    Tclass(Tob& inst, Tclass* sup = nullptr);
    Tclass(Tob& inst);
    static Tclass with(Tob& instance);

    const std::type_index& type() const;

    Tclass* super() const;
    Tclass& super(Tclass* v);



    Tclass& package(std::string v);

    Tclass& db(std::string v);


    void prepare();

    template <typename... Args>
    Tclass& fields(Args&&... args) {
        _fields.clear();
        (_fields.emplace_back(args.first, args.second), ...);

        prepare();

        return *this;
    }

    Tclass fields(std::initializer_list < std::pair < std::string, std::any>> fields);

    AnyList getFields(const bool super = true);

    // Compare types between this and another Tclass
    bool equals(const Tclass& other) const;

    std::vector<std::string> fieldsKeys();
    std::vector<std::string> geoFieldsKeys();
    bool isFunctField(std::string v);

    bool hasFunction(std::string fkey);
    std::string applyFunction(std::string fkey, std::string value);

    // Check if this class descends from another class and return generation distance
    int descendsFrom(const Tclass& parent) const;

    std::string name() const;

    std::string getPackage() const;

    std::string geTdb() const;

    std::string getModelName() const;

    bool equals(Tclass* cls) const;


    std::map<std::string, std::any> toMap(AnyList vec);


    bool operator==(Tclass& cls) const;

    bool operator!=(Tclass&& cls) const;

private:
    std::string name(const std::string& rawName) const;
};



} // namespace ai::tambo::tmodeler
#endif // TCLASS_H
