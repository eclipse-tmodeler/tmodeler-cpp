#include "ms/TFilterBuilder.h"
#include <sstream>

#include <nlohmann/json.hpp>


TFilterBuilder::TFilterBuilder(): _query(std::make_unique<Json>()){}

/*
struct WhereCondition {
    std::string column;
    std::string op;
    Json value;
};*/

TFilterBuilder& TFilterBuilder::select(const SelectClause& clause) {
    (*_query)["select"] = clause.columns;
    (*_query)["distinct"] = clause.distinct;
    return *this;
}

TFilterBuilder& TFilterBuilder::addSelect(const SelectClause& clause) {
    // S'assurer que "select" est bien un tableau JSON
    if (!(*_query)["select"].is_array()) {
        (*_query)["select"] = Json::array();
    }

    // Ajouter toutes les colonnes à la clause select
    for (const auto& col : clause.columns) {
        (*_query)["select"].push_back(col);
    }

    // Si "distinct" est déjà défini à true, on le garde
    // Sinon, on applique celui du nouveau clause
    if (!(*_query).contains("distinct") || !(*_query)["distinct"].get<bool>()) {
        (*_query)["distinct"] = clause.distinct;
    }

    return *this;
}

TFilterBuilder& TFilterBuilder::addSelect(const std::string& column) {
    // Si "select" n'existe pas encore ou n'est pas un tableau, on initialise
    if (!(*_query)["select"].is_array()) {
        (*_query)["select"] = Json::array();
    }

    // On ajoute la colonne au tableau
    (*_query)["select"].push_back(column);
    return *this;
}

TFilterBuilder& TFilterBuilder::from(const std::string& table) {
    (*_query)["from"] = table;
    return *this;
}

TFilterBuilder& TFilterBuilder::where(const std::vector<WhereCondition>& conditions) {
    for (const auto& cond : conditions) {
        (*_query)["where"].push_back({
            { "column", cond.column },
            { "op", cond.op },
            { "value", cond.value }
            });
    }
    return *this;
}

TFilterBuilder& TFilterBuilder::joins(const std::vector<JoinClause>& joinList) {
    for (const auto& j : joinList) {
        (*_query)["joins"].push_back({
            { "type", j.type },
            { "table", j.table },
            { "on", j.on }
            });
    }
    return *this;
}

TFilterBuilder& TFilterBuilder::groupBy(const std::vector<std::string>& columns) {
    (*_query)["groupBy"] = columns;
    return *this;
}

TFilterBuilder& TFilterBuilder::addGroupBy(const std::string& column) {
    // Si "groupBy" n'existe pas encore ou n'est pas un tableau, on initialise
    if (!(*_query)["groupBy"].is_array()) {
        (*_query)["groupBy"] = Json::array();
    }

    // On ajoute la colonne au tableau
    (*_query)["groupBy"].push_back(column);
    return *this;
}

TFilterBuilder& TFilterBuilder::having(const std::string& condition) {
    (*_query)["having"] = condition;
    return *this;
}

TFilterBuilder& TFilterBuilder::orderBy(const OrderByClause& clause) {
    (*_query)["orderBy"] = clause.columns;
    return *this;
}

TFilterBuilder& TFilterBuilder::limit(int max) {
    (*_query)["limit"] = max;
    return *this;
}

// ---------- BUILD SQL ----------
std::string TFilterBuilder::buildSQL() const {
    std::string sql;
    sql += parseSelect();

    if ((*_query).contains("from"))
        sql += " FROM " + (*_query)["from"].get<std::string>();

    sql += " " + parseJoins();
    sql += " " + parseWhere();
    sql += " " + parseGroupBy();
    sql += " " + parseHaving();
    sql += " " + parseOrderBy();
    sql += " " + parseLimit();

    return sql;
}

// ---------- Helpers ----------
std::string TFilterBuilder::join(const std::vector<std::string>& vec, const std::string& delimiter) const {
    std::stringstream ss;
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << vec[i];
        if (i < vec.size() - 1) ss << delimiter;
    }
    return ss.str();
}

std::string TFilterBuilder::parseSelect() const {
    std::string selectClause = "SELECT ";
    if ((*_query).value("distinct", false)) selectClause += "DISTINCT ";

    if ((*_query).contains("select")) {
        const auto& cols = (*_query)["select"];
        if (cols.size() == 1 && cols[0] == "*") {
            selectClause += "*";
        }
        else {
            std::vector<std::string> colList;
            for (const auto& col : cols) colList.push_back(col.get<std::string>());
            selectClause += join(colList, ", ");
        }
    }
    else {
        selectClause += "*";
    }

    return selectClause;
}

std::string TFilterBuilder::parseJoins() const {
    if (!(*_query).contains("joins")) return "";

    std::vector<std::string> clauses;
    for (const auto& join : (*_query)["joins"]) {
        clauses.push_back(join["type"].get<std::string>() + " JOIN " +
            join["table"].get<std::string>() +
            " ON " + join["on"].get<std::string>());
    }
    return join(clauses, " ");
}

std::string TFilterBuilder::parseWhere() const {
    if (!(*_query).contains("where")) return "";

    std::vector<std::string> conditions;
    for (const auto& cond : (*_query)["where"]) {
        if (cond["op"].get<std::string>().empty())
        {
            std::string val = cond["value"].get<std::string>();
            conditions.push_back(val);
        }
        else
        {
            std::string val = cond["value"].is_string() ? "'" + cond["value"].get<std::string>() + "'" : cond["value"].dump();
            conditions.push_back(cond["column"].get<std::string>() + " " + cond["op"].get<std::string>() + " " + val);
        }
    }
    return "WHERE " + join(conditions, " AND ");
}

std::string TFilterBuilder::parseGroupBy() const {
    if (!(*_query).contains("groupBy")) return "";
    return "GROUP BY " + join((*_query)["groupBy"].get<std::vector<std::string>>(), ", ");
}

std::string TFilterBuilder::parseHaving() const {
    if (!(*_query).contains("having")) return "";
    return "HAVING " + (*_query)["having"].get<std::string>();
}

std::string TFilterBuilder::parseOrderBy() const {
    if (!(*_query).contains("orderBy")) return "";
    return "ORDER BY " + join((*_query)["orderBy"].get<std::vector<std::string>>(), ", ");
}

std::string TFilterBuilder::parseLimit() const {
    if (!(*_query).contains("limit")) return "";
    return "LIMIT " + std::to_string((*_query)["limit"].get<int>());
}
