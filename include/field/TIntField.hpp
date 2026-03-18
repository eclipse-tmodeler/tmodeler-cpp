#pragma once

#include <any>
#include <string>
#include <chrono>
#include <vector>

#include "field/TField.hpp"

// Specialized field types

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
