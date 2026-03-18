#ifndef TM_H
#define TM_H

#include <string>
#include <memory>

#include "TMProvider.h"

class TModeler;

class TM {
public:
    // Type de base de données
    enum class DbType { SQLITE, MYSQL, POSTGRESQL };

    // Pattern Builder pour configurer TModeler
    class Builder {
    public:
        Builder();

        Builder& type(DbType v);
        Builder& dbDir(const std::string& v);
        Builder& dbName(const std::string& v);
        Builder& mediaDir(const std::string& v);
        Builder& host(const std::string& v);
        Builder& provider(std::unique_ptr<TMProvider> v);

        TModeler& getInstance();
    };

    static TModeler& getInstance();
};

#endif // TM_H
