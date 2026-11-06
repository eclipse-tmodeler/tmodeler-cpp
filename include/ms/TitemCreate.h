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
#include <any>
#include <sstream>
#include <iostream>

#include <vector>

#include "ms/TFilterBuilder.h"
#include "ms/Tlist.hpp"

namespace ai::tambo::tmodeler {

template <typename... Ts>
class Tms;

template <typename... Ts>
class TitemCreate {
public:
    template <typename T>
    static int create(std::shared_ptr<Tms<Ts...>> tms, std::shared_ptr<TFilterBuilder> builder, Tlist<T*>& instances);
};

} // namespace ai::tambo::tmodeler
