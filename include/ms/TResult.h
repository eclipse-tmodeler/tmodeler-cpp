#pragma once

#include <string>
#include <vector>
#include <optional>
#include "core/Tob.hpp"


/*
 * T query Result
 *
 */
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
