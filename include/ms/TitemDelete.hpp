#pragma once

#include "TitemDelete.h"

#include "ms/TmsDeep.hpp"
#include "ms/Tms.h"

template <typename... Ts>
template <typename T>
int TitemDelete<Ts...>::del(std::shared_ptr<Tms<Ts...>> tms, std::shared_ptr<TFilterBuilder> builder, Tlist<T> instances)
{
    std::string jsonData = T::data(instances);

    bool result = TmsDeep::deleteData(tms->modelClass(), jsonData, instances.get(0).keyName());

    if (result)
    {
        TmsDeep::notifyChange(tms->modelClass(), instances.keys(), false, false, true);

        return instances.size();
    }

    return 0;
}
