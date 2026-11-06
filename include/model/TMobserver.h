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

namespace ai::tambo::tmodeler {

template<typename T>
class TMlistener {
public:
    virtual ~TMlistener() = default;

    virtual void onChanged(const T& value) = 0;

    virtual void onDeleted(const T& value) {}
};

} // namespace ai::tambo::tmodeler
