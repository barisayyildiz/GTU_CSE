%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdbool.h>
	#include "y.tab.h"

	extern int yylex();
	void yyerror(char *msg);
%}

%%
"("																		{return OP_OP;}
")"																		{return OP_CP;}
"+"																		{return OP_PLUS;}
"-"																		{return OP_MINUS;}
"*"																		{return OP_MULT;}
"/"																		{return OP_DIV;}
"and"																	{return KW_AND;}
"or"																	{return KW_OR;}
"not"																	{return KW_NOT;}
"equal"																{return KW_EQUAL;}
"true"																{yylval.b=true; return BinaryValue;}
"false"																{yylval.b=false; return BinaryValue;}
"set"																	{return KW_SET;}
"defvar"															{return KW_DEFVAR;}
"concat"															{return KW_CONCAT;}
"append"															{return KW_APPEND;}
"\'"																	{return OP_OC_CC;}
"defun"															{return KW_DEFUN;}
"if"																	{return KW_IF;}
"while"																{return KW_WHILE;}
"for"																	{return KW_FOR;}
"list"																{return KW_LIST;}

";;".*																{return COMMENT;}
[ \t\n]																{;}
[[:digit:]]|[1-9][[:digit:]]+					{yylval.i=atoi(yytext); return IntegerValue;}						
[[:alpha:]][[:alnum:]]*								{strcpy(yylval.s, yytext); return IDENTIFIER;}

%%

int yywrap(){
	return 1;
}

