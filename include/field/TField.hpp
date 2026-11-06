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

#include "field/TField.h"
#include <iostream>

namespace ai::tambo::tmodeler {

template <typename Derived, typename T>
TField<Derived, T>::TField(TFtype type) : BaseField() { _type = type; }

// builder functions
template <typename Derived, typename T>
Derived& TField<Derived, T>::len(int max) {
    _maxLen = max;
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename T>
Derived& TField<Derived, T>::defaults(T v) {
    _value = v;
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename T>
Derived& TField<Derived, T>::onDelete(TF v) {
    _onDelete = v;
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename T>
Derived& TField<Derived, T>::constraint(TF v) {
    _constraints.addIfNotExist(v);
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename T>
template <typename... Args>
Derived& TField<Derived, T>::constraints(Args... args) {
    (constraint(args), ...);
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename T>
Derived& TField<Derived, T>::primaryKey(bool v) {
    if (v) {
        _constraints.addIfNotExist(TF::PRIMARY_KEY);
    }
    else {
        _constraints.removeIfExist(TF::PRIMARY_KEY);
    }
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename T>
Derived& TField<Derived, T>::autoIncrement(bool v) {
    if (v) {
        _constraints.addIfNotExist(TF::AUTOINCREMENT);
    }
    else {
        _constraints.removeIfExist(TF::AUTOINCREMENT);
    }
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename T>
Derived& TField<Derived, T>::notNull(bool v) {
    if (v) {
        _constraints.addIfNotExist(TF::NOT_NULL);
    }
    else {
        _constraints.removeIfExist(TF::NOT_NULL);
    }
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename T>
Derived& TField<Derived, T>::unique(bool v) {
    if (v) {
        _constraints.addIfNotExist(TF::UNIQUE);
    }
    else {
        _constraints.removeIfExist(TF::UNIQUE);
    }
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename T>
Derived& TField<Derived, T>::check(bool v) {
    if (v) {
        _constraints.addIfNotExist(TF::CHECK);
    }
    else {
        _constraints.removeIfExist(TF::CHECK);
    }
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename T>
Derived& TField<Derived, T>::foreignKey(bool v) {
    if (v) {
        _constraints.addIfNotExist(TF::FOREIGN_KEY);
    }
    else {
        _constraints.removeIfExist(TF::FOREIGN_KEY);
    }
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename T>
Derived& TField<Derived, T>::defaultValue(bool v) {
    if (v) {
        _constraints.addIfNotExist(TF::DEFAULT);
    }
    else {
        _constraints.removeIfExist(TF::DEFAULT);
    }
    return static_cast<Derived&>(*this);
}

template <typename Derived, typename T>
Derived& TField<Derived, T>::indexed(bool v) {
    if (v) {
        _constraints.addIfNotExist(TF::INDEX);
    }
    else {
        _constraints.removeIfExist(TF::INDEX);
    }
    return static_cast<Derived&>(*this);
}




// operators
template <typename Derived, typename T>
TField<Derived, T>::operator T() const {
    return get();
}
template <typename Derived, typename T>
Derived& TField<Derived, T>::operator=(T&& v) {
    set(v);
    return static_cast<Derived&>(*this);
}
template <typename Derived, typename T>
Derived& TField<Derived, T>::operator=(const T& v) {
    set(v);
    return static_cast<Derived&>(*this);
}



template <typename Derived, typename T>
Tx TField<Derived, T>::operator+() {
    Tx x(this, Operator::Asc);
    //x.value = val;
    return x;
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator-() {
    Tx x(this, Operator::Desc);
    //x.value = val;
    return x;
}

template <typename Derived, typename T>
Tx TField<Derived, T>::operator>(const T& val) {
    Tx x(this, Operator::GreaterThan);
    x.value = val;
    return x;
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator<(const T& val) {
    Tx x(this, Operator::LessThan);
    x.value = val;
    return x;
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator==(const T* val) {
    return isNull();
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator==(const T& val) {
    Tx x(this, Operator::Equal);
    x.value = val;
    return x;
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator!=(const T* val) {
    return !isNull();
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator!=(const T& val) {
    Tx x(this, Operator::NotEqual);
    x.value = val;
    return x;
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator>=(const T& val) {
    Tx x(this, Operator::GreaterThanEqual);
    x.value = val;
    return x;
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator<=(const T& val) {
    Tx x(this, Operator::LessThanEqual);
    x.value = val;
    return x;
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator%(const T& pattern) {
    Tx x(this, Operator::Like);
    x.value = pattern;
    return x;
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator>>=(const std::vector<T>& values)
{
    Tx x(this, Operator::In);
    x.value = vectorToString(values);
    return x;
}
//
template <typename Derived, typename T>
Tx TField<Derived, T>::operator>(const BaseField& val) {
    Tx x(this, Operator::GreaterThan);
    //x.value = val;
    return x;
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator<(const BaseField& val) {
    Tx x(this, Operator::LessThan);
    //x.value = val;
    return x;
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator==(const BaseField& val) {
    Tx x(this, Operator::Equal);
    //x.value = val;
    return x;
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator!=(const BaseField& val) {
    Tx x(this, Operator::NotEqual);
    //x.value = val;
    return x;
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator>=(const BaseField& val) {
    Tx x(this, Operator::GreaterThanEqual);
    //x.value = val;
    return x;
}
template <typename Derived, typename T>
Tx TField<Derived, T>::operator<=(const BaseField& val) {
    Tx x(this, Operator::LessThanEqual);
    //x.value = val;
    return x;
}




template <typename Derived, typename T>
std::string TField<Derived, T>::toString() {
    try {
        if (_value.type() == typeid(int)) {
            return std::to_string(std::any_cast<int>(_value));
        }
        else if (_value.type() == typeid(std::string)) {
            return std::any_cast<std::string>(_value);
        }
        else if (_value.type() == typeid(double)) {
            std::ostringstream oss;
            oss << std::any_cast<double>(_value);
            return oss.str();
        }
    }
    catch (const std::bad_any_cast&) {
        return "Invalid Cast";
    }
    return "Unsupported Type";
}

} // namespace ai::tambo::tmodeler
