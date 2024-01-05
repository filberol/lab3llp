%define parse.error verbose

%{
    #include <stdio.h>
    #include <math.h>
    #include <stdbool.h>
    #include "ast.h"
    extern void yyerror (char const *);
    extern int yylex(void);

    #ifdef YYDEBUG
      yydebug = 1;
    #endif
%}
%union {
    struct AstNode* node;
    double d_val;
    int i_val;
    char* s_val;
    bool b_val;
}
%token T_CREATE_TABLE "CREATE_TABLE"
%token T_DROP_TABLE "DROP_TABLE"

%token T_FOR "FOR"
%token T_FILTER "FILTER"
%token T_RETURN "RETURN"

%token T_REMOVE "REMOVE"
%token T_INSERT "INSERT"
%token T_UPDATE "UPDATE"
%token T_IN "IN"
%token T_QUOTED_STRING "q-string"

%token T_NULL "null"
%token T_TRUE "true"
%token T_FALSE "false"
%token T_STRING "string"
%token T_INTEGER "int"
%token T_DOUBLE "double"

%token T_EQ "=="
%token T_NE "!="
%token T_LT "<"
%token T_GT ">"
%token T_LE "<="
%token T_GE ">="

%token T_COMMA ","
%token T_OPEN "("
%token T_CLOSE ")"
%token T_OBJECT_OPEN "{"
%token T_OBJECT_CLOSE "}"
%token T_COLON ":"
%token T_END 0 "end"
%token T_NOT "not operator"
%token T_AND "and operator"
%token T_OR "or operator"
%token T_CONTAINS "contains function"


/* define operator precedence */

%left T_OR
%left T_AND
%left T_COMMA
%left T_EQ T_NE
%left T_LT T_GT T_LE T_GE
%right T_COLON
/* define token return types */



%type <s_val> T_QUOTED_STRING
%type <s_val> T_STRING
%type <node> T_INTEGER
%type <node> T_DOUBLE
%type <node> expression;
%type <node> reference;
%type <node> value_literal;
%type <s_val> variable_name;
%type <node> numeric_value;
%type <node> for_output_variable;
%type <node> in_collection;
%type <node> in_collection_name;
%type <node> operator_binary;
%type <node> object;
%type <node> optional_object_elements;
%type <node> object_elements_list;
%type <node> object_element;
%type <s_val> object_element_name;
%type <node> column;
%type <node> column_list;
%type <node> contains;
%start queryStart

%%



queryStart:
    create_table_statement {
    }
  | drop_table_statement {
    }
  | query {

    }
  ;

create_table_statement:
    T_CREATE_TABLE T_STRING T_OBJECT_OPEN column_list T_OBJECT_CLOSE {
        struct AstNode* tableNameNode = createNodeDataSource($2);
        struct AstNode* tableFieldsNode = (struct AstNode*)($4);
        struct AstNode* createTableNode = createNodeCreateTable(tableNameNode, tableFieldsNode);
        addOperation(getCurrScope(), createTableNode);
    }
  ;

drop_table_statement:
    T_DROP_TABLE T_STRING {
        struct AstNode* tableNameNode = createNodeValueString($2);
        struct AstNode* dropTableNode = createNodeDropTable(tableNameNode);
        addOperation(getCurrScope(), dropTableNode);
    }
  ;

column_list:
    column {
        struct AstNode* col = (struct AstNode*)($1);
        struct AstNode* columnListNode = createNodeColumnList();
        addOperation(columnListNode, col);
        $$ = columnListNode;

    }
  | column_list T_COMMA column {
        struct AstNode* columnListNode = (struct AstNode*)($1);
        struct AstNode* columnNode = (struct AstNode*)($3);
        addOperation(columnListNode, columnNode);
        $$ = columnListNode;
    }
  ;

column:
    T_STRING T_COLON T_STRING {
        // attribute-name : attribute-type
        struct AstNode* fieldNameNode = createNodeValueString($1);
        struct AstNode* fieldTypeNode = createNodeValueString($3);
        struct AstNode* tableFieldNode = createNodeTableField(fieldNameNode, fieldTypeNode);
        $$ = tableFieldNode;
    }
  ;


query:
    optional_statement_block_statements final_statement {
    }
  ;




final_statement:
    return_statement {
    }
  | remove_statement {
    }
  | insert_statement {
    }
  | update_statement {
    }
  ;

optional_statement_block_statements:
    /* empty */ {
    }
  | optional_statement_block_statements statement_block_statement {
    }
  ;

