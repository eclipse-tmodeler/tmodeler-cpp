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
#include <vector>
#include <optional>
#include "core/Tob.hpp"


/*
 * T query Result
 *
 */
namespace ai::tambo::tmodeler {

class TqResult {
public:
    enum class TStatus {
        DONE,
        NOT_FOUND
    };

public:
    TqResult::TStatus status = TqResult::TStatus::DONE;
    std::string data = "";
};

} // namespace ai::tambo::tmodeler
