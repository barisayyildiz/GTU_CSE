#ifndef MY_SERVER
#define MY_SERVER

#define BUF_SIZE 1024

typedef struct Request {
  int pid;
  char type[BUF_SIZE];
  char payload[3][BUF_SIZE];
  int payloadSize;
} Request;

#endif
