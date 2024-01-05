#include "to_xml_client.h"

#define WRONG_TYPE 1

#define CHECK_NODE_TYPE_IN(node, ...) \
    do { \
        enum AstNodeType expectedTypes[] = {__VA_ARGS__}; \
        size_t numExpectedTypes = sizeof(expectedTypes);  \
        int matched = 0; \
        for (size_t i = 0; i < numExpectedTypes; ++i) { \
            if ((node)->astNodeType == expectedTypes[i]) { \
                matched = 1; \
                break; \
            } \
        } \
        if (!matched) { \
            return WRONG_TYPE; \
        } \
    } while (0)

#define CHECK_RESULT(expr) do { \
    int result = (expr); \
    if (result != 0) { \
        return result; \
    } \
} while(0)

static int MAX_MESSAGE_SIZE = 1024;

static int variableCount = 0;

static int forCount = 0;


struct VariableToCollection {
    char *variable;
    char *collection;
};

static char* astTypeToStr(struct AstNode *astNode) {
    switch (astNode->astNodeValue.type) {
        case INT_TYPE: {
            return "INT";
        }
        case BOOL_TYPE: {
            return "BOOL";
        }
        case DOUBLE_TYPE: {
            return "DOUBLE";
        }
        case STRING_TYPE: {
            return "STRING";
        }
        default:
            return NULL;
    }

}

static xmlNodePtr getChildByName(xmlNodePtr parent, const char *childName) {
    if (parent == NULL || childName == NULL) {
        return NULL;
    }
    for (xmlNodePtr child = parent->children; child != NULL; child = child->next) {
        if (child->type == XML_ELEMENT_NODE && xmlStrcmp(child->name, (const xmlChar *) childName) == 0) {
            return child;
        }
    }
    return NULL;
}

static char *variableToString(struct AstNode *astNode) {
    switch (astNode->astNodeValue.type) {
        case INT_TYPE: {
            int length = snprintf(NULL, 0, "%d", astNode->astNodeValue._int);
            char *str = malloc(length + 1);
            snprintf(str, length + 1, "%d", astNode->astNodeValue._int);
            return str;
        }
        case BOOL_TYPE: {
            int length = snprintf(NULL, 0, "%d", astNode->astNodeValue._bool);
            char *str = malloc(length + 1);
            snprintf(str, length + 1, "%d", astNode->astNodeValue._bool);
            return str;
        }
        case DOUBLE_TYPE: {
            int length = snprintf(NULL, 0, "%f", astNode->astNodeValue._double);
            char *str = malloc(length + 1);
            snprintf(str, length + 1, "%f", astNode->astNodeValue._double);
            return str;
        }
        case STRING_TYPE: {
            return astNode->astNodeValue._string;
        }
        default:
            return NULL;
    }
}

static void freeStringVariable(struct AstNode *astNode, char *str) {
    if (astNode->astNodeValue.type != STRING_TYPE) {
        free(str);
    }
}


static char *getCollectionByVar(struct VariableToCollection *varToCol, char *var) {
    for (int i = 0; i < variableCount; i++) {
        if (strcmp(varToCol[i].variable, var) == 0) {
            return varToCol[i].collection;
        }
    }
    return NULL;
}

static void addVariableToCollection(struct VariableToCollection *varToCol, char *var, char *col, int index) {
    varToCol[index].variable = var;
    varToCol[index].collection = col;
}

static char *resolveVarsAndFindCollection(struct AstNode *astNode, struct VariableToCollection *varToCol, int index,
                                          enum AstNodeType requestType) {
    char *collectionName = NULL;
    if (astNode->astNodeType == NODE_TYPE_FOR) {
        forCount++;
        addVariableToCollection(varToCol, astNode->childrenList.data[0]->astNodeValue._string,
                                astNode->childrenList.data[1]->astNodeValue._string, index);
        index++;
    }
    if (astNode->astNodeType == requestType) {
        if (forCount > 1) {
            return "JOIN";
        }
        if (requestType == NODE_TYPE_INSERT || requestType == NODE_TYPE_UPDATE) {
            return astNode->childrenList.data[1]->astNodeValue._string;
        }
        if(requestType == NODE_TYPE_RETURN) {
            return varToCol[0].collection; // must be the only one
        }
        return getCollectionByVar(varToCol, astNode->childrenList.data[0]->astNodeValue._string);
    }
    for (int i = 0; i < astNode->childrenList.childrenCount; i++) {
        collectionName = resolveVarsAndFindCollection(astNode->childrenList.data[i], varToCol, index, requestType);
    }
    return collectionName;
}

