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

#include "field/TFields.hpp"


namespace ai::tambo::tmodeler {

inline BaseField* baseField(std::any field)
{
    if (!field.has_value())
        return nullptr;

    if (field.type() == typeid(BaseField*))      return std::any_cast<BaseField*>(field);

    if (field.type() == typeid(IntField*))      return std::any_cast<IntField*>(field);
    if (field.type() == typeid(FloatField*))    return std::any_cast<FloatField*>(field);
    if (field.type() == typeid(TextField*))     return std::any_cast<TextField*>(field);
    if (field.type() == typeid(TimeField*))     return std::any_cast<TimeField*>(field);
    if (field.type() == typeid(BoolField*))     return std::any_cast<BoolField*>(field);
    if (field.type() == typeid(JsonField*))     return std::any_cast<JsonField*>(field);

    return nullptr;
}

} // namespace ai::tambo::tmodeler
