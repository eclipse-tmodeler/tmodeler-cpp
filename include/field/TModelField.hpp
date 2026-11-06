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
#include <chrono>
#include <vector>

#include "field/TField.hpp"
#include "field/TIntField.hpp"
#include "ms/Tms.hpp"

namespace ai::tambo::tmodeler {

template <typename T>
class ModelField : public IntFieldBase<ModelField<T>> {
protected:
    std::shared_ptr<Tms<T>> _tms = nullptr;
    T* _instance = nullptr;
    bool _cache = false;
public:
    ModelField() : IntFieldBase<ModelField<T>>()
    {
        TField<ModelField, int>::foreignKey();

        // Bloc is necessary to avoid self Model bounce
        static thread_local bool alreadyIn = false;
        if (!alreadyIn) {
            try {
                alreadyIn = true;
                BaseField::_targetModel_ = Tob::instance<T>().clazz3();
                BaseField::_targetField = Tob::instance<T>().keyName();
            }
            catch (...) {}
            alreadyIn = false;
        }
    }

    std::shared_ptr<Tms<T>> tms()
    {
        if (_tms==nullptr)
        {
            _tms = std::make_shared<Tms<T>>();
        }
        return _tms;
    }

    int ref() {
        return IntFieldBase<ModelField<T>>::get();
    }

    void setCache(T* instance) {
        _instance = instance;
    }

    T* getCache() {
        return _instance;
    }

    T get() {
        if (_cache && _instance) {
            return *_instance;
        }
        return tms()->get(IntFieldBase<ModelField<T>>::get());
    }

    T model() {
        return tms()->get(IntFieldBase<ModelField<T>>::get());
    }

    void set(T& v) {
        if (v.saveOnLinking())
        {
            v.save();
        }
        if(v.isCreated())
        {
            IntFieldBase<ModelField<T>>::set(v.key().get());
        }
        if (_cache) _instance = &v;
    }

    virtual ModelField& defaults(T v) {
        set(v);
        return *this;
    }

    virtual ModelField& cache(bool v=true) {
        _cache = v;
        return *this;
    }

    virtual bool useCache() {
        return _cache;
    }

    virtual ModelField& onDelete(TF v) {
        IntFieldBase<ModelField<T>>::onDelete(v);
        return *this;
    }

    // operators
    ModelField& operator=(T& v) {
        set(v);
        return *this;
    }
    operator T() {
        return model();
    }
    Tx operator==(T& item) {
        Tx x(this, Operator::Equal);
        x.value = Tms<T>().name() + "." + item.keyName();
        return x;
    }
    //#
    Tx operator==(const int* val) {
        return TField<ModelField<T>, int>::operator==(val);
    }
    Tx operator!=(const int* val) {
        return TField<ModelField<T>, int>::operator!=(val);
    }
    Tx operator==(const int& val) {
        return TField<ModelField<T>, int>::operator==(val);
    }
    /*



    Tx operator>(const T& val) {
        Tx x(this, Operator::GreaterThan);
        x.value = val;
        return x;
    }
    Tx operator<(const T& val) {
        Tx x(this, Operator::LessThan);
        x.value = val;
        return x;
    }
    Tx operator!=(const T& val) {
        Tx x(this, Operator::NotEqual);
        x.value = val;
        return x;
    }
    Tx operator>=(const T& val) {
        Tx x(this, Operator::GreaterThanEqual);
        x.value = val;
        return x;
    }
    Tx operator<=(const T& val) {
        Tx x(this, Operator::LessThanEqual);
        x.value = val;
        return x;
    }*/
};

} // namespace ai::tambo::tmodeler
