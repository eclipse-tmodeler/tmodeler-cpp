/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#ifndef TOBSERVER_H
#define TOBSERVER_H

#include "Tserial.h"
#include "Tclass.h"

/*
 *Tob reflexion
 */
namespace ai::tambo::tmodeler {

class Treflexion {
public:
    virtual ~Treflexion() = default;
    Treflexion() {}

    virtual Tserial* serial_() = 0;
    virtual Tserial& serial() = 0;
    virtual Tclass& clazz() = 0;
};


} // namespace ai::tambo::tmodeler
#endif // TOBSERVER_H
