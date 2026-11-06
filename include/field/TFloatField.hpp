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
class FloatFieldBase : public TField<Derived, double> {
public:
    FloatFieldBase() : TField<Derived, double>(TFtype::REAL)
    {
        TField<Derived, double>::defaults(BaseField::NULL_FLOAT);
    }

    double get() const override { return to<double>(TField<Derived, double>::getValue()); }
    void set(double v) override { TField<Derived, double>::setValue(v); }
};

class FloatField : public FloatFieldBase<FloatField> {
public:
    using TField::operator=;
    using TField::operator double;
};

} // namespace ai::tambo::tmodeler
