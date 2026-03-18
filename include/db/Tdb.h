#pragma once
#include <cassert>
#include <map>
#include <string>
#include <vector>
#include <memory>

#include "Ttable.h"

class TFilterBuilder;

class TdbDriver {
public:
    virtual ~TdbDriver() {}

    virtual bool open(const std::string& dbName) = 0;
    virtual void close() = 0;
    virtual bool executeSQL(const std::string& sql) = 0;
    virtual int64_t last_insert_rowid() = 0;
    virtual std::string executeQuery(const std::string& query) = 0;
    virtual std::string executeQuery(const std::string& query, std::any builder) = 0;
    virtual bool deleteDatabase(const std::string& dbName) = 0;

    virtual bool createTable(const std::string& tableScript) = 0;
    virtual bool dropTable(const std::string& tableName) = 0;
    virtual bool tableExists(const std::string& tableName) = 0;

    virtual std::string tableSchema(const std::string& tableName) = 0;
};

// Classe Tdb de base
class Tdb{
    std::unique_ptr<TdbDriver> _driver;
    std::map<std::string, std::unique_ptr<Ttable>> _tables;
public:
    std::string _dbDir;
    std::string _dbName;
    std::string _mediaDir;
    std::string _serverHost;

    explicit Tdb(std::unique_ptr<TdbDriver> driver)
        : _driver(std::move(driver)) {
        assert(_driver != nullptr); 
    }

    // for db
    virtual bool open();
    // for table
    virtual bool tableExists(const std::string& name);
    virtual bool createTable(std::unique_ptr<Ttable> table);
    virtual bool dropTable(const std::string& name, KeysType keys);
    virtual bool tableSaves(const std::string& name);


    int64_t last_insert_rowid();
    bool executeSQL(const std::string& sql);
    std::string executeQuery(const std::string& query);
    std::string executeQuery(const std::string& query, std::shared_ptr<TFilterBuilder> builder);


    void addObserver(std::string modelName, const std::shared_ptr<TModelObserver>& observer);
    void removeObserver(std::string modelName, const std::shared_ptr<TModelObserver>& observer);
    void notifyChange(std::string modelName, KeysType keys, bool onCreate = false, bool onUpdate = false, bool onDelete = false);


    enum class Type { SQLITE, SPATIALITE, MYSQL, POSTGRESQL };

    class Builder {
    public:
        Builder();

        Builder& type(Type v);
        Builder& accept(const std::string& pkg);
        Builder& dbDir(const std::string& v);
        Builder& dbName(const std::string& v);
        Builder& mediaDir(const std::string& v);
        Builder& host(const std::string& v);

        std::unique_ptr<Tdb> get();

    private:
        std::unique_ptr<TdbDriver> _driver;
        std::unique_ptr<Tdb> _db;
    };
};
