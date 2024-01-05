#ifndef LAB3_FROM_XML_SERVER_H
#define LAB3_FROM_XML_SERVER_H

#include "string.h"
#include <libxml/xmlwriter.h>
#include <libxml/tree.h>

#include <libxml/xmlschemas.h>

#include "../public/database.h"

#define PARSE_ERROR 1


enum requestType {
    CREATE_TABLE,
    DROP_TABLE,
    INSERT,
    SELECT,
    DELETE,
    UPDATE
};

struct createTableRequest {
    char *tableName;
    int32_t columnNum;
    enum DataType *types;
    const char **names;
};

struct dropTableRequest {
    char *tableName;
};

struct insertRequest {
    char *tableName;
    int32_t dataCount;
    void **data;
};

struct selectRequest {
    char *tableName;
    int32_t conditionCount;
    struct Condition *conditions;
};


struct deleteRequest {
    char *tableName;
    int32_t conditionCount;
    struct Condition *conditions;
};

struct updateRequest {
    char *tableName;
    int32_t updateColumnsCount;
    struct UpdateColumnValue *updateColumnValues;
    int32_t conditionCount;
    struct Condition *conditions;
};


struct request {
    enum requestType type;
    union {
        struct createTableRequest createTableRequest;
        struct dropTableRequest dropTableRequest;
        struct insertRequest insertRequest;
        struct selectRequest selectRequest;
        struct deleteRequest deleteRequest;
        struct updateRequest updateRequest;
    };
};

void freeRequest(struct request *request);
int parseXml(xmlDocPtr xmlDocument, struct request *request);

#endif //LAB3_FROM_XML_SERVER_H
