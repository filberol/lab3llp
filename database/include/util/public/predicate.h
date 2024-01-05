//
// Created by moyak on 13.09.2023.
//

#ifndef LAB1_CONDITION_PUBLIC_H
#define LAB1_CONDITION_PUBLIC_H

#include <stdbool.h>
#include "../public/types.h"

enum Operator {
    MORE, LESS, EQUALS, INVALID_OPERATOR
};

struct FreeVariable {
    enum DataType dataType;
    void *operand;
};

struct Operand {
    bool isOperandAName;
    union {
        struct FreeVariable *freeVariable;
        char *columnName;
    } operandValue;
};
struct Condition {
    struct Operand left;
    struct Operand right;
    enum Operator op;
};

struct CompareResult {
    int32_t errorCode;
    int32_t compareResult;
};

struct UpdateColumnValue {
    char *name;
    struct Operand assignedValue;
};
struct JoinTablesHeaders {
    struct TableHeader *tableHeader;
    char *tableAlias;
};
struct JoinOperand {
    char *tableAlias;
    char *columnName;
};
struct JoinCondition {
    struct JoinOperand leftOperand;
    struct JoinOperand rightOperand;
    enum Operator operator;
};
struct TableAliasAndColumn {
    char *aliasName;
    char *columnName;
};

struct JoinWhereOperand {
    bool isOperandAName;
    union {
        struct FreeVariable *freeVariable;
        struct TableAliasAndColumn *nameAndColumn;
    } operandValue;
};
struct JoinWhereCondition {
    struct JoinWhereOperand leftOperand;
    struct JoinWhereOperand rightOperand;
    enum Operator operator;
};

#endif //LAB1_CONDITION_PUBLIC_H
