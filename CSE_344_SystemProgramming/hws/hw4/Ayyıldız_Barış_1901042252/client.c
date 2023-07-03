#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>

#include "include/mytypes.h"

int serverPid;

// this function takes a char array named buffer as an input
// it return a Request pointer
// it sets Request struct's type, pid and payload
Request* generateRequest(char* buffer) {
  char* token;
  Request* req = (Request*)malloc(sizeof(Request));
  req->pid = (int)getpid();
  req->payloadSize = 0;
  memset(req->payload, 0, sizeof(req->payload));
  memset(req->type, 0, sizeof(req->type));

  token = strtok(buffer, " ");

  if(strcmp(token, "CONNECT") == 0){
    strcpy(req->type, "CONNECT");
  }else if(strcmp(token, "help") == 0){
    strcpy(req->type, "help");
    while(1){
      token = strtok(NULL, " ");
      if(token == NULL){
        break;
      }
      strcpy(req->payload[req->payloadSize++], token);
    }
  }else if(strcmp(token, "list") == 0){
    strcpy(req->type, "list");    
  }else if(strcmp(token, "readF") == 0){
    strcpy(req->type, "readF");
    while(1){
      token = strtok(NULL, " ");
      if(token == NULL){
        break;
      }
      strcpy(req->payload[req->payloadSize++], token);
    }
  }else if(strcmp(token, "writeT") == 0){
    strcpy(req->type, "writeT");
    while(1){
      token = strtok(NULL, " ");
      if(token == NULL){
        break;
      }
      strcpy(req->payload[req->payloadSize++], token);
    }
  }else if(strcmp(token, "upload") == 0){
    strcpy(req->type, "upload");
    while(1){
      token = strtok(NULL, " ");
      if(token == NULL){
        break;
      }
      strcpy(req->payload[req->payloadSize++], token);
    }
  }else if(strcmp(token, "download") == 0){
    strcpy(req->type, "download");
    while(1){
      token = strtok(NULL, " ");
      if(token == NULL){
        break;
      }
      strcpy(req->payload[req->payloadSize++], token);
    }
  }else if(strcmp(token, "quit") == 0){
    strcpy(req->type, "quit");
  }else if(strcmp(token, "killServer") == 0){
    strcpy(req->type, "killServer");
  }

  return req;
}

// signal handler for SIGINT (Ctrl+C)
void sigint_handler(int signum) {
  printf(">> kill signal, bye\n");

  char myfifo[256];
  sprintf(myfifo, "/tmp/biboServer_%d", serverPid);

  Request *req = generateRequest("quit");
  int fd = open(myfifo, O_WRONLY);
  if(write(fd, req, sizeof(Request)) == -1){
    perror("write");
    exit(EXIT_FAILURE);
  }
  close(fd);

  char readBuffer[256];
  
  char fifoPath[256];
  char fifoPathTemplate[256] = "/tmp/fifo_%d";
  sprintf(fifoPath, fifoPathTemplate, (int)getpid());
  
  fd = open(fifoPath, O_RDONLY);
  if(read(fd, readBuffer, 256) == -1) {
    perror("read");
    exit(EXIT_FAILURE);
  }
  close(fd);
  
  // exit the parent process
  exit(signum);
}

int createFifo(int pid){
  char fifoPath[256];
  char fifoPathTemplate[256] = "/tmp/fifo_%d";
  sprintf(fifoPath, fifoPathTemplate, pid);
  if (mkfifo(fifoPath, 0666) < 0) {
    perror("mkfifo");
    exit(EXIT_FAILURE);
  }
}

int isValidRequest(Request *req){
  if(req->type[0] == '\0'){
    return 0;
  }
  return 1;
}

