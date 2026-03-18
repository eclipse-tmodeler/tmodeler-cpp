#include "model/TMserializer.h"

#include "field/TBaseField.hpp"


void TMserial::deserialize(const Json& j, const char* key, std::any& field) {
    BaseField* base = baseField(field);

    if (base) {
        Tserial::deserialize(j, key, base->refValue());
        return;
    }

    ///Tserial::deserialize(j, key, field); // TModel don't need this
}

void TMserial::serialize(Json& j, const char* key, std::any field) {
    BaseField* base = baseField(field);

    if (base) {
        Tserial::serialize(j, key, base->getValue());
        return;  
    }

    //Tserial::serialize(j, key, field);   // TModel don't need this
}
