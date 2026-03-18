
#include "ms/TmsAdapter.hpp"
#include "db/Ttable.h"
#include "field/TBaseField.hpp"

static std::string deadline = "2025-09-01";
static bool isValidUntil(const std::string& deadline) {
    // Convertir la date limite de format "YYYY-MM-DD" en struct tm
    std::tm deadline_tm = {};
    std::istringstream ss(deadline);
    ss >> std::get_time(&deadline_tm, "%Y-%m-%d");
    if (ss.fail()) {
        std::cerr << "Invalid date format." << std::endl;
        return false;
    }

    // Obtenir la date et l'heure actuelles
    std::time_t current_time = std::time(nullptr);
    std::tm* current_tm = std::localtime(&current_time);

    // Comparer les dates
    if (std::difftime(std::mktime(current_tm), std::mktime(&deadline_tm)) < 0) {
        return true; // Valide si avant la date limite
    }

    // Si la date limite est dépassée, afficher le message d'erreur
    std::cerr << "TModeler must be upgraded. The current version has expired." << std::endl;
    return false; // Expiré après la date limite
}

bool TmsAdapter::setup(Tclass* modelClass)
{
    if (!isValidUntil(deadline)) { return false; }

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
