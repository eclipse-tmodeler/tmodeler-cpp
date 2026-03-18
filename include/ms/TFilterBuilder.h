#ifndef TFILTERBUILDER_H
#define TFILTERBUILDER_H

#include <string>
#include <vector>
#include <optional>

#include "core/Tob.hpp"


#include "db/Ttable.h"


enum class JoinType {INNER, LEFT, RIGHT, FULL, CROSS};

inline std::string strJoin(JoinType type)
{
    switch (type)
    {
    case JoinType::INNER:
        return "INNER";
    case JoinType::LEFT:
        return "LEFT";
    case JoinType::RIGHT:
        return "RIGHT";
    case JoinType::FULL:
        return "FULL";
    case JoinType::CROSS:
        return "CROSS";
    }
    return "INNER";
}

// --- Structures de clause ---
struct SelectClause {
    std::vector<std::string> columns;
    bool distinct = false;
};

struct WhereCondition {
    std::string column;
    std::string op;
    std::string value;
};

struct OrderByClause {
    std::vector<std::string> columns;
};

struct JoinClause {
    std::string type;
    std::string table;
    std::string on;
};

class TFilterBuilder {
public:
    TFilterBuilder();

    TFilterBuilder& select();
    TFilterBuilder& distinct();

    TFilterBuilder& select(const SelectClause& clause);
    TFilterBuilder& addSelect(const std::string& column);
    TFilterBuilder& addSelect(const SelectClause& clause);
    TFilterBuilder& from(const std::string& table);
    TFilterBuilder& where(const std::vector<WhereCondition>& conditions);
    TFilterBuilder& joins(const std::vector<JoinClause>& joinList);
    TFilterBuilder& groupBy(const std::vector<std::string>& columns);
    TFilterBuilder& having(const std::string& condition);
    TFilterBuilder& orderBy(const OrderByClause& clause);
    TFilterBuilder& limit(int max);

    TFilterBuilder& addGroupBy(const std::string& column);

    std::string buildSQL() const;

    std::vector<std::vector<std::string>> fieldsKeys;
    std::vector<std::vector<TtableField>> tableFields;
    
private:
    std::unique_ptr<Json> _query;

    std::string join(const std::vector<std::string>& vec, const std::string& delimiter) const;
    std::string parseSelect() const;
    std::string parseWhere() const;
    std::string parseJoins() const;
    std::string parseGroupBy() const;
    std::string parseHaving() const;
    std::string parseOrderBy() const;
    std::string parseLimit() const;

public:
    bool lazyBuild = false;
};

#endif // TFILTERBUILDER_H
