#pragma once

#include <string>

#include "field/TFields.hpp"

class TMserial : public Tserial {

    void deserialize(const Json& j, const char* key, std::any& field) override;
    void serialize(Json& j, const char* key, std::any field) override;
};
