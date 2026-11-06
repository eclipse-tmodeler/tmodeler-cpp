/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#ifndef TMODELER_H
#define TMODELER_H

#include <iostream>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "TM.h"
#include "TMProvider.h"
#include "db/Tdb.h"
#include <functional>

namespace ai::tambo::tmodeler {

class TModeler {
private:
    using Callback = std::function<void(void)>;
    Callback _callback = nullptr;

public:

    TM::DbType _dbType;
    std::string _dbDir, _dbName, _mediaDir, _serverHost;

    static TModeler& start(); // with somme commun params
    static TM::Builder initialize(TM::DbType type, const std::string& dbDir, const std::string& dbName);
    static TModeler& getInstance();

    TModeler& init(std::unique_ptr<Tdb> db);
    Tdb& geTdb(const std::string& id, bool rw = false);

public:
    TModeler& dbReady(Callback cb);

private:
    TModeler& start(std::unique_ptr<Tdb> db);

private:
    friend std::unique_ptr<TModeler> std::make_unique<TModeler>();
    friend class TM::Builder;
    TModeler();

    std::unique_ptr<TMProvider> _provider;
    static std::unique_ptr<TModeler> _instance;

    mutable std::shared_mutex _mutex;
    std::unordered_map<std::string, std::unique_ptr<Tdb>> _dbs;
};


} // namespace ai::tambo::tmodeler
#endif // TMODELER_H
