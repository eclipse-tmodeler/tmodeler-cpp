#pragma once

#include <string>
#include <any>
#include <sstream>
#include <iostream>

#include <vector>

#include "ms/TFilterBuilder.h"
#include "ms/Tlist.hpp"

template <typename... Ts>
class Tms;

template <typename... Ts>
class TitemCreate {
public:
    template <typename T>
    static int create(std::shared_ptr<Tms<Ts...>> tms, std::shared_ptr<TFilterBuilder> builder, Tlist<T*>& instances);
};
