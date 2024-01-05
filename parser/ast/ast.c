//
// Created by moyak on 21.10.2023.
//
#include "ast.h"

static struct AstNode *root = NULL;

static struct AstNode *curScope = NULL;

static struct VariableStack *variableStack = NULL;

static int32_t currentDepth = 1;

static struct Stack *commonStack = NULL;

char *copyStr(const char *src) {
    char *dest = malloc(strlen(src));
    strcpy(dest, src);
    return dest;
}

char *copyQuotedString(const char *src) {
    char *dest = malloc(strlen(src) - 2);
    memcpy(dest, src + 1, strlen(src) - 2);
    return dest;
}


void initAst(void) {
    variableStack = initVariableStack(16);
    commonStack = initStack();
}


static struct AstNode *createNode(enum AstNodeType astNodeType) {
    struct AstNode *node = (struct AstNode *) malloc(sizeof(struct AstNode));
    node->astNodeType = astNodeType;
    node->childrenList = INIT_CHILDREN_LIST;
    return node;
}

struct AstNode *createNodeFilter(struct AstNode *expression) {
    struct AstNode *filterNode = createNode(NODE_TYPE_FILTER);
    addOperation(filterNode, expression);
    return filterNode;
}

struct AstNode *createNodeReturn(struct AstNode *expression) {
    struct AstNode *node = createNode(NODE_TYPE_RETURN);
    addOperation(node, expression);
    return node;
}

struct AstNode *createNodeRemove(struct AstNode *expression,
                                 struct AstNode *collection) {
    struct AstNode *removeNode = createNode(NODE_TYPE_REMOVE);
    addOperation(removeNode, expression);
    addOperation(removeNode, collection);
    return removeNode;
}

struct AstNode *createNodeInsert(struct AstNode *expression,
                                 struct AstNode *collection) {
    struct AstNode *insertNode = createNode(NODE_TYPE_INSERT);
    addOperation(insertNode, expression);
    addOperation(insertNode, collection);
    return insertNode;
}

struct AstNode *createNodeUpdate(struct AstNode *docExpression,
                                 struct AstNode *collection) {
    struct AstNode *updateNode = createNode(NODE_TYPE_UPDATE);
    addOperation(updateNode, docExpression);
    addOperation(updateNode, collection);
    return updateNode;

}

//free variable
struct AstNode *createNodeVariable(char *name) {
    struct AstNode *stringNode = createNode(NODE_TYPE_VARIABLE);
    stringNode->astNodeValue.type = STRING_TYPE;
    stringNode->astNodeValue._string = copyStr(name);
    return stringNode;
}

//column ref
struct AstNode *createNodeReference(char *columnName) {
    struct AstNode *stringNode = createNode(NODE_TYPE_REFERENCE);
    stringNode->astNodeValue.type = COLUMN_REF_TYPE;
    stringNode->astNodeValue._string = copyStr(columnName);
    return stringNode;

}

struct AstNode *createNodeValueBool(bool val) {
    struct AstNode *boolNode = createNode(NODE_TYPE_VARIABLE);
    boolNode->astNodeValue.type = BOOL_TYPE;
    boolNode->astNodeValue._bool = val;
    return boolNode;
}

struct AstNode *createNodeValueInt(int32_t val) {
    struct AstNode *intNode = createNode(NODE_TYPE_VARIABLE);
    intNode->astNodeValue.type = INT_TYPE;
    intNode->astNodeValue._int = val;
    return intNode;
}

struct AstNode *createNodeValueDouble(double val) {
    struct AstNode *doubleNode = createNode(NODE_TYPE_VARIABLE);
    doubleNode->astNodeValue.type = DOUBLE_TYPE;
    doubleNode->astNodeValue._double = val;
    return doubleNode;
}

struct AstNode *createNodeValueString(char *val) {
    struct AstNode *stringNode = createNode(NODE_TYPE_VARIABLE);
    stringNode->astNodeValue.type = STRING_TYPE;
    stringNode->astNodeValue._string = copyStr(val);
    return stringNode;
}

struct AstNode *createNodeDataSource(char *name) {
    struct AstNode *datasourceNode = createNode(NODE_TYPE_DATA_SOURCE);
    datasourceNode->astNodeValue.type = DATASOURCE_REF_TYPE;
    datasourceNode->astNodeValue._string = copyStr(name);
    return datasourceNode;
}

struct AstNode *createNodeValueNull(void) {
    struct AstNode *null = createNode(NODE_TYPE_VARIABLE);
    null->astNodeValue.type = NULL_TYPE;
    return null;
}

static struct AstNode *createRoot(void) {
    struct AstNode *astRoot = createNode(NODE_TYPE_ROOT);
    astRoot->astNodeValue = INVALID_AST_NODE_VALUE;
    return astRoot;

}

struct AstNode *getAstRoot(void) {
    if (root == NULL) {
        root = createRoot();
    }
    return root;
}