static char *compByNode(struct AstNode *astNode) {
    if (astNode->astNodeType == NODE_TYPE_OPERATOR_BINARY_EQ) {
        return "==";
    }
    if (astNode->astNodeType == NODE_TYPE_OPERATOR_BINARY_LT) {
        return "&gt;";
    }
    if (astNode->astNodeType == NODE_TYPE_OPERATOR_BINARY_GT) {
        return "&lt;";
    }
    return "";
}

static enum AstNodeType getRequestType(struct AstNode *astNode) {
    enum AstNodeType astNodeType = NODE_TYPE_ROOT;
    if (astNode->astNodeType == NODE_TYPE_VARIABLE) {
        variableCount++;
    }
    if (astNode->astNodeType == NODE_TYPE_INSERT ||
        astNode->astNodeType == NODE_TYPE_UPDATE ||
        astNode->astNodeType == NODE_TYPE_REMOVE ||
        astNode->astNodeType == NODE_TYPE_RETURN ||
        astNode->astNodeType == NODE_TYPE_CREATE_TABLE ||
        astNode->astNodeType == NODE_TYPE_DROP_TABLE) {
        astNodeType = astNode->astNodeType;
    }
    for (int i = 0; i < astNode->childrenList.childrenCount; i++) {
        enum AstNodeType type = getRequestType(astNode->childrenList.data[i]);
        if (type != NODE_TYPE_ROOT) {
            if (astNodeType != NODE_TYPE_ROOT) {
                return NODE_TYPE_ROOT;
            }
            astNodeType = type;
        }
    }
    return astNodeType;
}

static int parseComparison(xmlNodePtr node, struct AstNode *astNode, struct VariableToCollection *varToCol) {
    CHECK_NODE_TYPE_IN(astNode, NODE_TYPE_OPERATOR_BINARY_EQ,
                       NODE_TYPE_OPERATOR_BINARY_GE, NODE_TYPE_OPERATOR_BINARY_GT, NODE_TYPE_OPERATOR_BINARY_LE,
                       NODE_TYPE_OPERATOR_BINARY_LT, NODE_TYPE_OPERATOR_BINARY_NE);
    xmlNodePtr filterNode = xmlNewChild(node, NULL, BAD_CAST "filter", NULL);
    xmlAddChild(node, filterNode);
    CHECK_NODE_TYPE_IN(astNode->childrenList.data[0], NODE_TYPE_REFERENCE);
    CHECK_NODE_TYPE_IN(astNode->childrenList.data[1], NODE_TYPE_VARIABLE);
    char *fieldName = astNode->childrenList.data[0]->childrenList.data[0]->astNodeValue._string;
    xmlNodePtr leftOp = xmlNewTextChild(filterNode, NULL, BAD_CAST "leftOp", NULL);
    xmlAddChild(filterNode, leftOp);
    xmlNodePtr leftIsColName = xmlNewChild(leftOp, NULL, BAD_CAST "isColumnName", BAD_CAST "true");
    xmlNodePtr leftVal = xmlNewTextChild(leftOp, NULL, BAD_CAST "value", BAD_CAST fieldName);
    xmlAddChild(leftOp, leftIsColName);
    xmlAddChild(leftOp, leftVal);


    xmlNewTextChild(filterNode, NULL, BAD_CAST "operator", BAD_CAST compByNode(astNode));

    xmlNodePtr rightOp = xmlNewTextChild(filterNode, NULL, BAD_CAST "rightOp", NULL);
    xmlAddChild(filterNode, rightOp);
    xmlNodePtr rightIsColName = xmlNewChild(rightOp, NULL, BAD_CAST "isColumnName", BAD_CAST "false");
    char *rightValStr = variableToString(astNode->childrenList.data[1]);
    xmlNodePtr type = xmlNewTextChild(rightOp, NULL, BAD_CAST "type", BAD_CAST astTypeToStr(astNode->childrenList.data[1]));
    xmlNodePtr rightVal = xmlNewTextChild(rightOp, NULL, BAD_CAST "value", BAD_CAST rightValStr);
    xmlAddChild(rightOp, rightIsColName);
    xmlAddChild(rightOp, type);
    xmlAddChild(rightOp, rightVal);
    freeStringVariable(astNode, rightValStr);
    return 0;
}


