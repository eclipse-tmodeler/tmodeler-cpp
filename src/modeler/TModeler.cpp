/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "modeler/TModeler.h"

#include <iomanip>
#include <mutex>
#include <shared_mutex>
#include <thread>

#include "core/Tob.hpp"
#include "db/Tsqlite.h"
#include "modeler/TM.h"

namespace ai::tambo::tmodeler {


std::unique_ptr<TModeler> TModeler::_instance = nullptr;

TModeler::TModeler(){}

// Singleton Method
TModeler& TModeler::getInstance() {
    if (!_instance) {
        _instance = std::make_unique<TModeler>();
    }
    return *_instance;
}


TModeler& TModeler::start()
{
    // init with some communs params

    return TModeler::getInstance();
}

// Factory Method
TM::Builder TModeler::initialize(TM::DbType type, const std::string& dbDir, const std::string& dbName) {
    return TM::Builder()
        .type(type)
        .dbDir(dbDir)
        .dbName(dbName);
}



TModeler& TModeler::dbReady(Callback cb) {
    _callback = cb;
    return *this;
}

TModeler& TModeler::init(std::unique_ptr<Tdb> db) {
    if (_callback) {
        std::thread([this, db=std::move(db)]() mutable {
            try {
                start(std::move(db));
                _callback();
            }
            catch (const std::exception& e) {
                Log::d(std::string("Exception in async init: ") + e.what());
            }
        }).detach();
    }
    else {
        try {
            start(std::move(db));
        }
        catch (const std::exception& e) {
            Log::d(std::string("Exception in sync init: ") + e.what());
        }
    }

    return *this;
}

TModeler& TModeler::start(std::unique_ptr<Tdb> db) {
    if (!db->open())
    {
        return *this;
    }

    std::unique_lock lock(_mutex); 
    std::string id = db->_dbName;
    _dbs[id] = std::move(db);
    return *this;
}

Tdb& TModeler::geTdb(const std::string& id, bool rw) {
    if (id.empty())
    {
        if (_dbs.size()==0){
            throw std::runtime_error("Tdb not found: " + id);
        }
        if (rw) {
            std::unique_lock lock(_mutex);
            try
            {
                return *(_dbs.begin()->second);
            }catch (...){
                throw std::runtime_error("Tdb not found: " + id);
            }
        }
        else {
            std::shared_lock lock(_mutex);
            try
            {
                return *(_dbs.begin()->second);
            }
            catch (...) {
                throw std::runtime_error("Tdb not found: " + id);
            }
        }
    }
    if (rw) {
        std::unique_lock lock(_mutex);  
        auto it = _dbs.find(id);
        if (it == _dbs.end()) {
            throw std::runtime_error("Tdb not found: " + id);
        }
        return *(it->second);
    }
    else {
        std::shared_lock lock(_mutex); 
        auto it = _dbs.find(id);
        if (it == _dbs.end()) {
            throw std::runtime_error("Tdb not found: " + id);
        }
        return *(it->second);
    }
}

/*
TModeler& TModeler::init(std::unique_ptr<Tdb> db) {
    std::string id = db->id();
    _dbs[id] = std::move(db);  // transfert de propriété
    return *this;
}*/

/*
Tdb& TModeler::geTdb(std::string id)
{
    auto it = _dbs.find(id);
    if (it == _dbs.end()) {
        throw std::runtime_error("Tdb not found: " + id);
    }
    return it->second;
}*/

} // namespace ai::tambo::tmodeler
