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

#include <vector>
#include <stdexcept>

#include "field/Tx.h"

#include "db/TdbObserver.h"

namespace ai::tambo::tmodeler {

template <typename... Ts>
class Tms;


namespace detail {

    // Compte le nombre d'occurrences du type T dans un std::tuple<Types...>
    template<typename T, typename Tuple>
    struct count_in_tuple;

    template<typename T, typename... Types>
    struct count_in_tuple<T, std::tuple<Types...>> {
        static constexpr std::size_t value = (0 + ... + (std::is_same_v<T, Types> ? 1 : 0));
    };

} // namespace detail


template <typename... Ts>
class Tlist : public std::vector<
    typename std::conditional<
    sizeof...(Ts) == 1,
    typename std::tuple_element<0, std::tuple<Ts...>>::type,
    std::tuple<Ts...>
    >::type
> {
public:
    using BaseType = typename std::conditional<
        sizeof...(Ts) == 1,
        typename std::tuple_element<0, std::tuple<Ts...>>::type,
        std::tuple<Ts...>
    >::type;

    using BaseVector = std::vector<BaseType>;
    using ValueType = BaseType;
    using T = typename std::tuple_element<0, std::tuple<Ts...>>::type;
    using TupleType = std::tuple<std::reference_wrapper<Ts>...>;
private:
    std::shared_ptr<Tms<Ts...>> _tms;

public:
    Tlist() = default;
    Tlist(std::shared_ptr<Tms<Ts...>> tms);
    Tlist(std::shared_ptr<Tms<Ts...>> tms, const std::vector<BaseType>& vec);

    void tms(std::shared_ptr<Tms<Ts...>> tms) { _tms = tms; }

    void add(const ValueType& value);

    ValueType& get(size_t index);
    void set(size_t index, const ValueType& value);
    void add(size_t index, const ValueType& value);

    template <typename Sp>
    const Sp& get(size_t i) const;
    template <typename Sp>
    Sp& get(size_t i);

    void remove(size_t index);
    bool empty() const;
    void clear();

    bool remove(const ValueType& value);
    bool contains(const ValueType& value) const;
    int indexOf(const int k) const;
    int indexOf(const ValueType& value) const;
    int lastIndexOf(const ValueType& value) const;
    size_t size() const;

    Tlist<Ts...> subList(size_t fromIndex, size_t toIndex) const;

    std::vector<ValueType> vector() const { return *this; }

    KeysType keys();

    Tlist<Ts...> filter(Tx&& x);

    
    Tlist<Ts...> order(Tx&& tx, Tx&& tx2);
    Tlist<Ts...> order(Tx&& tx);
    template<typename... Args>
    Tlist<Ts...> group(Args&... args);

    Tlist<Ts...> build();


    Tlist<Ts...> filter(const std::function<bool(const ValueType&)>& predicate) const;


    std::string data();
    ValueType first_cp();
    ValueType& first();
    ValueType& last();
    template <typename Sp>
    Sp& first();
    template <typename Sp>
    Sp& last();

private:
    template <typename... Types, size_t... Is>
    Tlist<Types...> select_impl(std::index_sequence<Is...>) const;

public:
    template <typename T>
    Tlist<T> select() const;

    template <typename First, typename Second, typename... Rest>
    Tlist<First, Second, Rest...> select() const;
};

} // namespace ai::tambo::tmodeler
