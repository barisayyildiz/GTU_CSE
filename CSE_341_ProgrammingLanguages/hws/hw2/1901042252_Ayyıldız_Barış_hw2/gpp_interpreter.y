%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>

	bool printResult = true;

	int intList[20];
	int intListSize = 0;

	int idList[20];
	int idListSize = 0;

	extern int yylex();
	void yyerror(char *msg);
%}

%union{
	int i;
	bool b;
	char s[50];
}

%type <i> EXPI
%type <b> EXPB

%token OP_OP
%token OP_CP
%token OP_PLUS
%token OP_MINUS
%token OP_MULT
%token OP_DIV
%token <s> IDENTIFIER
%token <i> IntegerValue
%token KW_AND
%token KW_EQUAL
%token KW_NOT
%token KW_OR
%token <b> BinaryValue
%token KW_SET
%token KW_DEFVAR
%token COMMENT
%token OP_OC_CC
%token KW_CONCAT
%token KW_APPEND
%token KW_DEFUN
%token KW_IF
%token KW_WHILE
%token KW_FOR
%token KW_LIST

%%
START	:	INPUT																{endOfParse();}
			| START INPUT													{endOfParse();}
INPUT	:	EXPI																{if(printResult){ printf("Result : %d\n",$1); }}
			| EXPB																{printf("%s\n", $1?"true":"false");}
			| EXPLISTI														{printf("Result : "); printList();}
			| COMMENT															{printf("Syntax OK.\n");}
EXPI	:	OP_OP OP_PLUS EXPI EXPI OP_CP				{$$=$3+$4;}
			|	OP_OP OP_MINUS EXPI EXPI OP_CP			{$$=$3-$4;}
			| OP_OP OP_MULT EXPI EXPI OP_CP				{$$=$3*$4;}
			| OP_OP OP_DIV EXPI EXPI OP_CP				{$$=$3/$4;}
			| IDENTIFIER																	{printf("%s is a valid token...\n", $1);}
			| IntegerValue												{printResult=false; $$=$1;}
			| OP_OP IDENTIFIER EXPLISTI OP_CP							{printResult=false; printf("Syntax OK.\n");}
			| OP_OP KW_SET IDENTIFIER EXPI OP_CP					{printResult=false; printf("Syntax OK.\n");}
			| OP_OP KW_DEFVAR IDENTIFIER EXPI OP_CP				{printResult=false; printf("Syntax OK.\n");}
			| OP_OP KW_DEFUN IDENTIFIER IDLIST EXPLISTI OP_CP							{printResult=false; printf("Syntax OK.\n");}
			| OP_OP KW_IF EXPB EXPLISTI OP_CP												{printResult=false; printf("Syntax OK.\n");}
			| OP_OP KW_IF EXPB EXPLISTI EXPLISTI OP_CP							{printResult=false; printf("Syntax OK.\n");}
			| OP_OP KW_WHILE OP_OP EXPB OP_CP EXPLISTI OP_CP				{printResult=false; printf("Syntax OK.\n");}
			| OP_OP KW_FOR OP_OP IDENTIFIER EXPI EXPI OP_CP EXPLISTI OP_CP	{printResult=false; printf("Syntax OK.\n");}


EXPB	:	OP_OP KW_AND EXPB EXPB OP_CP				{$$=($3 && $4);}
			| OP_OP KW_OR EXPB EXPB OP_CP					{$$=($3 || $4);}
			| OP_OP KW_NOT EXPB OP_CP							{$$=!$3;}
			| OP_OP KW_EQUAL EXPB EXPB OP_CP			{$$=($3 == $4);}
			| OP_OP KW_EQUAL EXPI EXPI OP_CP			{$$=($3 == $4);}
			| BinaryValue													{$$=$1;}
EXPLISTI	:	OP_OP KW_CONCAT EXPLISTI EXPLISTI OP_CP							{printList();}
					| OP_OP KW_APPEND EXPI EXPLISTI OP_CP									{appendIntList($3);}
					| LISTVALUE
LISTVALUE	:	OP_OC_CC OP_OP VALUES OP_CP
					| OP_OC_CC OP_OP OP_CP
					| OP_OP KW_LIST VALUES OP_CP
VALUES		:	VALUES IntegerValue											{appendIntList($2);}
					| IntegerValue														{appendIntList($1);}
IDLIST		:	OP_OP VALUES_ID OP_CP
					| OP_OP	OP_CP
VALUES_ID	:	VALUES_ID IDENTIFIER										{appendIdList($2);}
					| IDENTIFIER															{appendIdList($1);}

%%

void yyerror(char *msg){
	fprintf(stderr, "%s\n", "SYNTAX_ERROR Expression not recognized");
	exit(1);
}

void printList(){
	if(intListSize != 0){
		printf("(");
		for(int i=0; i<intListSize; i++){
			printf("%d ", intList[i]);
		}
		printf("\b)\n");
	}else{
		printf("()\n");
	}
}

void appendIntList(int item){
	intList[intListSize++] = item;
}

void appendIdList(char *identifier){
	idList[idListSize++] = identifier;
}

void clearList(){
	intListSize = 0;
	idListSize = 0;
}

void endOfParse(){
	printResult = true;
	clearList();
}

int main(){
	yyparse();
	return 0;
}

