#pragma once

#include "Tms.h"
#include "field/TField.h"

#include "ms/TitemCreate.hpp"
#include "ms/TitemDelete.hpp"
#include "ms/TitemUpdate.hpp"
#include "ms/TitemLoad.hpp"

#include "TmsAdapter.hpp"
#include "TmsCrud.hpp"
#include "TmsDeep.hpp"
#include "TmsFilter.hpp"
#include "TmsObserver.hpp"



template <typename... Ts>
template <typename... NewTypes>
Tms<Ts..., NewTypes...> Tms<Ts...>::with(Ts&... ts, NewTypes&... args) {
    auto newTms = Tms<Ts..., NewTypes...>();

    if (newTms._builder) {
        _builder.reset();
    }
    newTms._builder = std::make_shared<TFilterBuilder>();

    newTms.select();

    newTms._builder->from(name());

    newTms._tuplet = std::make_shared<std::tuple<std::reference_wrapper<Ts>..., std::reference_wrapper<NewTypes>...>>(
        std::ref(ts)..., std::ref(args)...);

    return newTms;
}
template <typename... Ts>
Tms<Ts...> Tms<Ts...>::with(T& t)
{
    if (_builder) {
        _builder.reset();
    }
    _builder = std::make_shared<TFilterBuilder>();

    select();

    _builder->from(name());
    _instance = &t;
    return *this;
}

template <typename T>
constexpr bool hasGeo() {
    Tms<T> tms;
    std::vector<std::string> geo = tms.geoFieldsKeys();
    return !geo.empty();
}

template <typename T>
void selectUnique(std::shared_ptr<TFilterBuilder> builder) {
    Tms<T> tms;
    std::string modelName = tms.name(); // Ex: "models_geo_Road"

    std::vector<std::string> fields = tms.fieldsKeys();
    std::vector<std::string> geoFields = tms.geoFieldsKeys();
    std::vector<std::string> normalizeFields;

    for (const std::string& field : fields) {
        std::string fullName = modelName + "." + field;
        if (std::find(geoFields.begin(), geoFields.end(), field) != geoFields.end()) {
            normalizeFields.push_back("AsText(" + fullName + ") AS " + field);
        }
        else {
            normalizeFields.push_back(fullName);
        }
    }

    SelectClause clause;
    clause.columns = normalizeFields;
    clause.distinct = false;

    builder->addSelect(clause); // on ajoute les colonnes au SELECT global
}


template <typename... Ts>
void Tms<Ts...>::select() {
    if constexpr (sizeof...(Ts) == 1) {
        // Cas simple : un seul type
        using T = typename std::tuple_element<0, std::tuple<Ts...>>::type;
        selectUnique<T>(_builder);
    }
    else {
        // Détecter si au moins un type a une géométrie
        bool anyGeo = (hasGeo<Ts>() || ...);

        if (anyGeo) {
            std::apply([&](auto... args) {
                (..., selectUnique<decltype(args)>(_builder));
                }, std::tuple<Ts...>{});
        }
    }
}


template <typename... Ts>
void Tms<Ts...>::base_prepare()
{
    if (_isReady) { return; }

    if (_instance == nullptr)
    {
        _instance = new T();
    }
    if (_modelClass == nullptr)
    {
        _modelClass = &_instance->clazz();
    }
}

template <typename... Ts>
void Tms<Ts...>::prepare()
{
    if (_isReady) { return; }

    base_prepare();

    setup(_modelClass);

    initCallbacks();
}

template <typename... Ts>
bool Tms<Ts...>::exists() {
    base_prepare();
    return TmsDeep::exists(_modelClass);
}

// Get the model's class
template <typename... Ts>
Tclass* Tms<Ts...>::modelClass() {
    return _modelClass;
}

// Get the model's name
template <typename... Ts>
std::string Tms<Ts...>::name() {
    prepare();

    return _modelClass->getModelName();
}


template <typename... Ts>
std::vector<TtableField> Tms<Ts...>::tableFields() {
    prepare();

    return TmsAdapter::tableFields(_modelClass);
}
template <typename... Ts>
std::vector<std::vector<TtableField>> Tms<Ts...>::tableAllFields() {
    if (_tableAllFields.size() > 0) return _tableAllFields;

    // Pour chaque type T dans Ts, on crée un Tms<T> temporaire et on appelle fieldsKeys()
    // On utilise l'initialisation d'une liste pour forcer l'expansion du pack
    (void)std::initializer_list<int>{
        (_tableAllFields.push_back(Tms<Ts>{}.tableFields()), 0)... // virgule pour renvoyer 0
    };

    return _tableAllFields;
}

// Get the model's fields
template <typename... Ts>
std::vector<std::string> Tms<Ts...>::fieldsKeys() {
    prepare();

    return TmsDeep::fieldsKeys(_modelClass);
}

// Get the model's fields
template <typename... Ts>
std::vector<std::string> Tms<Ts...>::geoFieldsKeys() {
    prepare();

    return TmsDeep::geoFieldsKeys(_modelClass);
}

// Get the models fields keys
template <typename... Ts>
std::vector<std::vector<std::string>> Tms<Ts...>::allFieldsKeys() {
    std::vector<std::vector<std::string>> result;

    // Pour chaque type T dans Ts, on crée un Tms<T> temporaire et on appelle fieldsKeys()
    // On utilise l'initialisation d'une liste pour forcer l'expansion du pack
    (void)std::initializer_list<int>{
        (result.push_back(Tms<Ts>{}.fieldsKeys()), 0)... // virgule pour renvoyer 0
    };

    return result;
}

// Get the model's db name
template <typename... Ts>
std::string Tms<Ts...>::dbId() {
    prepare();

    return _modelClass->geTdb();
}

template <typename... Ts>
bool Tms<Ts...>::clear()
{
    if (exists())
    {
        bool result = TmsDeep::clear(_modelClass, {});
        if (result)
        {
            _isReady = false;
            return result;
        }
    }
    return false;
}


template <typename... Ts>
Tms<Ts...> Tms<Ts...>::lazy(bool v)
{
    _builder->lazyBuild = v;
    return *this;
}
template <typename... Ts>
Tlist<Ts...> Tms<Ts...>::ibuild()
{
    if (!_builder->lazyBuild) {
        return build();
    }
    return Tlist<Ts...>(std::make_shared<Tms<Ts...>>(*this));
}

template <typename... Ts>
Tlist<Ts...> Tms<Ts...>::build()
{
    prepare();

    return TitemLoad<Ts...>::get(std::make_shared<Tms<Ts...>>(*this), _builder);
}

template <typename... Ts>
void Tms<Ts...>::setup(Tclass* modelClass)
{
    _isReady = TmsAdapter::setup(modelClass);
}
