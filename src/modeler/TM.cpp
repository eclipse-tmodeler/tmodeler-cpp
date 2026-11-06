/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "modeler/TM.h"
#include "modeler/TModeler.h"

namespace ai::tambo::tmodeler {

TM::Builder::Builder(){}

TModeler& TM::getInstance() {
    return TModeler::getInstance();
}

TModeler& TM::Builder::getInstance() {
    return TModeler::getInstance();
}

TM::Builder& TM::Builder::type(DbType v) {
    getInstance()._dbType = v;
    return *this;
}

TM::Builder& TM::Builder::dbDir(const std::string& v) {
    getInstance()._dbDir = v;
    return *this;
}

TM::Builder& TM::Builder::dbName(const std::string& v) {
    getInstance()._dbName = v;
    return *this;
}

TM::Builder& TM::Builder::mediaDir(const std::string& v) {
    getInstance()._mediaDir = v;
    return *this;
}

TM::Builder& TM::Builder::host(const std::string& v) {
    getInstance()._serverHost = v;
    return *this;
}

TM::Builder& TM::Builder::provider(std::unique_ptr<TMProvider> v) {
    getInstance()._provider = std::move(v);
    return *this;
}

} // namespace ai::tambo::tmodeler
