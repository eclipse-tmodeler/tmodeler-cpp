#pragma once

#include "TitemCreate.h"

#include "ms/TmsDeep.hpp"
#include "ms/Tms.h"


template <typename... Ts>
template <typename T>
int TitemCreate<Ts...>::create(std::shared_ptr<Tms<Ts...>> tms, std::shared_ptr<TFilterBuilder> builder, Tlist<T*>& instances)
{
    std::string jsonData = Tob::data(instances.vector());

    bool result = TmsDeep::insertData(tms->modelClass(), jsonData);
    int64_t lastid = TmsDeep::lastInsertRowId(tms->modelClass());

    int lastIndex = instances.size()-1;
    for (int i=0;i<instances.size();i++)
    {
        instances.at(i)->key().set(lastid - lastIndex + i);
    }

    if (result)
    {
        TmsDeep::notifyChange(tms->modelClass(), instances.keys(), true, false, false);

        return instances.size();
    }

    return 0;
}
