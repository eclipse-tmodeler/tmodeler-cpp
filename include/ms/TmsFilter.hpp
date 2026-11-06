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

#include "Tms.h"
#include "field/TField.h"

#include "ms/TitemCreate.hpp"

#include "core/Tob.hpp"
#include "field/Tx.h"

namespace ai::tambo::tmodeler {

class TmsFilter : public Tob
{
public:
    static void fillTx(Tx& x, std::string modelName, AnyList modelFields);
    static TqResult query(Tclass* _modelClass, std::string sql);
};


template <typename... Ts>
Tlist<Ts...> Tms<Ts...>::filter()
{
    if (_builder == nullptr)
    {
        _builder = std::make_shared<TFilterBuilder>();
        _builder->from(name());
    }

    return ibuild();
}


template <typename... Ts>
Tlist<Ts...> Tms<Ts...>::filter(Tx& tx)
{
    return _tuplet != nullptr ? filter(_tuplet, tx) : _instance != nullptr ? filter(*_instance, tx) : filter();
}
template <typename... Ts>
Tlist<Ts...> Tms<Ts...>::filter(Tx&& tx)
{
    return _tuplet != nullptr ? filter(_tuplet, tx) : _instance != nullptr ? filter(*_instance, tx) : filter();
}

template <typename... Ts>
Tlist<Ts...> Tms<Ts...>::limit(int count)
{
    return filter();
}

template <typename... Ts>
Tlist<Ts...> Tms<Ts...>::order(Tx& tx)
{
    return _tuplet != nullptr ? order(_tuplet, tx) : order(*_instance, tx);
}

template <typename... Ts>
template<typename... Args>
Tlist<Ts...> Tms<Ts...>::group(Args&... args)
{
    static_assert((std::is_base_of_v<BaseField, Args> && ...), "All args must derive from BaseField");

    T& item = *_instance;

    // Pour chaque arg, on construit un Tx et on ajoute au builder
    ((
        [&]() {
            Tx tx(&args, Operator::GroupBy);

            if (_tuplet != nullptr) groupByTuple(_tuplet, tx);
            else groupByItem(*_instance, tx);

        }()
            ), ...);

    return ibuild();
}

template <typename... Ts>
void Tms<Ts...>::groupByItem(T& item, Tx& tx)
{
    fillTx(tx, name(), item.clazz().getFields());

    _builder->addGroupBy(tx.toString());
}

template <typename... Ts>
void Tms<Ts...>::groupByTuple(std::shared_ptr<TupleType> tuplet, Tx& tx)
{
    apply(*tuplet, tx);

    _builder->addGroupBy(tx.toString());
}


template <typename... Ts>
Tms<Ts...> Tms<Ts...>::join(JoinType type)
{
    joinType = type;
    return *this;
}


template <typename... Ts>
TqResult Tms<Ts...>::query(std::string sql)
{
    return TmsFilter::query(modelClass(), sql);
}

} // namespace ai::tambo::tmodeler