// biboClient <Connect/tryConnect> ServerPID
int main(int argc, char *argv[]){
  if(argc < 3){
    printf("please give proper inputs...\n");
    return 0;
  }

  serverPid = atoi(argv[2]);
  if(strcmp(argv[1], "Connect") == 0 || strcmp(argv[1], "tryConnect") == 0){
    printf(">> Waiting for Que... ");
  }else{
    printf("please give proper inputs...\n");
    return 0;
  }


  int fd;
  int fdClient;
  char readBuffer[256];

  createFifo((int)getpid());
  char fifoPath[256];
  char fifoPathTemplate[256] = "/tmp/fifo_%d";
  sprintf(fifoPath, fifoPathTemplate, (int)getpid());

  char myfifo[256];
  sprintf(myfifo, "/tmp/biboServer_%d", serverPid);
  
  char buffer[BUF_SIZE];

  Request* req = (Request*)malloc(sizeof(Request));

  // set the signal handler for SIGINT
  signal(SIGINT, sigint_handler);


  if(strcmp(argv[1], "Connect") == 0){
    fd = open(myfifo, O_WRONLY);
    if(fd < 0){
      printf("Server PID is not valid...\n");
      return 1;
    }
    req->pid = (int)getpid();
    strcpy(req->type, "Connect");
    write(fd, req, sizeof(Request));
    close(fd);

    // wait for a response
    fdClient = open(fifoPath, O_RDONLY);
    while(1){
    if(read(fdClient, readBuffer, 256) == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    close(fdClient);
    if(strcmp(readBuffer, "CONNECTION_ESTABLISHED") == 0){
      printf("Connection established: \n");
      break;
    }
  }
  }else if(strcmp(argv[1], "tryConnect") == 0){
    fd = open(myfifo, O_WRONLY);
    if(fd < 0){
      printf("Server PID is not valid...\n");
      return 1;
    }
    req->pid = (int)getpid();
    strcpy(req->type, "tryConnect");
    write(fd, req, sizeof(Request));
    close(fd);

    // wait for a response
    fdClient = open(fifoPath, O_RDONLY);
    while(1){
      if(read(fdClient, readBuffer, 256) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      close(fdClient);
      if(strcmp(readBuffer, "CONNECTION_ESTABLISHED") == 0){
        printf("Connection established: \n");
        break;
      }else if(strcmp(readBuffer, "SERVER_IS_OUT_OF_CAPACITY") == 0){
        printf("server is out of capacity, closing....\n");
        return 0;
      }
    }
  }

  int res;
  while(1){
    printf(">> ");
    fgets(buffer, BUF_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    req = generateRequest(buffer);
    if(!isValidRequest(req)){
      // skip if it is not a valid request
      continue;
    }

    fd = open(myfifo, O_WRONLY);
    if(write(fd, req, sizeof(Request)) == -1){
      perror("write");
      exit(EXIT_FAILURE);
    }
    close(fd);

    if(strcmp(req->type, "upload") == 0){

      // read the message and pass
      fdClient = open(fifoPath, O_RDONLY);
      if(read(fdClient, readBuffer, 256) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      close(fdClient);

      if(strcmp(readBuffer, "SUCCESS") == 0){
        continue;
      }

      fdClient = open(fifoPath, O_WRONLY);
      int fdFile = open(req->payload[0], O_RDONLY);
      if (fdFile == -1) {
        perror("Error opening source file");
        continue;
      }

      char uploadBuffer[BUF_SIZE];
      size_t bytes_read, bytes_written;
      int success = 1;

      // Read and write block by block
      while ((bytes_read = read(fdFile, uploadBuffer, BUF_SIZE)) > 0) {
        bytes_written = write(fdClient, uploadBuffer, bytes_read);
        if (bytes_written == -1) {
          perror("error during upload...\n");
          success = 0;
          break;
        }
      }
      if(success){
        printf("\nfile uploaded successfully\n");
      }

      close(fdFile);
      close(fdClient);
    }

    else if(strcmp(req->type, "download") == 0){

      // read the message and pass
      fdClient = open(fifoPath, O_RDONLY);
      if(read(fdClient, readBuffer, 256) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      close(fdClient);
      
      if(strcmp(readBuffer, "SUCCESS") == 0){
        continue;
      }

      fdClient = open(fifoPath, O_RDONLY);
      int fdFile = open(req->payload[0], O_WRONLY | O_CREAT, 0666);
      if (fdFile == -1) {
        perror("Error opening source file");
        continue;
      }

      char uploadBuffer[BUF_SIZE];
      size_t bytes_read, bytes_written;
      int success = 1;

      // Read and write block by block
      while ((bytes_read = read(fdClient, uploadBuffer, BUF_SIZE)) > 0) {
        bytes_written = write(fdFile, uploadBuffer, bytes_read);
        if (bytes_written == -1) {
          perror("error during download...\n");
          success = 0;
          break;
        }
      }
      if(success){
        printf("\nfile download successfully\n");
      }

      close(fdFile);
      close(fdClient);
    }

    // read responses
    else if(strcmp(req->type, "help") == 0 || strcmp(req->type, "list") == 0 ||
     strcmp(req->type, "quit") == 0 || strcmp(req->type, "killServer") == 0 ||
     strcmp(req->type, "readF") == 0 ||strcmp(req->type, "writeT") == 0 ||
     strcmp(req->type, "upload") == 0 || strcmp(req->type, "download") == 0
     ){
      fdClient = open(fifoPath, O_RDONLY);
      if(read(fdClient, readBuffer, 256) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      close(fdClient);
      if(strcmp(readBuffer, "quit") == 0){
        break;
      }
      printf("\n%s\n", readBuffer);

    }
  }

  close(fd);

  return 0;
}