static int parseFilter(xmlNodePtr node, struct AstNode *astNode, struct VariableToCollection *varToCol) {
    CHECK_NODE_TYPE_IN(astNode, NODE_TYPE_FILTER);
    astNode = astNode->childrenList.data[0];
    CHECK_NODE_TYPE_IN(astNode, NODE_TYPE_OPERATOR_BINARY_AND, NODE_TYPE_OPERATOR_BINARY_EQ,
                       NODE_TYPE_OPERATOR_BINARY_GE, NODE_TYPE_OPERATOR_BINARY_GT, NODE_TYPE_OPERATOR_BINARY_LE,
                       NODE_TYPE_OPERATOR_BINARY_LT, NODE_TYPE_OPERATOR_BINARY_NE);
    if (astNode->astNodeType == NODE_TYPE_OPERATOR_BINARY_AND) {
        CHECK_RESULT(parseFilter(node, astNode->childrenList.data[0], varToCol));
        CHECK_RESULT(parseFilter(node, astNode->childrenList.data[1], varToCol));
        return 0;
    } else {
        CHECK_RESULT(parseComparison(node, astNode, varToCol));
    }
    return 0;
}

static int parseFilters(xmlNodePtr node, struct AstNode *forNode, struct VariableToCollection *varToCol, int *index) {
    xmlNodePtr
            filters = xmlNewTextChild(node, NULL, BAD_CAST
            "filters", NULL);
    xmlAddChild(node, filters);
    while (*index < forNode->childrenList.childrenCount) {
        if (forNode->childrenList.data[*index]->astNodeType == NODE_TYPE_FILTER) {
            CHECK_RESULT(parseFilter(filters, forNode->childrenList.data[*index], varToCol));
        }
        (*index)++;
    }
    return 0;
}

static int parseUpdate(xmlNodePtr node, struct AstNode *astNode, struct VariableToCollection *varToCol) {
    char *collName = resolveVarsAndFindCollection(astNode, varToCol, 0, NODE_TYPE_UPDATE);
    xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "requestType", BAD_CAST "UPDATE"));
    xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "tableName", BAD_CAST collName));
    struct AstNode *forNode = astNode->childrenList.data[0];
    int childIndex = 2;
    parseFilters(node, forNode, varToCol, &childIndex);
    childIndex--;
    xmlNodePtr updateValuesNode = xmlNewChild(node, NULL, BAD_CAST "updateValues", NULL);
    CHECK_NODE_TYPE_IN(forNode->childrenList.data[childIndex], NODE_TYPE_UPDATE);

    struct AstNode *update = forNode->childrenList.data[childIndex];
    struct AstNode *object = update->childrenList.data[0];

    for (int index = 0; index < object->childrenList.childrenCount; index++) {
        if (object->childrenList.data[index]->astNodeType == NODE_TYPE_OBJECT_ELEMENT) {
            xmlNodePtr field = xmlNewChild(updateValuesNode, NULL, BAD_CAST "field", NULL);
            struct AstNode *objectElement = object->childrenList.data[index];
            char *fieldName = objectElement->astNodeValue._string;
            char *value;
            if (objectElement->childrenList.data[0]->astNodeType == NODE_TYPE_REFERENCE) {
                xmlNewChild(field, NULL, BAD_CAST "isValueColumnName", BAD_CAST "true");
                value = objectElement->childrenList.data[0]->childrenList.data[0]->astNodeValue._string;
            } else {
                xmlNewChild(field, NULL, BAD_CAST "isValueColumnName", BAD_CAST "false");
                char* type = astTypeToStr(objectElement->childrenList.data[0]);
                xmlNewChild(field, NULL, BAD_CAST "type", BAD_CAST type);
                value = variableToString(objectElement->childrenList.data[0]);
            }
            xmlNewChild(field, NULL, BAD_CAST "name", BAD_CAST fieldName);
            xmlNewChild(field, NULL, BAD_CAST "value", BAD_CAST value);
            if (objectElement->childrenList.data[0]->astNodeType == NODE_TYPE_REFERENCE) {
                freeStringVariable(object->childrenList.data[index], value);
            }
        }
    }
    return 0;
}

