#pragma once

#include <string>

#include "field/TFields.hpp"
#include "ms/Tms.h"
#include "model/TMserializer.h"


#define TMClass Tclass::with(*this)
#define TModelSuper &(TModel::clazz())
#define TMSuper(T) &(T::clazz())
#define TM_super(T) &(T::clazz())

#define TM_SCHEMA(CLASSNAME, PACKAGE, ...) \
private: \
    Tclass _class = TMClass; \
public: \
    static Tms<CLASSNAME> tms; \
    Tclass& clazz() override { \
        return _class.super(TModelSuper).package(PACKAGE).fields(__VA_ARGS__); \
    }

#define TM_MANAGER(CLASSNAME) \
    Tms<CLASSNAME> CLASSNAME::tms;

#define TM_SCHEMA_EXPLICIT(SUPERCLASS, CLASSNAME, PACKAGE, ...) \
private: \
    Tclass _class = TMClass; \
public: \
    static Tms<CLASSNAME> tms; \
    Tclass& clazz() override { \
        return _class.super(SUPERCLASS).package(PACKAGE).fields(__VA_ARGS__); \
    }

#define TM_SERIAL(SERIAL_EXPR) \
protected: \
    TMserial _serializer = SERIAL_EXPR; \
public: \
    Tserial& serial() override { \
        return _serializer; \
    }






template <typename T>
class TModel : public Tob{
    TM_SCHEMA_EXPLICIT(TMSuper(Tob), T, "", TF(id), TF(_id), TF(meta))
    TM_SERIAL(TMserial())

    Tms<T> _tms;

    IntField id;
    IntField _id = init<IntField>().primaryKey().autoIncrement().notNull();
    JsonField meta = init<JsonField>();

    virtual const IntField& key() const{ return _id; }
    virtual IntField& key() { return _id; }
    virtual std::string keyName() { return TF(_id).first;}

    int save();
    int create();
    int update();
    int del();

    void clone();
    void reset();

    virtual bool isCreated();
    virtual bool saveOnLinking();

    virtual bool isNull();
    virtual void null(bool v);
    virtual bool empty();


    bool operator==(const TModel<T>& v) const{
        if (key().empty() || v.key().empty())
        {
            return false;
        }
        return key().get() == v.key().get();
    }

    bool operator==(const int k) const {
        if (key().empty())
        {
            return false;
        }
        return key().get() == k;
    }
private:
    bool _null = false;
};
