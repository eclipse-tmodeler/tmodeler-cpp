#pragma once

#include <any>
#include <string>
#include <chrono>
#include <vector>

#include "field/TField.hpp"

// Specialized field types

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