static int parseDelete(xmlNodePtr node, struct AstNode *astNode, struct VariableToCollection *varToCol) {
    char *collName = resolveVarsAndFindCollection(astNode, varToCol, 0, NODE_TYPE_REMOVE);
    xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "requestType", BAD_CAST "DELETE"));
    xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "tableName", BAD_CAST collName));
    struct AstNode *forNode = astNode->childrenList.data[0];
    int firstIndex = 2;
    parseFilters(node, forNode, varToCol, &firstIndex);
    return 0;
}


static int parseJoinCondition(xmlNodePtr node, struct AstNode *astNode, struct VariableToCollection *varToCol) {
    if (astNode->astNodeType == NODE_TYPE_OPERATOR_BINARY_AND) {
        parseJoinCondition(node, astNode->childrenList.data[0], varToCol);
        parseJoinCondition(node, astNode->childrenList.data[1], varToCol);
        return 0;
    }
    astNode = astNode->childrenList.data[0];//comparison
    if (astNode->childrenList.data[0]->astNodeType == NODE_TYPE_REFERENCE &&
        astNode->childrenList.data[1]->astNodeType == NODE_TYPE_REFERENCE) {
        char *leftVar = astNode->childrenList.data[0]->astNodeValue._string;
        char *rightVar = astNode->childrenList.data[1]->astNodeValue._string;
        char *leftColumn = astNode->childrenList.data[0]->childrenList.data[0]->astNodeValue._string;
        char *rightColumn = astNode->childrenList.data[1]->childrenList.data[0]->astNodeValue._string;
        char *operator = compByNode(astNode);
        xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "leftAlias", BAD_CAST leftVar));
        xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "rightAlias", BAD_CAST rightVar));
        xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "operator", BAD_CAST operator));
        xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "leftColumn", BAD_CAST leftColumn));
        xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "rightColumn", BAD_CAST rightColumn));
    } else {
        char *leftVar = astNode->childrenList.data[0]->astNodeValue._string;
        char *leftColumn = astNode->childrenList.data[0]->childrenList.data[0]->astNodeValue._string;
        char *operator = compByNode(astNode);
        char *rightValue = variableToString(astNode->childrenList.data[1]);
        xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "tableAlias", BAD_CAST leftVar));
        xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "operator", BAD_CAST operator));
        xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "column", BAD_CAST leftColumn));
        xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "value", BAD_CAST rightValue));
    }
    return 0;
}

