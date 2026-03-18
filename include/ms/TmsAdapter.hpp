#pragma once

#include "core/Tob.hpp"

class TtableField;

class TmsAdapter
{
public:
    static bool setup(Tclass* modelClass);

    static std::vector<TtableField> tableFields(Tclass* modelClass);
};
