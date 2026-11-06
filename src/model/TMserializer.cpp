/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "model/TMserializer.h"

#include "field/TBaseField.hpp"


namespace ai::tambo::tmodeler {

void TMserial::deserialize(const Json& j, const char* key, std::any& field) {
    BaseField* base = baseField(field);

    if (base) {
        Tserial::deserialize(j, key, base->refValue());
        return;
    }

    ///Tserial::deserialize(j, key, field); // TModel don't need this
}

void TMserial::serialize(Json& j, const char* key, std::any field) {
    BaseField* base = baseField(field);

    if (base) {
        Tserial::serialize(j, key, base->getValue());
        return;  
    }

    //Tserial::serialize(j, key, field);   // TModel don't need this
}

} // namespace ai::tambo::tmodeler
