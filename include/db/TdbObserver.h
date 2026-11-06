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
#include <map>

#include <vector>

#include "Ttable.h"

namespace ai::tambo::tmodeler {

template <typename... Ts>
class Tlist;


using KeysType = std::vector<int>;

class TModelObserver {
public:
    virtual ~TModelObserver() {}

    virtual void onModelChange(KeysType& keys) = 0;
    virtual void onSave(KeysType& keys) = 0;
    virtual void onCreate(KeysType& keys) = 0;
    virtual void onUpdate(KeysType& keys) = 0;
    virtual void onDelete(KeysType& keys) = 0;
};

} // namespace ai::tambo::tmodeler

