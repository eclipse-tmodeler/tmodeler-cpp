/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */


#include "field/TBaseField.hpp"
#include "ms/TmsFilter.hpp"

namespace ai::tambo::tmodeler {

void TmsFilter::fillTx(Tx& x, std::string modelName, AnyList modelFields)
{
    BaseField* bf = nullptr;
    for (const auto& [key, field] : modelFields) {
        bf = baseField(field);

        x.notify(bf, modelName, modelName + "." + key);
    }
}



TqResult TmsFilter::query(Tclass* _modelClass, std::string sql)
{
    TqResult result;

    // Convertir la requête SQL en minuscules pour une comparaison insensible à la casse
    std::string sql_lower = sql;
    std::transform(sql_lower.begin(), sql_lower.end(), sql_lower.begin(), ::tolower);

    // Vérifie si la requête contient "select"
    if (sql_lower.find("select") != std::string::npos)
    {
        // Cas d'une requête de lecture
        std::string r = TModeler::getInstance().geTdb(_modelClass->geTdb()).executeQuery(sql);
        result.data = r;
        result.status = TqResult::TStatus::DONE;
    }
    else
    {
        // Cas d'une requête de modification
        bool b = TModeler::getInstance().geTdb(_modelClass->geTdb()).executeSQL(sql);
        result.status = b ? TqResult::TStatus::DONE : TqResult::TStatus::NOT_FOUND;
    }

    return result;
}

} // namespace ai::tambo::tmodeler
