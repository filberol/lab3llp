#ifndef LAB3_FROM_AST_H
#define LAB3_FROM_AST_H

#include <stdio.h>
#include <stdlib.h>
#include <libxml/xmlwriter.h>
#include <libxml/tree.h>

#include <libxml/xmlschemas.h>
#include "../ast/ast_node.h"

int fromAst(struct AstNode *root, xmlDocPtr docPtr, char** buf);

int next(xmlDocPtr docPtr, char **buf);
#endif //LAB3_FROM_AST_H
