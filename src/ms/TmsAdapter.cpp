/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */


#include "ms/TmsAdapter.hpp"
#include "db/Ttable.h"
#include "field/TBaseField.hpp"

namespace ai::tambo::tmodeler {

bool TmsAdapter::setup(Tclass* modelClass)
{
    std::string modelPackage = modelClass->getPackage();

    std::string className = modelClass->name();

    std::string modelName = modelClass->getModelName();

    auto fields = modelClass->getFields();

    std::vector<TtableField> modelFields;
    std::vector<std::string> primaryKeys;

    TtableField* tField;

    for (auto& [key, field] : fields) {
        BaseField* bField = baseField(field);

        if (!bField) continue;

        if (bField->getConstraints().isPrimaryKey()) {
            primaryKeys.push_back(key);
        }
    }

    for (auto& [key, field] : fields) {
        BaseField* bField = baseField(field);

        if (!bField) continue;

        std::string fieldName = key;
        TFtype fieldType = bField->getType();
        TFconstraints fieldConstraints = bField->getConstraints();
        TF fieldOnDelete = bField->getOnDelete();
        std::any fieldDefaultValue = bField->getValue();
        int fieldMaxLen = bField->getMaxLen();
        std::string fieldTargetField = bField->getTargetField();

        TFtype adjustedType = (fieldType == TFtype::TEXT && fieldMaxLen > 0)
            ? TFtype::VARCHAR
            : fieldType;

        tField = new TtableField(fieldName, tf_str(adjustedType));

        if (fieldConstraints.isPrimaryKey() && primaryKeys.size() > 1)
        {
            fieldConstraints.removeIfExist(TF::PRIMARY_KEY);
        }

        if (fieldConstraints.isForeignKey())
        {
            Tclass fieldTargetModel = bField->getTargetModel();

            fieldConstraints.removeIfExist(TF::FOREIGN_KEY);
            tField->targetField(fieldTargetField);
            tField->onDelete(tf_str(fieldOnDelete));
            tField->targetTable(fieldTargetModel.getModelName());
        }

        if (auto* geo = dynamic_cast<GeoField*>(bField)) {
            tField->geoType(GeoField::typeToStr(geo->_type))
                .geoSrid(geo->_srid)
                .geoDim(geo->_dim)
                .geoMeasure(geo->_measure)
                .geoSpatialIndex(geo->_spatialIndex);
        }

        tField->defaultValue(fieldDefaultValue)
            .constraints(fieldConstraints.str())
            .maxlen(fieldMaxLen);

        modelFields.push_back(*tField);
    }

    std::unique_ptr<Ttable> _table = std::make_unique<Ttable>(modelName, modelFields, primaryKeys);

    TModeler::getInstance().geTdb(modelClass->geTdb()).createTable(std::move(_table));

    return TModeler::getInstance().geTdb(modelClass->geTdb()).tableExists(modelClass->getModelName());
}


std::vector<TtableField> TmsAdapter::tableFields(Tclass* modelClass) {
    auto fields = modelClass->getFields();
    std::vector<TtableField> modelFields;

    TtableField* tField;

    for (auto& [key, field] : fields) {
        BaseField* bField = baseField(field);

        if (!bField) continue;

        std::string fieldName = key;
        TFtype fieldType = bField->getType();
        TFconstraints fieldConstraints = bField->getConstraints();
        TF fieldOnDelete = bField->getOnDelete();
        std::any fieldDefaultValue = bField->getValue();
        int fieldMaxLen = bField->getMaxLen();
        std::string fieldTargetField = bField->getTargetField();

        TFtype adjustedType = (fieldType == TFtype::TEXT && fieldMaxLen > 0)
            ? TFtype::VARCHAR
            : fieldType;

        tField = new TtableField(fieldName, tf_str(adjustedType));

        modelFields.push_back(*tField);
    }

    return modelFields;
}

} // namespace ai::tambo::tmodeler
