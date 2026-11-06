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

#include <string>

#include "field/TFields.hpp"

namespace ai::tambo::tmodeler {

class TMserial : public Tserial {

    void deserialize(const Json& j, const char* key, std::any& field) override;
    void serialize(Json& j, const char* key, std::any field) override;
};

} // namespace ai::tambo::tmodeler
