#pragma once

#include "TitemUpdate.h"

#include "ms/TmsDeep.hpp"
#include "ms/Tms.h"


template <typename... Ts>
template <typename T>
int TitemUpdate<Ts...>::put(std::shared_ptr<Tms<Ts...>> tms, std::shared_ptr<TFilterBuilder> builder, Tlist<T> instances)
{
    if (instances.size()==0)
    {
        return 0;
    }

    std::string jsonData = Tob::data(instances);

    bool result = TmsDeep::updateData(tms->modelClass(), jsonData, instances.get(0).keyName());

    if (result)
    {
        TmsDeep::notifyChange(tms->modelClass(), instances.keys(), false, true, false);

        return instances.size();
    }

    return 0;
}
