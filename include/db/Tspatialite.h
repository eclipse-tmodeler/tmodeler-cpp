#pragma once

#include <string>

#include "Tsqlite.h"

class TspatialiteDriver : public TsqliteDriver {
public:

    bool ensureSpatialMetadataInitialized();

    bool open(const std::string& dbName) override;
    void close() override;
    bool dropTable(const std::string& tableName) override; 
};


void test_sqlite_and_spatialite();
