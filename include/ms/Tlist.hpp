/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "ms/Tlist.h"

#include "Tms.hpp"

#include <utility>

namespace ai::tambo::tmodeler {

template <typename... Ts>
Tlist<Ts...>::Tlist(std::shared_ptr<Tms<Ts...>> tms) : std::vector<BaseType>(), _tms(tms){}

template <typename... Ts>
Tlist<Ts...>::Tlist(std::shared_ptr<Tms<Ts...>> tms, const std::vector<BaseType>& vec) : std::vector<BaseType>(vec), _tms(tms) {}

template <typename... Ts>
void Tlist<Ts...>::add(const ValueType& value) {
    this->push_back(value);
}

template <typename... Ts>
typename Tlist<Ts...>::ValueType& Tlist<Ts...>::get(size_t index) {
    if (index >= this->size())
        throw std::out_of_range("Index out of range");
    return (*this)[index];
}

template <typename... Ts>
template <typename Sp>
Sp& Tlist<Ts...>::get(size_t i){
    static_assert((std::is_same_v<T, Ts> || ...), "T must be one of Ts...");

    if (i >= this->size())
        throw std::out_of_range("Index out of range");

    if constexpr (sizeof...(Ts) == 1) {
        return (*this)[i];
    }
    else {
        return std::get<Sp>((*this)[i]);
    }
}

template <typename... Ts>
template <typename Sp>
const Sp& Tlist<Ts...>::get(size_t i) const {
    static_assert((std::is_same_v<T, Ts> || ...), "T must be one of Ts...");

    if (i >= this->size())
        throw std::out_of_range("Index out of range");

    if constexpr (sizeof...(Ts) == 1) {
        return (*this)[i];
    }
    else {
        return std::get<Sp>((*this)[i]);
    }
}

template <typename... Ts>
void Tlist<Ts...>::set(size_t index, const ValueType& value) {
    if (index >= this->size())
        throw std::out_of_range("Index out of range");
    (*this)[index] = value;
}

template <typename... Ts>
void Tlist<Ts...>::add(size_t index, const ValueType& value) {
    if (index > this->size())
        throw std::out_of_range("Index out of range");

    this->insert(this->begin() + index, value);
}

template <typename... Ts>
void Tlist<Ts...>::remove(size_t index) {
    if (index >= this->size())
        throw std::out_of_range("Index out of range");
    this->erase(this->begin() + index);
}

template <typename... Ts>
bool Tlist<Ts...>::empty() const {
    return BaseVector::empty();
}

template <typename... Ts>
void Tlist<Ts...>::clear() {
    BaseVector::clear();
}




template <typename... Ts>
bool Tlist<Ts...>::remove(const ValueType& value) {
    auto it = std::find(this->begin(), this->end(), value);
    if (it != this->end()) {
        this->erase(it);
        return true;
    }
    return false;
}

template <typename... Ts>
bool Tlist<Ts...>::contains(const ValueType& value) const {
    return std::find(this->begin(), this->end(), value) != this->end();
}

template <typename... Ts>
int Tlist<Ts...>::indexOf(const ValueType& value) const {
    if constexpr (sizeof...(Ts) == 1) {
        for (int i = 0; i < size(); i++) {
            if (this->at(i) == value)
                return i;
        }
    }
    return -1;
}
template <typename... Ts>
int Tlist<Ts...>::indexOf(const int k) const {
    for (int i=0;i<size();i++)
    {
        if (get<T>(i) == k)
        {
            return i;
        }
    }
    return -1;
}

template <typename... Ts>
int Tlist<Ts...>::lastIndexOf(const ValueType& value) const {
    auto it = std::find(this->rbegin(), this->rend(), value);
    if (it != this->rend())
        return static_cast<int>(this->size() - 1 - std::distance(this->rbegin(), it));
    return -1;
}

template <typename... Ts>
size_t Tlist<Ts...>::size() const {
    return BaseVector::size();
}

template <typename... Ts>
KeysType Tlist<Ts...>::keys() {
    KeysType keys_;
    for (int i=0;i<size();i++)
    {
        auto& element = get<T>(i);

        if constexpr (std::is_pointer<T>::value) {
            keys_.push_back(element->key().get());
        }
        else {
            keys_.push_back(element.key().get());
        }
    }
    return keys_;
}

template <typename... Ts>
Tlist<Ts...> Tlist<Ts...>::subList(size_t fromIndex, size_t toIndex) const {
    if (fromIndex > toIndex || toIndex > this->size())
        throw std::out_of_range("Invalid sublist range");
    return Tlist<Ts...>(this->begin() + fromIndex, this->begin() + toIndex);
}



template <typename... Ts>
Tlist<Ts...> Tlist<Ts...>::filter(const std::function<bool(const typename Tlist<Ts...>::ValueType&)>& predicate) const {
    /* T T T ::: IMPORTANT */
    /*
    Tlist<T> result = Tlist<T>(_tms);
    for (const auto& item : *this) {
        if (predicate(item)) {
            result.add(item);
        }
    }
    return result;*/
    return *this;
}




template <typename... Ts>
Tlist<Ts...> Tlist<Ts...>::filter(Tx&& tx) // start by dispatching T H I S
{
    return _tms->filter(tx);
}


template <typename... Ts>
Tlist<Ts...> Tlist<Ts...>::order(Tx&& tx, Tx&& tx2)
{
    return _tms->order(tx);
}
template <typename... Ts>
Tlist<Ts...> Tlist<Ts...>::order(Tx&& tx)
{
    return _tms->order(tx);
}
template <typename... Ts>
template<typename... Args>
Tlist<Ts...> Tlist<Ts...>::group(Args&... args)
{
    static_assert((std::is_base_of_v<BaseField, Args> && ...), "All args must derive from BaseField");
    return _tms->group(args...);
}

template <typename... Ts>
Tlist<Ts...> Tlist<Ts...>::build()
{
    return _tms->build();
}

/*
template <typename... Ts>
Tlist<Ts...> Tlist<Ts...>::group(Tx&& tx)
{
    return _tms->orderBy(tx);
}

template <typename... Ts>
Tlist<Ts...> Tlist<Ts...>::orderBy(Tx&& tx)
{
    return _tms->orderBy(tx);
}*/


/*
template <typename... Ts>
std::string Tlist<Ts...>::data()
{
    return Tob::data<T>(*this);
}
*/
template <typename... Ts>
std::string Tlist<Ts...>:: data()
{
    if constexpr (sizeof...(Ts) == 1) {
        return Tob::data<T>(*this);
    }
    else {
        Json result = Json::array();

        for (const auto& tup : *this) {
            Json row = Json::array();

            std::apply([&row](const Ts&... elems) {
                (
                    row.push_back(Json::parse(
                        /*Tob::data<std::decay_t<decltype(elems)>>(
                            std::vector<std::decay_t<decltype(elems)>>{ elems }
                        )*/
                        (std::vector<std::decay_t<decltype(elems)>>{ elems }).at(0).data()
                    )),
                    ...
                    );
                }, tup);

            result.push_back(row);
        }

        return result.dump(4);
    }
}

template <typename... Ts>
typename Tlist<Ts...>::ValueType Tlist<Ts...>::first_cp()
{
    T obj;
    obj.null(true);
    return empty() ? obj : get(0);
}
template <typename... Ts>
typename Tlist<Ts...>::ValueType& Tlist<Ts...>::first()
{
    T obj;
    obj.null(true);
    return empty() ? obj : get(0); 
}
template <typename... Ts>
typename Tlist<Ts...>::ValueType& Tlist<Ts...>::last()
{
    T obj;
    obj.null(true);
    return empty() ? obj : get(size() - 1);
}
template <typename... Ts>
template <typename Sp>
Sp& Tlist<Ts...>::first()
{
    Sp obj;
    obj.null(true);
    return empty() ? obj : get<Sp>(0);
}
template <typename... Ts>
template <typename Sp>
Sp& Tlist<Ts...>::last()
{
    Sp obj;
    obj.null(true);
    return empty() ? obj : get<Sp>(size() - 1);
}






template<typename... Ts>
template<typename T>
Tlist<T> Tlist<Ts...>::select() const {
    static_assert(detail::count_in_tuple<T, std::tuple<Ts...>>::value == 1,
        "Type must occur exactly once in Tlist");

    Tlist<T> result;
    for (const auto& val : *this) {
        if constexpr (sizeof...(Ts) == 1) {
            result.push_back(val);
        }
        else {
            result.push_back(std::get<T>(val));
        }
    }
    return result;
}

template<typename... Ts>
template <typename First, typename Second, typename... Rest>
Tlist<First, Second, Rest...> Tlist<Ts...>::select() const {

    Tlist<First, Second, Rest...> result;

    for (const auto& row : *this) {
        result.push_back(std::make_tuple(
            std::get<First>(row),
            std::get<Second>(row),
            std::get<Rest>(row)...
        ));
    }
    return result;
}

} // namespace ai::tambo::tmodeler
