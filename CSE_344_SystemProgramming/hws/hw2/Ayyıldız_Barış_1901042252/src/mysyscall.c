#include <stdio.h>
#include <stdlib.h>
#include "../include/mysyscall.h"
#include "../include/mystring.h"

char* generatePath(char* command){
  return concatStrings("/bin/", command);
}

// ls
// cat input.txt
// cat input.txt input.txt2
char** generateParameters(char** tokens){
  int size = sizeOfTokenizer(tokens);
  char* command;
  char** rest;
  char** res;

  if(size == 0){
    return;
  }else if(size == 1){
    res = malloc(sizeof(char*)*2);
    res[0] = tokens[0];
    res[1] = NULL;
    return res;
  }

  res = malloc(sizeof(char*)*(size+1));
  for(int i=0; i<size; i++){
    res[i] = tokens[i];
  }
  res[size] = NULL;
  return res;
}
