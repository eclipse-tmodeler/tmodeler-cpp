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

#include "field/TBaseField.h"

/*
 * About Model Field
 */
namespace ai::tambo::tmodeler {

template <typename Derived, typename T>
class TField : public BaseField {
protected:

public:
    TField(TFtype type);
    virtual ~TField() = default;

    // builder
    virtual Derived& len(int max);
    virtual Derived& defaults(T v);
    virtual Derived& onDelete(TF v);
    virtual Derived& constraint(TF v);
    template <typename... Args>
    Derived& constraints(Args... args);
    virtual Derived& primaryKey(bool v = true);
    virtual Derived& autoIncrement(bool v = true);
    virtual Derived& notNull(bool v = true);
    virtual Derived& unique(bool v = true);
    virtual Derived& check(bool v = true);
    virtual Derived& foreignKey(bool v = true);
    virtual Derived& defaultValue(bool v = true);
    virtual Derived& indexed(bool v = true);

    // Pure virtual methods - must be overridden in derived classes
    virtual T get() const = 0;
    virtual void set(T v) = 0;

    // override operators
    operator T() const;
    Derived& operator=(T&& i);
    Derived& operator=(const T& i);


    Tx operator+();
    Tx operator-();

    Tx operator>(const T& val);
    Tx operator<(const T& val);
    Tx operator==(const T* val);
    Tx operator==(const T& val);
    Tx operator!=(const T* val);
    Tx operator!=(const T& val);
    Tx operator>=(const T& val);
    Tx operator<=(const T& val);
    Tx operator%(const T& pattern);
    Tx operator>>=(const std::vector<T>& values);
    //
    /*
     * Not Ended
     *
     */
    Tx operator>(const BaseField& val);
    Tx operator<(const BaseField& val);
    Tx operator==(const BaseField& val);
    Tx operator!=(const BaseField& val);
    Tx operator>=(const BaseField& val);
    Tx operator<=(const BaseField& val);

    // override
    std::string toString() override;


    // filter aggr
    Derived& max() {
        this->aggr = Tx(this, Operator::Max);
        return static_cast<Derived&>(*this);
    }
    Derived& min() {
        this->aggr = Tx(this, Operator::Min);
        return static_cast<Derived&>(*this);
    }
    Derived& avg() {
        this->aggr = Tx(this, Operator::Avg);
        return static_cast<Derived&>(*this);
    }
    Derived& sum() {
        this->aggr = Tx(this, Operator::Sum);
        return static_cast<Derived&>(*this);
    }
    Derived& count() {
        this->aggr = Tx(this, Operator::Count);
        return static_cast<Derived&>(*this);
    }
    Derived& countDistinct() {
        this->aggr = Tx(this, Operator::CountDistinct);
        return static_cast<Derived&>(*this);
    }

    // These for Aggr field build
    Derived& max(BaseField& bf) {
        Tx(&bf, Operator::Max);
        return bf;
    }
    Derived& min(BaseField& bf) {
        Tx(&bf, Operator::Min);
        return bf;
    }
    Derived& avg(BaseField& bf) {
        Tx(&bf, Operator::Avg);
        return bf;
    }
    Derived& sum(BaseField& bf) {
        Tx(&bf, Operator::Sum);
        return bf;
    }
    Derived& count(BaseField& bf) {
        Tx(&bf, Operator::Count);
        return bf;
    }
    Derived& countDistinct(BaseField& bf) {
        Tx(&bf, Operator::CountDistinct);
        return bf;
    }
};

} // namespace ai::tambo::tmodeler
