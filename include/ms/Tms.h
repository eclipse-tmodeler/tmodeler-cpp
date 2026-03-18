#pragma once

#include "core/Tclass.h"
#include "field/TFields.hpp"
#include "field/TBaseField.hpp"
#include "db/Tdb.h"
#include "db/Ttable.h"
#include <memory>
#include <string>
#include <vector>

#include "modeler/TModeler.h"
#include "ms/TFilterBuilder.h"
#include "Tlist.h"
#include "TmObserver.h"
#include "TResult.h"

template <typename... Ts>
class Tms : public Tob{
private:
    using T = typename std::tuple_element<0, std::tuple<Ts...>>::type;
    using TupleType = std::tuple<std::reference_wrapper<Ts>...>;
public:
    Tms(){}

    // Check if the model class exists (not null)
    bool exists();

    // Return the name of the model
    Tclass* modelClass();
    std::string name();
    std::vector<std::string> fieldsKeys();
    std::vector<std::vector<std::string>> allFieldsKeys();
    std::vector<TtableField> tableFields();
    std::vector<std::vector<TtableField>> tableAllFields();
    std::vector<std::string> geoFieldsKeys();
    std::string dbId();
    bool clear();
    bool migrate();

    Tlist<Ts...> all();
    Tlist<Ts...> get(KeysType& keys);
    T get(int k);
    T getById(int id);
    T getByKey(int lid);

    Tms<Ts...> join(JoinType type);
    template <typename... NewTypes>
    Tms<Ts..., NewTypes...> with(Ts&... ts, NewTypes&... args);
    Tms<Ts...> with(T& t);

    TqResult query(std::string sql);

    Tlist<Ts...> filter(Tx& x);
    Tlist<Ts...> filter(Tx&& x);
    Tlist<Ts...> filter(T& item, Tx& x);
    Tlist<Ts...> filter(std::shared_ptr<TupleType> tuplet, Tx& x);


    Tlist<Ts...> order(Tx& tx);
    Tlist<Ts...> order(T& item, Tx& x);
    Tlist<Ts...> order(std::shared_ptr<TupleType> tuplet, Tx& tx);
    template<typename... Args>
    Tlist<Ts...> group(Args&... args);
    void groupByItem(T& item, Tx& x);
    void groupByTuple(std::shared_ptr<TupleType> tuplet, Tx& tx);


    Tms<Ts...> lazy(bool v=true);
    Tlist<Ts...> ibuild();
    Tlist<Ts...> build();


    Tlist<Ts...> filter();
    Tlist<Ts...> limit(int count);


    template <typename... Ts2>
    int save(T& first, Ts2&... rest);
    template <typename... Ts2>
    int simulate(T* first, Ts2*... rest);
    template <typename... Ts2>
    int create(T* first, Ts2*... rest);
    template <typename... Ts2>
    int update(T& first, Ts2&... rest);
    template <typename... Ts2>
    int del(T& first, Ts2&... rest);

    int save(Tlist<T> instances);
    int create(Tlist<T*>& instances);
    int create(std::vector<T> instances);
    int update(Tlist<T> instances);

    int save(std::vector<T> instances);
    int update(std::vector<T> instances);
    int del(Tlist<T> instances);
    int del(std::vector<T> instances);



    void addObserver(const std::shared_ptr<TModelObserver>& observer);
    void removeObserver(const std::shared_ptr<TModelObserver>& observer);
    void onModelChange(Callback c);
    void onSave(Callback c);
    void onCreate(Callback c);
    void onUpdate(Callback c);
    void onDelete(Callback c);

private:
    std::vector<std::vector<TtableField>> _tableAllFields;

public:
    std::shared_ptr<TFilterBuilder> _builder = nullptr;
    std::shared_ptr<TupleType> _tuplet = nullptr;
    
private:
    bool _isReady = false;
    Tclass* _modelClass = nullptr;
    T* _instance = nullptr;

    std::shared_ptr<TModelCallback> _model_callback;

    JoinType joinType = JoinType::INNER;



    void setup(Tclass* modelClass);

    void base_prepare();
    void prepare();

    void initCallbacks();

public: //
    void select();

private:
    void fillTx(Tx& x, std::string modelName, AnyList modelFields);
    template <typename Tuple, std::size_t... Is>
    void applyImpl(Tuple& tuple, Tx& tx, std::index_sequence<Is...>);

    void apply(TupleType& tuple, Tx& tx);
};
