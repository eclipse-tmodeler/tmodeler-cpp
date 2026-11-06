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

namespace ai::tambo::tmodeler {

class BoolField : public IntFieldBase<BoolField> {
public:

    BoolField() : IntFieldBase<BoolField>() {}

    bool is() const {
        return IntFieldBase::get()==1;
    }

    void set(bool v) {
        IntFieldBase::set(v?1:0);
    }

    virtual BoolField& defaults(bool v){
        set(v);
        return *this;
    }

    virtual BoolField& value(bool v){
        set(v);
        return *this;
    }

    operator bool() const {
        return is();
    }
    BoolField& operator=(const bool& v) {
        set(v);
        return static_cast<BoolField&>(*this);
    }
    BoolField& operator=(bool&& v) {
        set(v);
        return static_cast<BoolField&>(*this);
    }

    Tx operator!=(const bool& val) {
        Tx x(this, Operator::NotEqual);
        x.value = val;
        return x;
    }
    Tx operator==(const bool& val) {
        Tx x(this, Operator::Equal);
        x.value = val;
        return x;
    }
};

} // namespace ai::tambo::tmodeler
