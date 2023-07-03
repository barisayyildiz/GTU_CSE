%{
	int toggleQuote = 0;
%}

%%
"and"															printf("KW_AND\n");
"or"															printf("KW_OR\n");
"not"															printf("KW_NOT\n");
"equal"														printf("KW_EQUAL\n");
"less"														printf("KW_LESS\n");
"nil"															printf("KW_NIL\n");
"list"														printf("KW_LIST\n");
"append"													printf("KW_APPEND\n");
"concat"													printf("KW_CONCAT\n");
"set"															printf("KW_SET\n");
"deffun"													printf("KW_DEFFUN\n");
"for"															printf("KW_FOR\n");
"if"															printf("KW_IF\n");
"exit"														printf("KW_EXIT\n");
"load"														printf("KW_LOAD\n");
"disp"														printf("KW_DISP\n");
"true"														printf("KW_TRUE\n");
"false"														printf("KW_FALSE\n");
"+"																printf("OP_PLUS\n");
"-"																printf("OP_MINUS\n");
"/"																printf("OP_DIV\n");
"*"																printf("OP_MULT\n");
"("																printf("OP_OP\n");
")"																printf("OP_CP\n");
"**"															printf("OP_DBLMULT\n");
","																printf("OP_COMMA\n");
";;".*														printf("COMMENT\n");
"\n"
" "
"\""															if(toggleQuote == 0){
																		printf("OP_OC\n");
																	}else{
																		printf("OP_CC\n");
																		toggleQuote = !toggleQuote;
																	}
[1-9][0-9]*|0											printf("VALUE\n");
[_a-zA-Z][_a-zA-Z0-9]*						printf("IDENTIFIER\n");
[0][0-9]+													printf("SYNTAX_ERROR %s cannot be tokenized\n", yytext);
.																	printf("SYNTAX_ERROR %s cannot be tokenized\n", yytext);

%%

int yywrap(){
	return 1;
}

int main(){
	int ntoken, vtoken;

	ntoken = yylex();
	while(ntoken){
		printf("%d\n", ntoken);
		ntoken = yylex();
	}

}

