#pragma once

#include "Tms.h"
#include "field/TField.h"

#include "core/Tob.hpp"
#include "field/Tx.h"

#include "TmsFilter.hpp"

class TmsDeep : public Tob
{
public:
    static bool exists(Tclass* _modelClass);
    static bool clear(Tclass* _modelClass, KeysType keys);
    static std::string read(Tclass* _modelClass, std::shared_ptr<TFilterBuilder> builder);
    static int64_t lastInsertRowId(Tclass* _modelClass);
    static void notifyChange(Tclass* _modelClass, KeysType keys, bool onCreate, bool onUpdate, bool onDelete);
    static bool insertData(Tclass* _modelClass, const std::string j);
    static bool updateData(Tclass* _modelClass, const std::string jsonData, const std::string key);
    static std::string join(const std::vector<std::string>& vec, const std::string& delim);
    static bool deleteData(Tclass* _modelClass, const std::string jsonData, const std::string key);
    static void addObserver(Tclass* _modelClass, const std::shared_ptr<TModelObserver>& observer);
    static void removeObserver(Tclass* _modelClass, const std::shared_ptr<TModelObserver>& observer);
    static std::vector<std::string> fieldsKeys(Tclass* _modelClass);
    static std::vector<std::string> geoFieldsKeys(Tclass* _modelClass);
};


template <typename... Ts>
Tlist<Ts...> Tms<Ts...>::order(T& item, Tx& tx)
{
    fillTx(tx, name(), item.clazz().getFields());

    _builder->orderBy({
        { tx.toString() }
        });

    return ibuild();
}
template <typename... Ts>
Tlist<Ts...> Tms<Ts...>::order(std::shared_ptr<TupleType> tuplet, Tx& tx) {
    apply(*tuplet, tx);

    if (tx.hasIntegratedFunc()) {
        auto [orderExpr, direction] = tx.splitOrderExpr();

        std::string alias = "__distance";

        _builder->addSelect(orderExpr + " AS " + alias);
        _builder->orderBy({ {alias + " " + direction} });
    }
    else
    {
        _builder->orderBy({
            { tx.toString() }
        });
    }

    return ibuild();
}

template <typename... Ts>
Tlist<Ts...> Tms<Ts...>::filter(std::shared_ptr<TupleType> tuplet, Tx& tx) {
    apply(*tuplet, tx);

    if (tx.isJoining())
    {
        _builder->joins({
            { strJoin(joinType), tx.targetModel(), tx.toString() }
            });
    }
    else if (tx.isAggr())
    {
        _builder->having(tx.toString());
    }
    else
    {
        _builder->where({
            {"", "", tx.toString() }
            });
    }

    return filter();
}
template <typename... Ts>
Tlist<Ts...> Tms<Ts...>::filter(T& item, Tx& tx) {
    fillTx(tx, name(), item.clazz().getFields());
    if (tx.isAggr())
    {
        _builder->having(tx.toString());
    }
    else
    {
        _builder->where({
            {"", "", tx.toString() }
            });
    }

    return filter();
}
template <typename... Ts>
void Tms<Ts...>::fillTx(Tx& x, std::string modelName, AnyList modelFields)
{
    TmsFilter::fillTx(x, modelName, modelFields);
}
template <typename... Ts>
template <typename Tuple, std::size_t... Is>
void Tms<Ts...>::applyImpl(Tuple& tuple, Tx& tx, std::index_sequence<Is...>) {
    (void)std::initializer_list<int>{
        (
            [&]() {
                auto& elt = std::get<Is>(tuple).get();
                using Ti = std::decay_t<decltype(elt)>;
                auto name = (Tms<Ti>()).name();
                auto fields = elt.clazz().getFields();

                fillTx(tx, name, fields);
            }(),
                0
                )...
    };
}
template <typename... Ts>
void Tms<Ts...>::apply(TupleType& tuple, Tx& tx) {
    applyImpl(tuple, tx, std::index_sequence_for<Ts...>{});
}

template <typename... Ts>
bool Tms<Ts...>::migrate()
{
    auto backup = all().data();
    if (clear())
    {
        auto all_ = Tob::parse<T>(backup);
        for (auto e : all_)
        {
            e.create();
        }
    }
    return false;
}
