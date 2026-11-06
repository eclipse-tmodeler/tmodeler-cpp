/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#pragma once

#include "TitemDelete.h"

#include "ms/TmsDeep.hpp"
#include "ms/Tms.h"

namespace ai::tambo::tmodeler {

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

} // namespace ai::tambo::tmodeler
