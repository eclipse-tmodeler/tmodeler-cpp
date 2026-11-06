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

#include "model/TModel.h"
#include <iostream>


namespace ai::tambo::tmodeler {

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

} // namespace ai::tambo::tmodeler
