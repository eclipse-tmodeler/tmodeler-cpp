#pragma once
#include <string>
#include <any>
#include <sstream>
#include <iostream>
#include <map>

#include <vector>

#include "Ttable.h"

template <typename... Ts>
class Tlist;


using KeysType = std::vector<int>;

class TModelObserver {
public:
    virtual ~TModelObserver() {}

    virtual void onModelChange(KeysType& keys) = 0;
    virtual void onSave(KeysType& keys) = 0;
    virtual void onCreate(KeysType& keys) = 0;
    virtual void onUpdate(KeysType& keys) = 0;
    virtual void onDelete(KeysType& keys) = 0;
};

