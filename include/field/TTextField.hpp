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
class TextFieldBase : public TField<Derived, std::string> {
public:
    TextFieldBase() : TField<Derived, std::string>(TFtype::TEXT)
    {
        TField<Derived, std::string>::defaults(BaseField::NULL_STRING);
    }

    std::string get() const override { return to<std::string>(TField<Derived, std::string>::getValue()); }
    void set(std::string v) override { TField<Derived, std::string>::setValue(v); }
};

class TextField : public TextFieldBase<TextField>
{
public:
    using TField::operator=;
    using TField::operator std::string;
};

} // namespace ai::tambo::tmodeler
