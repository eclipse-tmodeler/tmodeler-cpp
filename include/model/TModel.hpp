#pragma once

#include "model/TModel.h"
#include <iostream>


template <typename T>
int TModel<T>::save() {
    return isCreated() ? update() : create();
}
template <typename T>
int TModel<T>::create()
{
    return _tms.create(static_cast<T*>(this));
}
template <typename T>
int TModel<T>::update()
{
    return _tms.update(static_cast<T&>(*this));
}
template <typename T>
int TModel<T>::del()
{
    return _tms.del(static_cast<T&>(*this));
}

template <typename T>
void TModel<T>::clone()
{
    key().setValue(BaseField::NULL_INT);
    // must return
}
template <typename T>
void TModel<T>::reset()
{
    key().setValue(BaseField::NULL_INT);
}

template <typename T>
bool TModel<T>::isCreated()
{
    return !key().empty();
}

template <typename T>
bool TModel<T>::saveOnLinking()
{
    return false;
}

template <typename T>
bool TModel<T>::isNull()
{
    return _null;
}

template <typename T>
void TModel<T>::null(bool v)
{
    _null = v;
}

template <typename T>
bool TModel<T>::empty()
{
    auto d = data();
    return isNull() || d=="null" || d=="{}";
}
