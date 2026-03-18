#pragma once

#include <string>
#include "Tdb.h"

struct sqlite3;
class TFilterBuilder;

class TsqliteDriver : public TdbDriver {
protected:
    sqlite3* db;
    int lastId;

public:
    TsqliteDriver();
    virtual ~TsqliteDriver();

    bool open(const std::string& dbName) override;
    void close() override;
    bool createTable(const std::string& tableScript) override;
    bool dropTable(const std::string& tableName) override;
    bool tableExists(const std::string& tableName) override;
    bool executeSQL(const std::string& sql) override;
    int64_t last_insert_rowid() override;
    std::string executeQuery(const std::string& query) override;
    std::string executeQuery(const std::string& query, std::any builder) override;
    bool deleteDatabase(const std::string& dbName) override;
    std::string tableSchema(const std::string& tableName) override;
};
