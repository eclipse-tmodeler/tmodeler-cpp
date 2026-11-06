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

#include "core/Tob.hpp"

namespace ai::tambo::tmodeler {

class TtableField;

class TmsAdapter
{
public:
    static bool setup(Tclass* modelClass);

    static std::vector<TtableField> tableFields(Tclass* modelClass);
};

} // namespace ai::tambo::tmodeler
