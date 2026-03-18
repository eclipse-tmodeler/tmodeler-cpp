#pragma once

#include <string>
#include <any>
#include <sstream>
#include <iostream>

#include <vector>

#include "ms/Tlist.h"
#include "ms/TFilterBuilder.h"

template <typename... Ts>
class Tms;

template <typename... Ts>
class TitemUpdate {
public:
    template <typename T>
    static int put(std::shared_ptr<Tms<Ts...>> tms, std::shared_ptr<TFilterBuilder> builder, Tlist<T> instances);
};
