/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "db/Tdb.h"

#include "core/Tob.hpp"
#include "db/Tspatialite.h"
#include "db/Tsqlite.h"
#include "ms/Tlist.h"

#include "ms/TFilterBuilder.h"

namespace ai::tambo::tmodeler {

bool Tdb::open()
{
    return _driver->open(_dbName);
}
bool Tdb::tableExists(const std::string& name)
{
    return _driver->tableExists(name);
}
bool Tdb::tableSaves(const std::string& name)
{
    auto it = _tables.find(name);
    return it != _tables.end();
}
bool Tdb::createTable(std::unique_ptr<Ttable> table)
{
    if (tableExists(table->_driver->getName()))
    {
        if (!tableSaves(table->_driver->getName()))
        {
            _tables[table->_driver->getName()] = std::move(table);
        }
        return true;
    }
    bool is =  _driver->createTable(table->_driver->toScript());
    if (is)
    {
        _tables[table->_driver->getName()] = std::move(table);
        
    }
    return is;
}
bool Tdb::dropTable(const std::string& name, KeysType keys)
{
    bool ret = _driver->dropTable(name);
    if (ret)
    {
        notifyChange(name, keys, false, false, true);
    }
    return ret;
}





void Tdb::notifyChange(std::string modelName, KeysType keys, bool onCreate, bool onUpdate, bool onDelete) {
    auto it = _tables.find(modelName);
    if (it != _tables.end()) {
        it->second->notifyChange(keys, onCreate, onUpdate, onDelete);
    }
}

void Tdb::addObserver(std::string modelName, const std::shared_ptr<TModelObserver>& observer) {
    auto it = _tables.find(modelName);
    if (it != _tables.end()) {
        it->second->addObserver(observer);
    }
}

void Tdb::removeObserver(std::string modelName, const std::shared_ptr<TModelObserver>& observer) {
    auto it = _tables.find(modelName);
    if (it != _tables.end()) {
        it->second->removeObserver(observer);
    }
}




int64_t Tdb::last_insert_rowid()
{
    return _driver->last_insert_rowid();
}

bool Tdb::executeSQL(const std::string& sql)
{
    return _driver->executeSQL(sql);
}

std::string Tdb::executeQuery(const std::string& query)
{
    return _driver->executeQuery(query);
}

std::string Tdb::executeQuery(const std::string& query, std::shared_ptr<TFilterBuilder> builder)
{
    return _driver->executeQuery(query, builder);
}



// ----------- Tdb::Builder ------------

Tdb::Builder::Builder() = default;

std::unique_ptr<TdbDriver> createDriver(Tdb::Type type) {
    switch (type) {
    case Tdb::Type::SQLITE:
        return std::make_unique<TsqliteDriver>();
    case Tdb::Type::SPATIALITE:
        return std::make_unique<TspatialiteDriver>();
    default:
        throw std::invalid_argument("Unknown DB type");
    }
}

Tdb::Builder& Tdb::Builder::type(Type v) {
    _db = std::make_unique<Tdb>(createDriver(v));
    return *this;
}

/** Juste une fonction racourcir pour lier model & db : donc c'est un racourcir*/
Tdb::Builder& Tdb::Builder::accept(const std::string& pkg) {
    //if (_db) _db->_dbDir = pkg;
    return *this;
}

Tdb::Builder& Tdb::Builder::dbDir(const std::string& v) {
    if (_db) _db->_dbDir = v;
    return *this;
}

Tdb::Builder& Tdb::Builder::dbName(const std::string & v) {
    if (_db) _db->_dbName = v;
    return *this;
}

Tdb::Builder& Tdb::Builder::mediaDir(const std::string & v) {
    if (_db) _db->_mediaDir = v;
    return *this;
}

Tdb::Builder& Tdb::Builder::host(const std::string & v) {
    if (_db) _db->_serverHost = v;
    return *this;
}

std::unique_ptr<Tdb> Tdb::Builder::get() {
    return std::move(_db);
}

} // namespace ai::tambo::tmodeler