struct AstNode *getCurrScope(void) {
    if (curScope == NULL) {
        return getAstRoot();
    } else {
        return curScope;
    }
}


struct AstNode *createNodeAttributeAccess(char *name) {
    struct AstNode *attribute = createNode(NODE_TYPE_ATTRIBUTE_ACCESS);
    attribute->astNodeValue.type = ATTRIBUTE_TYPE;
    attribute->astNodeValue._string = copyStr(name);
    return attribute;

}


struct AstNode *createNodeBinaryOperator(enum AstNodeType astNodeType, struct AstNode *left, struct AstNode *right) {
    struct AstNode *astNode = createNode(astNodeType);
    addOperation(astNode, left);
    addOperation(astNode, right);
    return astNode;
}

struct AstNode *createNodeObject() {
    struct AstNode *astNode = createNode(NODE_TYPE_OBJECT);
    return astNode;
}

struct AstNode *createObjectElement(char *name, struct AstNode *expr) {
    struct AstNode *astNode = createNode(NODE_TYPE_OBJECT_ELEMENT);
    astNode->astNodeValue.type = ATTRIBUTE_TYPE;
    astNode->astNodeValue._string = name;
    addOperation(astNode, expr);
    return astNode;
}

void addOperation(struct AstNode *parent, struct AstNode *child) {
    addAstOperation(parent, child);
    if (child->astNodeType == NODE_TYPE_FOR) {
        curScope = child;
    }
}


void pushCommon(struct AstNode *node) {
    push(commonStack, node);
}

struct AstNode *popCommon() {
    return (struct AstNode *) pop(commonStack);
}


void pushObjectElement(char *attributeName, struct AstNode *node) {
    struct AstNode *currentObject = (struct AstNode *) (peek(commonStack));
    struct AstNode *objectAttrubute = createObjectElement(attributeName, node);
    addOperation(currentObject, objectAttrubute);
}

struct AstNode *createNodeContains(struct AstNode *left, struct AstNode *right) {
    struct AstNode *containsNode = createNode(NODE_TYPE_CONTAINS);
    addOperation(containsNode, left);
    addOperation(containsNode, right);

    return containsNode;
}

void addVariable(char *name, struct AstNode *value) {
    struct VariableNode variableNode = {
            .type = INVALID,
            .variableName = copyStr(name),
            .depth = currentDepth,
            .val = value
    };
    addData(variableStack, variableNode);
}

struct AstNode *getVariable(char *name) {
    struct VariableNode *vn = getStackVariable(variableStack, name);
    struct AstNode *var = vn == NULL ? NULL : createNodeReference(vn->variableName);
    return var;
}

struct AstNode *createNodeFor(struct AstNode *variableNode, struct AstNode *expression) {
    struct AstNode *astNode = createNode(NODE_TYPE_FOR);
    addOperation(astNode, variableNode);
    addOperation(astNode, expression);
    // curScope = astNode;
    return astNode;
}

static void freeNode(struct AstNode *node) {
    for (size_t child = 0; child < node->childrenList.childrenCount; child++) {
        freeNode(node->childrenList.data[child]);
    }
    freeNodeNonRecursively(node);
}

struct AstNode *createNodeCreateTable(struct AstNode *nameNode, struct AstNode *columns) {
    struct AstNode *createTableNode = createNode(NODE_TYPE_CREATE_TABLE);
    addOperation(createTableNode, nameNode);
    addOperation(createTableNode, columns);
    return createTableNode;
}

struct AstNode *createNodeDropTable(struct AstNode *nameNode) {
    struct AstNode *dropTableNode = createNode(NODE_TYPE_DROP_TABLE);
    addOperation(dropTableNode, nameNode);
    return dropTableNode;
}

static int checkIfTypeIsCorrect(struct AstNode *typeNode) {
    if (typeNode->astNodeType != NODE_TYPE_VARIABLE || typeNode->astNodeValue.type != STRING_TYPE) {
        return 1;
    }
    if (strcmp(typeNode->astNodeValue._string, "INT") != 0 &&
        strcmp(typeNode->astNodeValue._string, "DOUBLE") != 0 &&
        strcmp(typeNode->astNodeValue._string, "STRING") != 0 &&
        strcmp(typeNode->astNodeValue._string, "BOOL") != 0) {
        return 1;
    }
    return 0;
}

struct AstNode *createNodeTableField(struct AstNode *nameNode, struct AstNode *typeNode) {
    struct AstNode *tableFieldNode = createNode(NODE_TYPE_OBJECT_ELEMENT);
    addOperation(tableFieldNode, nameNode);
    if (checkIfTypeIsCorrect(typeNode) != 0) {
        return NULL;
    }
    addOperation(tableFieldNode, typeNode);
    return tableFieldNode;
}

struct AstNode* createNodeColumnList(void) {
    struct AstNode* columnListNode = createNode(NODE_TYPE_OBJECT);
    return columnListNode;
}

void freeTree(void) {
    struct AstNode *root = getAstRoot();
    freeNode(root);
    freeStack(variableStack);
}

