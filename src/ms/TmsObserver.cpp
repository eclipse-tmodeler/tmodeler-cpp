
#include "field/TBaseField.hpp"
#include "ms/TmsFilter.hpp"

void TmsFilter::fillTx(Tx& x, std::string modelName, AnyList modelFields)
{
    BaseField* bf = nullptr;
    for (const auto& [key, field] : modelFields) {
        bf = baseField(field);

        x.notify(bf, modelName, modelName + "." + key);
    }
}
