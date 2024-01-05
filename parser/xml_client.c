#include "net/client_socket_operations.h"
#include "xml/to_xml_client.h"
#include "xml/from_xml_client.h"
#include <stdio.h>

#include "ast/ast.h"

#define YY_STDINIT
#define YYSTYPE double
extern YYSTYPE yylval;

extern int yyparse(void);

extern FILE *yyin;

void yyerror(const char *s) {
    printf("ERROR: %s\n", s);
}


char *typeString[25] = {
        "Root",
        "FOR",
        "FILTER",
        "RETURN",
        "REMOVE",
        "INSERT",
        "UPDATE",
        "Variable",
        "Reference",
        "Collection",
        "Attribute",
        "OR",
        "AND",
        "==",
        "!=",
        "<",
        ">",
        "<=",
        ">=",
        "IN",
        "Object",
        "Object element",
        "Contains",
        "Create table",
        "Drop table"
};

static int max_id = 0;

static char *getAstNodeVal(struct AstNodeValue astNodeValue) {
    switch (astNodeValue.type) {
        case INT_TYPE: {
            int length = snprintf(NULL, 0, "%d", astNodeValue._int);
            char *str = malloc(length + 1);
            snprintf(str, length + 1, "%d", astNodeValue._int);
            return str;
        }
        case BOOL_TYPE: {
            int length = snprintf(NULL, 0, "%d", astNodeValue._int);
            char *str = malloc(length + 1);
            snprintf(str, length + 1, "%d", astNodeValue._int);
            return str;
        }
        case DOUBLE_TYPE: {
            int length = snprintf(NULL, 0, "%f", astNodeValue._double);
            char *str = malloc(length + 1);
            snprintf(str, length + 1, "%f", astNodeValue._double);
            return str;
        }
        default:
            return astNodeValue._string;
    }
}

static char *getValue(struct AstNode *node) {
    switch (node->astNodeType) {
        case NODE_TYPE_VARIABLE:
            return getAstNodeVal(node->astNodeValue);
        case NODE_TYPE_REFERENCE:
            return node->astNodeValue._string;
        case NODE_TYPE_DATA_SOURCE:
            return node->astNodeValue._string;
        case NODE_TYPE_ATTRIBUTE_ACCESS:
            return node->astNodeValue._string;
        case NODE_TYPE_OBJECT_ELEMENT:
            return node->astNodeValue._string;
        default:
            return "NULL";
    }
}

static void printNode(int depth, struct AstNode *node) {
    for (int i = 0; i < depth; i++) {
        printf("%s", "\t");
    }
    printf("Type: %s Value: %s\n", typeString[node->astNodeType], getValue(node));

}

static void printfTree(struct AstNode *node, int depth) {
    printNode(depth, node);
    for (int i = 0; i < node->childrenList.childrenCount; i++) {
        printfTree(node->childrenList.data[i], depth + 1);
    }
}


int main(int argc, char **argv) {
    initAst();
    ++argv;
    --argc;
    if (argc != 2) {
        printf("Wrong arguments");
        return 1;
    }
    char *serverAddress = argv[0];
    int port = atoi(argv[1]);
    printf("Server address: %s\n", serverAddress);
    printf("Port: %d\n", port);

    initAst();
    int socket = initializeClientSocket();
    if (socket < 0) {
        printf("Unable to initialize socket");
        return -socket;
    }
    int err = connectToServer(socket, serverAddress, port);
    if (err < 0) {
        printf("Unable to connect to server");
        return -err;
    }
    yyin = stdin;
    yyparse();
/*
    char *filePath = "/home/egor/CLionProjects/llp/test.file";
    yyin = fopen(filePath, "r");
    if (!yyin) {
        printf("Can't open file %s", filePath);
        return 1;
    }
    yyparse();
*/
    fclose(yyin);
    struct AstNode *root = getAstRoot();
    //printfTree(root, 0);
    char **buf = malloc(sizeof(char *) * 1024);
    char *buffer = malloc(MAX_BUFFER_SIZE);
    xmlDocPtr docPtr = xmlNewDoc(BAD_CAST "1.0");
    fromAst(root, docPtr, buf);
    printf("%s\n", *buf);
    int hasNext = 1;
    while (hasNext && err == 0) {
        err = sendDataToServer(socket, *buf);
        if(err != 0) {
            freeTree();
            return 0;
        }
        err = receiveDataFromServer(socket, buffer);
        if(err != 0) {
            free(buf);
            free(buffer);
            freeTree();
            return 0;
        }
        xmlDocPtr response = xmlParseMemory(buffer, strlen(buffer));
        hasNext = parseResponse(response);
    }
    free(buf);
    free(buffer);
    freeTree();
    return 0;
}
