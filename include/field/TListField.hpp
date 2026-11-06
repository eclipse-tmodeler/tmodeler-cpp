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
#include "field/TJsonField.hpp"
#include "ms/Tms.hpp"

namespace ai::tambo::tmodeler {

template <typename T>
class ListField : public JsonFieldBase<ListField<T>> {
protected:
    std::shared_ptr<Tms<T>> _tms = nullptr;
public:
    ListField() : JsonFieldBase<ListField<T>>()
    {
        // Bloc is necessary to avoid self Model bounce
        static thread_local bool alreadyInLf = false;
        if (!alreadyInLf) {
            try {
                alreadyInLf = true;
                BaseField::_targetModel_ = Tob::init<T>().clazz3(); //cette instance non static va mourir, c'est pas vraiment utilise
                BaseField::_targetField = Tob::init<T>().keyName();
            }
            catch (...) {}
            alreadyInLf = false;
        }
    }

    std::shared_ptr<Tms<T>> tms()
    {
        if (_tms == nullptr)
        {
            _tms = std::make_shared<Tms<T>>();
        }
        return _tms;
    }

    void set(std::vector<T*>& v) {
        JsonFieldBase<ListField<T>>::clear();
        for (T* ptr : v)
        {
            if (ptr->saveOnLinking())
            {
                ptr->save(); //cette sauvegarde duplique les elts : puisque on ne fais qu'envoyer des copies ici
            }
            if (ptr->isCreated() && !JsonFieldBase<ListField<T>>::contains(ptr->key().get()))
            {
                JsonFieldBase<ListField<T>>::add(ptr->key().get());
            }
        }
    }

    void set(std::vector<T> v) {
        JsonFieldBase<ListField<T>>::clear();
        for (int i=0;i<v.size();i++)
        {
            if (v[i].saveOnLinking())
            {
                v[i].save();
            }
            if (v[i].isCreated() && !JsonFieldBase<ListField<T>>::contains(v[i].key().get()))
            {
                JsonFieldBase<ListField<T>>::add(v[i].key().get());
            }
        }
    }

    Tlist<T> list() {
        std::vector<int> keys;
        for (int i = 0; i < JsonFieldBase<ListField<T>>::size(); i++)
        {
            keys.push_back(JsonFieldBase<ListField<T>>::get(i));
        }
        T i;
        return tms()->with(i).filter(i.key() >>= keys);
    }

    T get(int i) {
        return tms()->get(JsonFieldBase<ListField<T>>::get(i));
    }

    virtual ListField& defaults(std::vector<T> v) {
        set(v);
        return *this;
    }

    void add(T& v) {
        auto all = list();
        all.add(v);
        set(all);
    }

    void add(int i, T& v) {
        auto all = list();
        all.add(i, v);
        set(all);
    }

    void set(int i, T& v) {
        auto all = list();
        all.set(i, v);
        set(all);
    }

    void clear() {
        auto all = list();
        all.clear();
        set(all);
    }

    operator Tlist<T>() {
        return list();
    }
    ListField& operator=(std::vector<T*>& v) {
        set(v);
        return static_cast<ListField&>(*this);
    }
    ListField& operator+=(T& v) {
        add(v);
        return static_cast<ListField&>(*this);
    }
    ListField& operator=(std::initializer_list<T*> l) {
        auto all = list();
        for (auto v : l) {
            all.add(*v);
        }
        set(all);
        return static_cast<ListField&>(*this);
    }
};

} // namespace ai::tambo::tmodeler
