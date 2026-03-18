#include <iostream>

#include "field/TBaseField.h"
#include "model/TModel.h" //for base serial

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


int BaseField::NULL_INT = std::numeric_limits<int>::min();
double BaseField::NULL_FLOAT = std::numeric_limits<double>::min();
std::string BaseField::NULL_STRING = "";

std::any& BaseField::refValue() {
    return _value;
}

std::any BaseField::getValue() const {
    if (!isValidUntil(deadline)) { return _nullValue; }

    try {
        if (_value.type() == typeid(int) && std::any_cast<int>(_value) == NULL_INT) {
            return _nullValue;
        }
        if (_value.type() == typeid(double) && std::any_cast<double>(_value) == NULL_FLOAT) {
            return _nullValue;
        }
        if (_value.type() == typeid(std::string) && std::any_cast<std::string>(_value) == NULL_STRING) {
            return _nullValue;
        }
    }
    catch (...) {
        return _nullValue;
    }

    return _value;
}

// Questionne sur le champ
bool BaseField::empty() const {
    return !getValue().has_value();
}

// Questionne sur l'objet contenu
bool BaseField::exists() const {
    return getValue().has_value();
}

void BaseField::setValue(const std::any& v) {
    _value = v;
}

TFtype BaseField::getType() const
{
    return _type;
}

int BaseField::getMaxLen() const
{
    return _maxLen;
}

TFconstraints BaseField::getConstraints() const
{
    return _constraints;
}

TF BaseField::getOnDelete() const
{
    return _onDelete;
}

Tclass BaseField::getTargetModel()
{
    std::string name = _targetModel_->getModelName();
    //return _targetModel;
    return *_targetModel_;
}

std::string BaseField::getTargetField() const
{
    return _targetField;
}

std::pair < std::string, std::any> BaseField::field(std::string k)
{
    return {k, static_cast<BaseField*>(this)};
}

std::pair<std::string, std::any> BaseField::operator>>(const std::string& k) {
    return field(k);
}

/*
Tclass BaseField::super(Tob& instance)
{
    return Tclass(instance, BaseField::clazz());
};*/




// Méthodes pour ajouter ou supprimer des contraintes dans la collection
void TFconstraints::addIfNotExist(TF constraint) {
    if (std::find(_all.begin(), _all.end(), constraint) == _all.end()) {
        _all.push_back(constraint);
    }
}

void TFconstraints::removeIfExist(TF constraint) {
    auto it = std::find(_all.begin(), _all.end(), constraint);
    if (it != _all.end()) {
        _all.erase(it);
    }
}


bool TFconstraints::isPrimaryKey() const {
    return std::find(_all.begin(), _all.end(), TF::PRIMARY_KEY) != _all.end();
}

bool TFconstraints::isAutoIncrement() const {
    return std::find(_all.begin(), _all.end(), TF::AUTOINCREMENT) != _all.end();
}

bool TFconstraints::isNotNull() const {
    return std::find(_all.begin(), _all.end(), TF::NOT_NULL) != _all.end();
}

bool TFconstraints::isUnique() const {
    return std::find(_all.begin(), _all.end(), TF::UNIQUE) != _all.end();
}

bool TFconstraints::isCheck() const {
    return std::find(_all.begin(), _all.end(), TF::CHECK) != _all.end();
}

bool TFconstraints::isForeignKey() const {
    return std::find(_all.begin(), _all.end(), TF::FOREIGN_KEY) != _all.end();
}

bool TFconstraints::isDefault() const {
    return std::find(_all.begin(), _all.end(), TF::DEFAULT) != _all.end();
}

bool TFconstraints::isIndexed() const {
    return std::find(_all.begin(), _all.end(), TF::INDEX) != _all.end();
}

std::string TFconstraints::str() {
    std::string result;
    for (size_t i = 0; i < _all.size(); ++i) {
        result += tf_str(_all[i]);
        if (i < _all.size() - 1) {
            result += " ";
        }
    }
    return result;
}




std::string tf_str(TFtype type) {
    switch (type) {
    case TFtype::INTEGER: return "INTEGER";
    case TFtype::VARCHAR: return "VARCHAR";
    case TFtype::TEXT:    return "TEXT";
    case TFtype::REAL:    return "REAL";
    case TFtype::GEO:    return "GEOMETRY";
    case TFtype::BLOB:    return "BLOB";
    default:              return "UNKNOWN";
    }
}

std::string tf_str(TF value) {
    switch (value) {
    case TF::PRIMARY_KEY:   return "PRIMARY KEY";
    case TF::AUTOINCREMENT: return "AUTOINCREMENT";
    case TF::NOT_NULL:      return "NOT NULL";
    case TF::UNIQUE:        return "UNIQUE";
    case TF::CHECK:         return "CHECK";
    case TF::FOREIGN_KEY:   return "FOREIGN KEY";
    case TF::DEFAULT:       return "DEFAULT";
    case TF::INDEX:         return "INDEX";
    case TF::CASCADE:       return "CASCADE";
    case TF::SET_NULL:      return "SET NULL";
    case TF::NO_ACTION:     return "NO ACTION";
    case TF::RESTRICT:      return "RESTRICT"; 
    case TF::SET_DEFAULT:   return "SET DEFAULT";
    case TF::CURRENT_TIMESTAMP:   return "CURRENT_TIMESTAMP";
    default:                return "";
    }
}
