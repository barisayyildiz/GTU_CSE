#ifndef MY_STRING
#define MY_STRING

int strlength(char* s);
char* strstrip(char* str);
int strcompare(char* s, char* s2);
char** stringTokenizer(char *str, char c);
int sizeOfTokenizer(char** tokenizer);
char** sliceTokenizer(char** tokenizer, int start, int end);
char** concatTokenizer(char** tokenizer1, char** tokenizer2);
char* concatStrings(char* str, char* str2);
int indexOf(char** tokens, char* token);

#endif
