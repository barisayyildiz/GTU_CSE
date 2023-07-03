#include <stdio.h>
#include <stdlib.h>
#include "../include/mystring.h"

int strlength(char* s){
  int length = 0;
  while(s[length] != '\0'){
    length++;
  }
  return length;
}

int strcompare(char* s, char* s2){
  int length = strlength(s);
  int length2 = strlength(s2);
  if(length != length2){
    return -1;
  }
  for(int i=0; i<length; i++){
    if(s[i] != s2[i]) return -1;
  }
  return 0;
}

char* strstrip(char* str){
  int length = strlength(str);

  if(length == 0){
    return str;
  }

  int startIndex = 0;
  int endIndex = length-1;

  while(str[startIndex] == ' '){
    startIndex++;
  }
  while(str[endIndex] == ' '){
    endIndex--;
  }

  char *res = malloc(sizeof(char) * (endIndex - startIndex + 1));

  for(int i=startIndex; i<=endIndex; i++){
    res[i-startIndex] = str[i];
  }
  return res;
}

char** stringTokenizer(char *str, char c){
  char** res;
  int counter = 0;
  int indexArray[20];
  int isSingleQuoteOpen = 0;
  int isDoubleQuoteOpen = 0;

  if(strlength(str) == 0){
    res = malloc(sizeof(char*));
    res[0] = malloc(sizeof(char));
    res[0] = str;
    return res;
  }

  for(int i=0; i<strlength(str); i++){
    if(c == ' '){
      if(str[i] == '\''){
        isSingleQuoteOpen = !isSingleQuoteOpen;
      }else if(str[i] == '\"'){
        isDoubleQuoteOpen = !isDoubleQuoteOpen;
      }else if(str[i] == ' ' && !isSingleQuoteOpen && !isDoubleQuoteOpen){
        indexArray[counter++] = i;
      }
    }else{
      if(str[i] == c){
        indexArray[counter++] = i;
      }
    }
  }

  if(counter == 0){
    res = malloc(sizeof(char*));
    res[0] = malloc(sizeof(char) * strlength(str));
    res[0] = str;
    return res;
  }

  res = malloc(sizeof(char*) * (counter + 2));
  // ab|e|de
  // 2, 4, 7
  int totalSaved = 0;
  int currentIndex = 0; 
  for(int i=0; i<counter; i++){
    res[totalSaved] = malloc(sizeof(char) * (indexArray[i] - currentIndex + 1));
    for(int j=currentIndex; j<indexArray[i]; j++){
      res[totalSaved][j-currentIndex] = str[j];
    }
    res[totalSaved][indexArray[i] - currentIndex] = '\0';

    currentIndex = indexArray[i] + 1;
    totalSaved++;
  }

  if(currentIndex < strlength(str)){
    res[totalSaved] = malloc(sizeof(char) * (strlength(str) - currentIndex + 1));
    for(int j=currentIndex; j<strlength(str); j++){
      res[totalSaved][j-currentIndex] = str[j];
    }
    res[totalSaved][strlength(str) - currentIndex] = '\0';
  }

  return res;
}

int sizeOfTokenizer(char** tokenizer){
  int counter = 0;
  while(tokenizer[counter] != NULL){
    counter++;
  }
  return counter;
}

// cat input.txt input2.txt
// t,1,2 -> input.txt input2.txt
// t,0,1 -> cat input.txt 
char** sliceTokenizer(char** tokenizer, int start, int end){
  char** res = malloc(sizeof(char*)*(end-start+2));
  int counter = 0;
  for(int i=0; i<=(end-start); i++){
    res[i] = tokenizer[start+i];
    counter++;
  }
  // [a,b,c,d], (0,2) => [a,b,c]
  res[counter] = NULL;
  return res;
}

// {abc, def} {123} => {abc def 123}
char** concatTokenizer(char** tokenizer1, char** tokenizer2){
  int size1 = sizeOfTokenizer(tokenizer1);
  int size2 = sizeOfTokenizer(tokenizer2);

  printf("size1 : %d\nsize2 : %d\n", size1, size2);

  char** res = malloc(sizeof(char*) * (size1+size2+1));
  int counter = 0;
  for(int i=0; i<size1; i++){
    printf("->%s\n", tokenizer1[i]);
    res[counter++] = tokenizer1[i];
  }
  printf("...........\n");
  printf("#%s\n", tokenizer2[0]);
  printf("#%s\n", tokenizer2[1]);
  for(int i=0; i<size2; i++){
    printf("->%s\n", tokenizer2[i]);
    res[counter++] = tokenizer2[i];
  }
  res[counter] = NULL;
  return res;
}

char* concatStrings(char* str, char* str2){
  int size = strlength(str);
  int size2 = strlength(str2);
  int idx = 0;

  char* res = malloc(sizeof(char) * (size+size2+1));
  for(int i=0; i<size; i++){
    res[idx++] = str[i];
  }
  for(int i=0; i<size2; i++){
    res[idx++] = str2[i];
  }
  res[idx] = '\0';
  return res;
}

int indexOf(char** tokens, char* token){
  for(int i=0; i<sizeOfTokenizer(tokens); i++){
    if(strcompare(tokens[i], token) == 0){
      return i;
    }
  }
  return -1;
}

