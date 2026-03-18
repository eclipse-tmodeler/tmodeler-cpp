#pragma once

#include "TitemLoad.h"

#include "ms/TmsDeep.hpp"
#include "ms/Tms.h"


template <typename... Ts>
Tlist<Ts...> TitemLoad<Ts...>::get(std::shared_ptr<Tms<Ts...>> tms, std::shared_ptr<TFilterBuilder> builder)
{
    using T = typename std::tuple_element<0, std::tuple<Ts...>>::type;


    builder->tableFields = tms->tableAllFields();
    builder->fieldsKeys = tms->allFieldsKeys();
    std::string data_ = TmsDeep::read(tms->modelClass(), builder);
    Json parsed = Json::parse(data_);

    if constexpr (sizeof...(Ts) == 1) {
        auto list = Tob::parse<T>(data_);
        return Tlist<T>(tms, list);
    }
    else {
        auto list = Tob::parseTuples<Ts...>(data_);
        return Tlist<Ts...>(tms, list);
    }
}