statement_block_statement:
    for_statement {
    }
  | filter_statement {
    }
  |  remove_statement {
    }
  | insert_statement {
    }
  | update_statement {
    }
  ;

for_output_variable:
    variable_name {
          struct AstNode* node = createNodeValueString($1);
          $$ = node;
        }
    ;

for_statement:
    T_FOR for_output_variable T_IN expression {
        struct AstNode* variableNameNode = (struct AstNode*)($2);
        struct AstNode* variableNode = createNodeVariable(variableNameNode->astNodeValue._string);
        addVariable(variableNameNode->astNodeValue._string, variableNode);
        addOperation(getCurrScope(), createNodeFor(variableNode, $4));
    }
  ;

filter_statement:
    T_FILTER expression {
      struct AstNode* filterNode = createNodeFilter($2);
      addOperation(getCurrScope(), filterNode);

    }
  ;

return_statement:
    T_RETURN expression {
      struct AstNode* node = createNodeReturn($2);
      addOperation(getCurrScope(), node);
    }
  ;

in_collection:
    T_IN in_collection_name {
      $$ = $2;
    }
   ;

remove_statement:
    T_REMOVE expression in_collection  {
      struct AstNode* node = createNodeRemove($2, $3);
      addOperation(getCurrScope(), node);
    }
  ;

insert_statement:
    T_INSERT expression in_collection {
      struct AstNode* node = createNodeInsert($2, $3);
      addOperation(getCurrScope(), node);
    }
  ;

update_parameters:
    expression in_collection {
      struct AstNode* node = createNodeUpdate($1, $2);
      addOperation(getCurrScope(), node);
    }
  ;

update_statement:
    T_UPDATE update_parameters {
    }
  ;


object:
    T_OBJECT_OPEN {
      struct AstNode* node = createNodeObject();
      pushCommon(node);
    } optional_object_elements T_OBJECT_CLOSE {
      $$ = (struct AstNode*)(popCommon());
    }
  ;

optional_object_elements:
    /* empty */ {
    }
  | object_elements_list {
    }
  | object_elements_list T_COMMA {
    }
  ;

object_elements_list:
    object_element {
    }
  | object_elements_list T_COMMA object_element {
    }
  ;

object_element:
    T_STRING {
      // attribute-name-only e.g. { foo, bar }
      struct AstNode* variable = getVariable($1);
      if (variable != NULL) {
        yyerror("variable used by object does not exists");
      }
      struct AstNode* node = createNodeReference(variable->astNodeValue._string);
      pushObjectElement($1, node);
      $$ = node;
    }
  | object_element_name T_COLON expression {
      struct AstNode* value = (struct AstNode*)($3);
      // attribute-name : attribute-value
      pushObjectElement($1, $3);
    }
  ;


object_element_name:
    T_STRING {
      $$ = ($1);
    }
  | T_QUOTED_STRING {
      $$ = ($1);
    }