static int parseJoin(xmlNodePtr node, struct AstNode *astNode, struct VariableToCollection *varToCol) {
    astNode = astNode->childrenList.data[0];
    xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "requestType", BAD_CAST "JOIN"));
    xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "tableName", BAD_CAST "JOIN"));
    while (astNode->childrenList.data[2]->astNodeType == NODE_TYPE_FOR) {
        astNode = astNode->childrenList.data[2];
    }
    CHECK_NODE_TYPE_IN(astNode->childrenList.data[2], NODE_TYPE_FILTER);
    xmlNodePtr join = xmlNewTextChild(node, NULL, BAD_CAST "join", NULL);
    xmlAddChild(node, join);
    xmlNodePtr aliases = xmlNewTextChild(join, NULL, BAD_CAST "aliases", NULL);
    xmlNodePtr joinConditions = xmlNewTextChild(join, NULL, BAD_CAST "joinConditions", NULL);
    xmlNodePtr filterConditions = xmlNewTextChild(join, NULL, BAD_CAST "filterConditions", NULL);
    xmlNodePtr returnStatement = xmlNewTextChild(join, NULL, BAD_CAST "return", NULL);

    xmlAddChild(join, joinConditions);
    xmlAddChild(join, filterConditions);
    xmlAddChild(join, returnStatement);
    xmlAddChild(join, aliases);

    int index = 2;
    for (int i = 0; i < variableCount; i++) {
        xmlNodePtr alias = xmlNewTextChild(aliases, NULL, BAD_CAST "alias", BAD_CAST NULL);
        xmlNodePtr table = xmlNewTextChild(alias, NULL, BAD_CAST "table", BAD_CAST varToCol[i].collection);
        xmlNodePtr al = xmlNewTextChild(alias, NULL, BAD_CAST "al", BAD_CAST varToCol[i].variable);
        xmlAddChild(aliases, alias);
        xmlAddChild(alias, table);
        xmlAddChild(alias, al);

    }

    while (index < astNode->childrenList.childrenCount &&
           astNode->childrenList.data[index]->astNodeType == NODE_TYPE_FILTER) {
        parseJoinCondition(joinConditions, astNode->childrenList.data[index], varToCol);
        index++;
    }
    CHECK_NODE_TYPE_IN(astNode->childrenList.data[index], NODE_TYPE_RETURN);
    struct AstNode *returnNode = astNode->childrenList.data[index];
    struct AstNode *obj = returnNode->childrenList.data[0];
    for (int i = 0; i < obj->childrenList.childrenCount; i++) {
        //supposed to be reference
        xmlNodePtr returnField = xmlNewTextChild(returnStatement, NULL, BAD_CAST "returnField", NULL);
        xmlAddChild(returnStatement, returnField);
        struct AstNode *objectElement = obj->childrenList.data[i]->childrenList.data[0]; //we ignore field name
        xmlNodePtr tableAlias = xmlNewTextChild(returnField, NULL, BAD_CAST "tableAlias",
                                                BAD_CAST objectElement->astNodeValue._string);
        xmlNodePtr field = xmlNewTextChild(returnField, NULL, BAD_CAST "field",
                                           BAD_CAST objectElement->childrenList.data[0]->astNodeValue._string);
        xmlAddChild(returnField, tableAlias);
        xmlAddChild(returnField, field);

    }

    return 0;
}

static int parseReturn(xmlNodePtr node, struct AstNode *astNode, struct VariableToCollection *varToCol) {

    char *collName = resolveVarsAndFindCollection(astNode, varToCol, 0, NODE_TYPE_RETURN);
    if (forCount > 1) {
        return parseJoin(node, astNode, varToCol);
    }
    xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "requestType", BAD_CAST "SELECT"));
    xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "tableName", BAD_CAST collName));
    struct AstNode *forNode = astNode->childrenList.data[0];
    int firstIndex = 2;
    parseFilters(node, forNode, varToCol, &firstIndex);
    return 0;
}

static int parseInsert(xmlNodePtr node, struct AstNode *astNode, struct VariableToCollection *varToCol) {
    char *collName = resolveVarsAndFindCollection(astNode, varToCol, 0, NODE_TYPE_INSERT);
    xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "requestType", BAD_CAST "INSERT"));
    xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "tableName", BAD_CAST collName));
    xmlNodePtr insertValuesNode = xmlNewChild(node, NULL, BAD_CAST "insertValues", NULL);
    struct AstNode *insert = astNode->childrenList.data[0];
    struct AstNode *object = insert->childrenList.data[0];

    for (int index = 0; index < object->childrenList.childrenCount; index++) {
        if (object->childrenList.data[index]->astNodeType == NODE_TYPE_OBJECT_ELEMENT) {
            xmlNodePtr field = xmlNewChild(insertValuesNode, NULL, BAD_CAST "field", NULL);
            struct AstNode *objectElement = object->childrenList.data[index];
            char *fieldName = objectElement->astNodeValue._string;
            char *value;
            char* type = astTypeToStr(objectElement->childrenList.data[0]);
            if (objectElement->childrenList.data[0]->astNodeType == NODE_TYPE_REFERENCE) {
                value = objectElement->childrenList.data[0]->childrenList.data[0]->astNodeValue._string;
            } else {
                value = variableToString(objectElement->childrenList.data[0]);
            }
            xmlNewChild(field, NULL, BAD_CAST "name", BAD_CAST fieldName);
            xmlNewChild(field, NULL, BAD_CAST "value", BAD_CAST value);
            xmlNewChild(field, NULL, BAD_CAST "type", BAD_CAST type);

            if (objectElement->childrenList.data[0]->astNodeType == NODE_TYPE_REFERENCE) {
                freeStringVariable(object->childrenList.data[index], value);
            }
        }
    }
    return 0;
}

