/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#ifndef TMPROVIDER_H
#define TMPROVIDER_H

namespace ai::tambo::tmodeler {

class TMProvider {
public:
    virtual void onServerStatusChange(bool isConnected) = 0;
    virtual void onCommit() = 0;
    virtual ~TMProvider() = default;
};


} // namespace ai::tambo::tmodeler
#endif // TMPROVIDER_H