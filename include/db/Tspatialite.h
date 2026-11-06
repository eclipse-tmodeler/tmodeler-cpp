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

#include <string>

#include "Tsqlite.h"

namespace ai::tambo::tmodeler {

class TspatialiteDriver : public TsqliteDriver {
public:

    bool ensureSpatialMetadataInitialized();

    bool open(const std::string& dbName) override;
    void close() override;
    bool dropTable(const std::string& tableName) override; 
};


void test_sqlite_and_spatialite();

} // namespace ai::tambo::tmodeler