static int parseCreateTable(xmlNodePtr node, struct AstNode *astNode, struct VariableToCollection *varToCol) {
    xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "requestType", BAD_CAST "CREATE_TABLE"));
    struct AstNode* createTableNode = astNode->childrenList.data[0];
    char* collectionName = createTableNode->childrenList.data[0]->astNodeValue._string;
    xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "tableName", BAD_CAST collectionName));
    xmlNodePtr fields = xmlNewTextChild(node, NULL, BAD_CAST "fields", NULL);
    xmlAddChild(node, fields);
    struct AstNode* object = createTableNode->childrenList.data[1];
    for(int i = 0; i < object->childrenList.childrenCount; i++) {
        struct AstNode* objectEl = object->childrenList.data[i];
        xmlNodePtr field = xmlNewChild(fields, NULL, BAD_CAST "field", NULL);
        xmlAddChild(fields, field);
        xmlNodePtr name = xmlNewChild(field, NULL, BAD_CAST "name", BAD_CAST objectEl->childrenList.data[0]->astNodeValue._string);
        xmlNodePtr type = xmlNewChild(field, NULL, BAD_CAST "type", BAD_CAST objectEl->childrenList.data[1]->astNodeValue._string);
        xmlAddChild(field, name);
        xmlAddChild(field, type);
    }
    return 0;
}

static int parseDeleteTable(xmlNodePtr node, struct AstNode *astNode, struct VariableToCollection *varToCol) {
    xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "requestType", BAD_CAST "DROP_TABLE"));    struct AstNode* createTableNode = astNode->childrenList.data[0];
    struct AstNode* dropTableNode = astNode->childrenList.data[0];
    char* collectionName = dropTableNode->childrenList.data[0]->astNodeValue._string;
    xmlAddChild(node, xmlNewTextChild(node, NULL, BAD_CAST "tableName", BAD_CAST collectionName));
    return 0;
}

int fromAst(struct AstNode *root, xmlDocPtr docPtr, char **buf) {
    variableCount = 0;
    forCount = 0;
    enum AstNodeType requestType = getRequestType(root);

    struct VariableToCollection variableToCollection[variableCount];
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "sqlQuery");
    xmlDocSetRootElement(docPtr, xmlNode);

    if (requestType == NODE_TYPE_UPDATE) {
        parseUpdate(xmlNode, root, variableToCollection);
    }
    if (requestType == NODE_TYPE_REMOVE) {
        parseDelete(xmlNode, root, variableToCollection);
    }
    if (requestType == NODE_TYPE_RETURN) {
        parseReturn(xmlNode, root, variableToCollection);
    }
    if (requestType == NODE_TYPE_INSERT) {
        parseInsert(xmlNode, root, variableToCollection);
    }
    if (requestType == NODE_TYPE_CREATE_TABLE) {
        parseCreateTable(xmlNode, root, variableToCollection);
    }
    if (requestType == NODE_TYPE_DROP_TABLE) {
        parseDeleteTable(xmlNode, root, variableToCollection);
    }

    xmlDocDumpMemory(docPtr, (xmlChar **) buf, &MAX_MESSAGE_SIZE);
    return 0;
}
