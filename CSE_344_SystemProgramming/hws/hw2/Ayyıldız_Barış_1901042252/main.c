#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include "./include/mystring.h"
#include "./include/mysyscall.h"


int main() {

  char buffer[200];
  char** tokenPipes;
  char** commandTokens;
  char** params;
  char* command;
  int fd;
  int indexInput;
  int indexOutput;
  pid_t pid;

  while(1){
    printf("$ ");
    scanf("%[^\n]", buffer);
    getchar();

    tokenPipes = stringTokenizer(buffer, '|');
    int sizeTokenPipes = sizeOfTokenizer(tokenPipes);

    tokenPipes[0] = strstrip(tokenPipes[0]);
    commandTokens = stringTokenizer(tokenPipes[0], ' ');
  
    indexInput = indexOf(commandTokens, "<");  // input
    if(indexInput != -1){
      fd = open(commandTokens[indexInput+1], O_RDONLY);
      char** sliced = sliceTokenizer(commandTokens, 0, indexInput-1);
      params = generateParameters(sliced);
      pid = fork();
      if(pid == 0){ 
        // child process
        dup2(fd, 0);
        close(fd);
        
        execv(generatePath(commandTokens[0]), params);
        perror("execl error");
        exit(1);
      }
    }
    indexOutput = indexOf(commandTokens, ">");  // output
    if(indexOutput != -1){
      fd = open(commandTokens[indexOutput+1], O_WRONLY | O_CREAT, 0777);
      char** sliced = sliceTokenizer(commandTokens, 0, indexOutput-1);
      params = generateParameters(sliced);
      pid = fork();
      if(pid == 0){ 
        // child process
        dup2(fd, 1);
        close(fd);
        
        execv(generatePath(commandTokens[0]), params);
        perror("execl error");
        exit(1);
      }
    }

    if(indexInput == -1 && indexOutput == -1){
      params = generateParameters(commandTokens);
      command = commandTokens[0];

      pid = fork();
      if(pid == 0){
        // child process
        execv(generatePath(command), params);
        return 1;
      }
    }

    wait(NULL);
    if(strcompare(":q", buffer) == 0){
      break;
    }
  }

  return 0;
}



