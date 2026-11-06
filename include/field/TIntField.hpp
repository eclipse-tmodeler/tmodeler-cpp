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

#include <any>
#include <string>
#include <chrono>
#include <vector>

#include "field/TField.hpp"

// Specialized field types

namespace ai::tambo::tmodeler {

template <typename Derived>
class IntFieldBase : public TField<Derived, int> {
public:
    IntFieldBase() : TField<Derived, int>(TFtype::INTEGER)
    {
        TField<Derived, int>::defaults(BaseField::NULL_INT);
    }

    int get() const override
    {
        return to<int>(TField<Derived, int>::getValue());
    }
    void set(int v) override
    {
        TField<Derived, int>::setValue(v);
    }
};

class IntField : public IntFieldBase<IntField>
{
public:
    using TField::operator=;
    using TField::operator int;
};

} // namespace ai::tambo::tmodeler
