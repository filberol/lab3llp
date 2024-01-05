//
// Created by moyak on 13.10.2023.
//

#ifndef LAB_2_AST_H
#define LAB_2_AST_H

#include "ast_node.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "variable.h"
#include "variable_stack.h"
#include "common_stack.h"

char* copyQuotedString(const char* src);


char* copyStr(const char* src);

struct AstNode *createNodeFilter(struct AstNode *astNode);

struct AstNode *createNodeReturn(struct AstNode *astNode);

struct AstNode *createNodeRemove(struct AstNode *expression,
                                 struct AstNode *collection);

struct AstNode *createNodeInsert(struct AstNode *expression,
                                 struct AstNode *collection);

struct AstNode *createNodeUpdate(struct AstNode *docExpression,
                                 struct AstNode *collection);

//free variable
struct AstNode *createNodeVariable(char *name);

//column ref
struct AstNode *createNodeReference(char *);

struct AstNode *createNodeValueBool(bool);

struct AstNode *createNodeValueInt(int32_t value);

struct AstNode *createNodeValueDouble(double);

struct AstNode *createNodeValueString(char *);

struct AstNode *createNodeDataSource(char *name);

struct AstNode *createNodeValueNull(void);

struct AstNode *getAstRoot(void);

struct AstNode* getCurrScope(void);


void addVariable(char *name, struct AstNode*);

struct AstNode *getVariable(char *name);

void initAst(void);

struct AstNode* createNodeAttributeAccess(char* name);

struct AstNode* createNodeFor(struct AstNode* variableNode, struct AstNode* expression);

struct AstNode* createNodeBinaryOperator(enum AstNodeType astNodeType, struct AstNode* left, struct AstNode* right);

struct AstNode* createNodeObject();

struct AstNode* createObjectElement(char* name, struct AstNode* expr);

void addOperation(struct AstNode* parent, struct AstNode* child);

void pushCommon(struct AstNode* node);

struct AstNode* popCommon();

void pushObjectElement(char * attributeName, struct AstNode* node);

struct AstNode* createNodeContains(struct AstNode* left, struct AstNode* right);

struct AstNode* createNodeTableField(struct AstNode* nameNode, struct AstNode* typeNode);

struct AstNode* createNodeColumnList(void);


struct AstNode* createNodeCreateTable(struct AstNode* nameNode, struct AstNode* columns);

struct AstNode* createNodeDropTable(struct AstNode* nameNode);


void freeTree(void);

#endif //LAB_2_AST_H
