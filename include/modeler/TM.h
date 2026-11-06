/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#ifndef TM_H
#define TM_H

#include <string>
#include <memory>

#include "TMProvider.h"

namespace ai::tambo::tmodeler {

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


} // namespace ai::tambo::tmodeler
#endif // TM_H
