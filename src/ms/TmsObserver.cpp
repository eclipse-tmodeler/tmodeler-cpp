/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */


#include "field/TBaseField.hpp"
#include "ms/TmsFilter.hpp"

namespace ai::tambo::tmodeler {

void TmsFilter::fillTx(Tx& x, std::string modelName, AnyList modelFields)
{
    BaseField* bf = nullptr;
    for (const auto& [key, field] : modelFields) {
        bf = baseField(field);

        x.notify(bf, modelName, modelName + "." + key);
    }
}

} // namespace ai::tambo::tmodeler
