#pragma once

#include <string>
#include <any>
#include <sstream>
#include <iostream>

#include <vector>

#include "ms/TFilterBuilder.h"

template <typename... Ts>
class Tms;

template <typename... Ts>
class TitemDelete {
public:
    template <typename T>
    static int del(std::shared_ptr<Tms<Ts...>> tms, std::shared_ptr<TFilterBuilder> builder, Tlist<T> instances);
};
