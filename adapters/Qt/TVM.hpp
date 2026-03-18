#ifndef TVM_HPP
#define TVM_HPP

#include "TViewModel.hpp"
#include "TItem.hpp"



#define TM_QML_ITEM \
Q_PROPERTY(QVariantMap data READ get WRITE set) \
    Q_INVOKABLE QVariantMap get() const { return TItem::get(); } \
    Q_INVOKABLE void set(const QVariantMap& data) { TItem::set(data); } \
    using TItem::TItem;


#define TM_QML_SIGNALS \
    void errorOccurred(const QString& message); \
    void countChanged();


#define TM_QML_VM(Type, ItemType) \
\
    public: \
    using TViewModel::TViewModel; \
\
    Tlist<Type> all(){ \
        return TViewModel::all(); \
} \
\
    Q_INVOKABLE QVariantMap data(int index) { \
        return TViewModel::data(index); \
} \
\
    Q_INVOKABLE bool create(const QVariantMap& data) { \
        return TViewModel::create(data); \
} \
\
    Q_INVOKABLE bool update(int index, const QVariantMap& data) { \
        return TViewModel::update(index, data); \
} \
\
    Q_INVOKABLE bool remove(int index) { \
        return TViewModel::remove(index); \
} \
\
    Q_INVOKABLE int count() const { \
        return TViewModel::count(); \
} \
\
    Q_INVOKABLE void refresh() { \
        TViewModel::refresh(); \
} \
\
    Q_INVOKABLE QObject* get(int index) { \
        return TViewModel::get<ItemType>(index); \
} \
\
    Q_INVOKABLE bool isEmpty() const { \
        return TViewModel::isEmpty(); \
} \
\
    Q_INVOKABLE int indexOf(int key) const { \
        return TViewModel::indexOf(key); \
} \
\
    Q_INVOKABLE QVariant field(int index, const QString& name) const { \
        return TViewModel::field(index, name); \
} \
\
    Q_INVOKABLE void filter(const QString& key, const QVariant& value) { \
        TViewModel::filter(key, value); \
} \
\
    Q_INVOKABLE void loadMore(int offset, int limit) { \
        TViewModel::loadMore(offset, limit); \
}


#define TM_QML_VM_COMPLETED(Type, ItemType) \
\
    protected: \
    void emit_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>()){ \
        emit dataChanged(topLeft, bottomRight, roles); \
} \
    void emit_errorOccurred(const QString& message){ \
        emit errorOccurred(message); \
} \
    void emit_countChanged(){ \
        emit countChanged(); \
} \
\
    public: \
    using TViewModel::TViewModel; \
\
    Tlist<Type> all(){ \
        return TViewModel::all(); \
} \
\
    Q_INVOKABLE QVariantMap data(int index) { \
        return TViewModel::data(index); \
} \
\
    Q_INVOKABLE bool create(const QVariantMap& data) { \
        return TViewModel::create(data); \
} \
\
    Q_INVOKABLE bool update(int index, const QVariantMap& data) { \
        return TViewModel::update(index, data); \
} \
\
    Q_INVOKABLE bool remove(int index) { \
        return TViewModel::remove(index); \
} \
\
    Q_INVOKABLE int count() const { \
        return TViewModel::count(); \
} \
\
    Q_INVOKABLE void refresh() { \
        TViewModel::refresh(); \
} \
\
    Q_INVOKABLE QObject* get(int index) { \
        return TViewModel::get<ItemType>(index); \
} \
\
    Q_INVOKABLE bool isEmpty() const { \
        return TViewModel::isEmpty(); \
} \
\
    Q_INVOKABLE int indexOf(int key) const { \
        return TViewModel::indexOf(key); \
} \
\
    Q_INVOKABLE QVariant field(int index, const QString& name) const { \
        return TViewModel::field(index, name); \
} \
\
    Q_INVOKABLE void filter(const QString& key, const QVariant& value) { \
        TViewModel::filter(key, value); \
} \
\
    Q_INVOKABLE void loadMore(int offset, int limit) { \
        TViewModel::loadMore(offset, limit); \
}



#define TM_QML_VM2(T, TItemType)                                               \
public:                                                                       \
    using TViewModel::TViewModel;                                             \
                                                                              \
    Tlist<T> all() {                                                          \
        return TViewModel::all();                                             \
}                                                                         \
                                                                              \
    Q_INVOKABLE QVariantMap data(int index) {                                 \
        return TViewModel::data(index);                                       \
}                                                                         \
                                                                              \
    Q_INVOKABLE bool create(const QVariantMap& data) {                        \
        return TViewModel::create(data);                                      \
}                                                                         \
                                                                              \
    Q_INVOKABLE bool update(int index, const QVariantMap& data) {            \
        return TViewModel::update(index, data);                               \
}                                                                         \
                                                                              \
    Q_INVOKABLE bool remove(int index) {                                      \
        return TViewModel::remove(index);                                     \
}                                                                         \
                                                                              \
    Q_INVOKABLE int count() const {                                           \
        return TViewModel::count();                                           \
}                                                                         \
                                                                              \
    Q_INVOKABLE void refresh() {                                              \
        TViewModel::refresh();                                                \
}                                                                         \
                                                                              \
    Q_INVOKABLE QObject* get(int index) {                                     \
        return TViewModel::get<TItemType>(index);                             \
}


#endif // TVM_HPP