operator_binary:
    expression T_OR expression {
      struct AstNode* left = (struct AstNode*)($1);
      struct AstNode* right = (struct AstNode*)($3);

      if(left->astNodeType == NODE_TYPE_VARIABLE && left->astNodeValue.type != BOOL_TYPE ||
        right->astNodeType == NODE_TYPE_VARIABLE && right->astNodeValue.type != BOOL_TYPE) {
          yyerror("wrong OR operands");
      }
      $$ = createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_OR, $1, $3);
    }
  | expression T_AND expression {
      struct AstNode* left = (struct AstNode*)($1);
      struct AstNode* right = (struct AstNode*)($3);

      if(left->astNodeType == NODE_TYPE_VARIABLE && left->astNodeValue.type != BOOL_TYPE ||
        right->astNodeType == NODE_TYPE_VARIABLE && right->astNodeValue.type != BOOL_TYPE) {
          yyerror("wrong AND operands");
      }
      $$ = createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_AND, $1, $3);
    }
  | expression T_EQ expression {
      struct AstNode* left = (struct AstNode*)($1);
      struct AstNode* right = (struct AstNode*)($3);

      if(left->astNodeType == NODE_TYPE_VARIABLE && right->astNodeType == NODE_TYPE_VARIABLE
          && left->astNodeValue.type != right->astNodeValue.type
        ) {
          yyerror("wrong EQ operands");
      }

      $$ = createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_EQ, $1, $3);
    }
  | expression T_NE expression {
      struct AstNode* left = (struct AstNode*)($1);
      struct AstNode* right = (struct AstNode*)($3);

      if(left->astNodeType == NODE_TYPE_VARIABLE && right->astNodeType == NODE_TYPE_VARIABLE
          && left->astNodeValue.type != right->astNodeValue.type
        ) {
          yyerror("wrong NE operands");
      }

      $$ = createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_NE, $1, $3);
    }
  | expression T_LT expression {
      struct AstNode* left = (struct AstNode*)($1);
      struct AstNode* right = (struct AstNode*)($3);

      if(left->astNodeType == NODE_TYPE_VARIABLE && left->astNodeValue.type != INT_TYPE ||
        right->astNodeType == NODE_TYPE_VARIABLE && right->astNodeValue.type != INT_TYPE) {
          yyerror("wrong T_LT operands");
      }
      $$ = createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_LT, $1, $3);
    }
  | expression T_GT expression {
      struct AstNode* left = (struct AstNode*)($1);
      struct AstNode* right = (struct AstNode*)($3);

      if(left->astNodeType == NODE_TYPE_VARIABLE && left->astNodeValue.type != INT_TYPE ||
        right->astNodeType == NODE_TYPE_VARIABLE && right->astNodeValue.type != INT_TYPE) {
          yyerror("wrong T_GT operands");
      }
      $$ = createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_GT, $1, $3);
    }
  | expression T_LE expression {
      struct AstNode* left = (struct AstNode*)($1);
      struct AstNode* right = (struct AstNode*)($3);

      if(left->astNodeType == NODE_TYPE_VARIABLE && left->astNodeValue.type != INT_TYPE ||
        right->astNodeType == NODE_TYPE_VARIABLE && right->astNodeValue.type != INT_TYPE) {
          yyerror("wrong T_LE operands");
      }
      $$ = createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_LE, $1, $3);
    }
  | expression T_GE expression {
      $$ = createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_GE, $1, $3);

      struct AstNode* left = (struct AstNode*)($1);
      struct AstNode* right = (struct AstNode*)($3);

      if(left->astNodeType == NODE_TYPE_VARIABLE && left->astNodeValue.type != INT_TYPE ||
        right->astNodeType == NODE_TYPE_VARIABLE && right->astNodeValue.type != INT_TYPE) {
          yyerror("wrong T_GE operands");
      }
    }
  | expression T_IN expression {
      $$ = createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_IN, $1, $3);
    }
  ;


contains:
  T_CONTAINS T_OPEN expression T_COMMA expression T_CLOSE {
    struct AstNode* left = (struct AstNode*)($3);
    struct AstNode* right = (struct AstNode*)($5);
    if(((left->astNodeType == NODE_TYPE_VARIABLE && left->astNodeValue.type == STRING_TYPE) || left->astNodeType == NODE_TYPE_REFERENCE) ||
      ((right->astNodeType == NODE_TYPE_VARIABLE && right->astNodeValue.type == STRING_TYPE) || right->astNodeType == NODE_TYPE_REFERENCE)
    ) {
      struct AstNode* containsNode = createNodeContains(left, right);
      $$ = containsNode;
    } else {
        yyerror("wrong contains expression");
    }
  }


expression:
 operator_binary {
      $$ = $1;
    }
  | value_literal {
        $$ = $1;
    }
  | reference {
      $$ = $1;
    }
  | contains {
    $$ = $1;
  }
  ;






reference:
    T_STRING {
      // variable or collection or view
      struct AstNode* node = NULL;
      void* variable = NULL;

      variable = getVariable($1);
      if (variable == NULL) {
          node = createNodeDataSource($1);
      } else {
        node = variable;
      }
      $$ = node;
    }
    | reference '.' T_STRING {
        struct AstNode* ref = (struct AstNode*)($1);
        struct AstNode* access = createNodeAttributeAccess($3);
        addOperation(ref, access);
        $$ = ref;
    }
    | T_OPEN expression T_CLOSE {
        $$ = $2;
    }
    | object {
      $$ = $1;
    }


  ;



numeric_value:
    T_INTEGER {
      $$ = $1;
    }
  | T_DOUBLE {
      $$ = $1;
    }
  ;

value_literal:
    T_QUOTED_STRING {
      $$ = createNodeValueString($1);
    }
  | numeric_value {
      $$ = $1;
    }
  | T_NULL {
      $$ = createNodeValueNull();
    }
  | T_TRUE {
      $$ = createNodeValueBool(true);
    }
  | T_FALSE {
      $$ = createNodeValueBool(false);
    }
  ;

in_collection_name:
    T_STRING {
      $$ = createNodeDataSource($1);
    }
  ;


variable_name:
    T_STRING {
      $$ = ($1);
    }
  ;

%%
