%option yylineno
%option noyywrap nounput batch

%{
#include "ast.h"
#include "grammar.h"
#define YY_NO_INPUT 1
%}

%%

":" {
  return T_COLON;
}


"FOR" {
  return T_FOR;
}

"FILTER" {
  return T_FILTER;
}

"RETURN" {
  return T_RETURN;
}
"{" {
  return T_OBJECT_OPEN;
}

"}" {
  return T_OBJECT_CLOSE;
}


"IN" {
  return T_IN;
}

"REMOVE" {
  return T_REMOVE;
}

"INSERT" {
  return T_INSERT;
}

"UPDATE" {
  return T_UPDATE;
}

"TRUE" {
  return T_TRUE;
}

"FALSE" {
  return T_FALSE;
}


"==" {
  return T_EQ;
}

"!=" {
  return T_NE;
}

">=" {
  return T_GE;
}

">" {
  return T_GT;
}

"<=" {
  return T_LE;
}

"<" {
  return T_LT;
}


"," {
  return T_COMMA;
}

"(" {
  return T_OPEN;
}

")" {
  return T_CLOSE;
}


"!" {
  return T_NOT;
}

"&&" {
  return T_AND;
}

"||" {
  return T_OR;
}

"CONTAINS" {
  return T_CONTAINS;
}

"CREATE_TABLE" {
  return T_CREATE_TABLE;
}

"DROP_TABLE" {
  return T_DROP_TABLE;
}



(\"[^\"]*\") {
  yylval.s_val = copyQuotedString(yytext);
  printf("%s", yytext);
  return T_QUOTED_STRING;
}

([a-zA-Z]+[_a-zA-Z0-9]*) {
  yylval.s_val = copyStr(yytext);
  return T_STRING;
}

(0|[1-9][0-9]*) {
  yylval.node = createNodeValueInt(atoi(yytext));
  return T_INTEGER;
}


((0|[1-9][0-9]*)(\.[0-9]+)?|\.[0-9]+)([eE][\-\+]?[0-9]+)? {
  double val = atof(yytext);
  yylval.node = createNodeValueDouble(val);
  return T_DOUBLE;
}


[ \t\r]+ {
}



. {
  return (int) yytext[0];
}


%%
