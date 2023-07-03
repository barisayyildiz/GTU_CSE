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
#include <pthread.h>

#include "include/mytypes.h"

pid_t parent_pid;
char rootAddress[256];

Request requestQueue[256];
int requestCounter = 0;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

pthread_mutex_t mutexClients;
pthread_mutex_t mutexClientsCounter;

pthread_mutex_t mutexClientsQueue;
pthread_mutex_t mutexClientsQueueCounter;

int (*clients)[2];
int clientsCounter;
int totalCounter;
int* clientsQueue;
int clientsQueueCounter;

void writeLog(char* message){
  int fd;

  fd = open("logfile.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
  // lock the log file for writing
  if (flock(fd, LOCK_EX) == -1) {
    printf("Error: could not lock log file.\n");
    close(fd);
    return;
  }
  write(fd, message, strlen(message));

  // unlock the log file
  flock(fd, LOCK_UN);

  // close the log file
  close(fd);
}

// signal handler for SIGINT (Ctrl+C)
void sigint_handler(int signum) {
  printf(">> kill signal, bye\n");
  writeLog("kill signal, bye\n");
  // send SIGTERM to all child processes
  kill(-parent_pid, SIGTERM);
  // exit the parent process
  exit(signum);
}

void server_send(int pid, char* message) {
  char fifoPath[256];
  char fifoPathTemplate[256] = "/tmp/fifo_%d";
  sprintf(fifoPath, fifoPathTemplate, pid);

  int fd = open(fifoPath, O_WRONLY);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  write(fd, message, strlen(message) + 1);
  close(fd);
}

char* getManualText(Request req){
  if(req.payloadSize == 0) {
    return "\tAvailable comments are : \n\t\thelp, list, readF, writeT, upload, download, quit, killServer\n";
  }else if(strcmp(req.payload[0], "help") == 0) {
    return "\thelp\n\t\tdisplay the list of possible client requests\n";
  }else if(strcmp(req.payload[0], "list") == 0) {
    return "\tlist\n\t\tsends a request to display the list of files in Servers directory\n";
  }else if(strcmp(req.payload[0], "readF") == 0) {
    return "\treadF <file> <line #>\n\t\trequests to display the # line of the <file>, if no line number is given\n"
    "the whole contents of the file is requested (and displayed on the client side)\n";
  }else if(strcmp(req.payload[0], "writeT") == 0) {
    return "\twriteT <file> <line #> <string>\n\t\trequest to write the content of “string” to the #th line the <file>, if the line # is not given\n"
    "writes to the end of file. If the file does not exists in Servers directory creates and edits the\n"
    "file at the same time\n";
  }else if(strcmp(req.payload[0], "upload") == 0) {
    return "\tupload <file\n""\t\tuploads the file from the current working directory of client to the Servers directory\n"
      "(beware of the cases no file in clients current working directory and file with the same\n"
      "name on Servers side)\n";
  }else if(strcmp(req.payload[0], "download") == 0) {
    return "\tdownload <file>\n""\t\trequest to receive <file> from Servers directory to client side\n";
  }else if(strcmp(req.payload[0], "quit") == 0) {
    return "\tquit\n""\t\tSend write request to Server side log file and quits\n";
  }else if(strcmp(req.payload[0], "killServer") == 0) {
    return "\tkillServer\n""\t\tSends a kill request to the Server\n";
  }
}

void printManual(Request req){
  char fifoPath[256];
  char fifoPathTemplate[256] = "/tmp/fifo_%d";
  sprintf(fifoPath, fifoPathTemplate, req.pid);

  int fd;
  fd = open(fifoPath, O_WRONLY);
  if(fd < 0){
    perror("open");
    exit(1);
  }

  char* message;
  
  message = getManualText(req);
  server_send(req.pid, message);

  close(fd);

}

void printListOfFiles(Request req){
  char fifoPath[256];
  char fifoPathTemplate[256] = "/tmp/fifo_%d";
  sprintf(fifoPath, fifoPathTemplate, req.pid);

  char *args[] = {"ls", rootAddress, NULL};
  char *argsCurrent[] = {"ls", NULL};

  printf("fifopath : %s\n", fifoPath);
  printf("rootAddress : %s\n", rootAddress);

  pid_t pid = fork();

  if (pid == 0) {
    // child process
    // redirect STDOUT to the FIFO
    int fd = open(fifoPath, O_WRONLY);
    if (fd == -1) {
      perror("open");
      exit(EXIT_FAILURE);
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);

    if(strcmp(rootAddress, "") != 0){
      execvp(args[0], args);
    } else{
      execvp(args[0], argsCurrent);
    }
  } else {
    wait(NULL);
  }
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

bool isNumber(char *number){
  char* endptr;
  long val;

  val = strtol(number, &endptr, 10);

  if (errno == ERANGE || *endptr != '\0') {
    return false;
  } else {
    return true;
  }
}

void readFileFull(Request req){
  char fifoPath[256];
  char fifoPathTemplate[256] = "/tmp/fifo_%d";
  sprintf(fifoPath, fifoPathTemplate, req.pid);
  char fileName[256];
  int fd;
  pid_t pid;

  char filePath[256];
  strcpy(filePath, rootAddress);
  strcat(filePath, req.payload[0]);

  pid = fork();
  if (pid == 0) {
    // child process
    fd = open(fifoPath, O_WRONLY);
    if(flock(fd, LOCK_EX) == -1){
      perror("can't lock file\n");
      close(fd);
      return;
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
    flock(fd, LOCK_UN);
    execvp("cat", (char *[]){"cat", filePath, NULL});

  } else if (pid > 0) {
    wait(NULL);
  } else {
    // Fork failed
    perror("fork");
    exit(1);
  }

}

void readFileLine(Request req, int target){
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int counter = 1;
  int fd;

  char filePath[256];
  strcpy(filePath, rootAddress);
  strcat(filePath, req.payload[0]);

  fp = fopen(filePath, "r");
  if (fp == NULL){
    server_send(req.pid, "file doesn't exists...");
    exit(EXIT_FAILURE);
  }

  fd = fileno(fp);
  if (flock(fd, LOCK_EX) == -1) {
    printf("can't lock file\n");
    return;
  }

  while ((read = getline(&line, &len, fp)) != -1) {
    if(counter == target){
      server_send(req.pid, line);
      break;
    }else{
      counter++;
    }
  }
  if(counter < target){
    server_send(req.pid, "requested file is smaller than the target size...");
  }

  flock(fd, LOCK_UN);
  fclose(fp);
  if (line) {
    free(line);
  }
}

// ["file.txt", "helloworld.txt"]
void writeFileEOF(Request req){
  char fifoPath[256];
  char fifoPathTemplate[256] = "/tmp/fifo_%d";
  sprintf(fifoPath, fifoPathTemplate, req.pid);
  char fileName[256];
  int fd;
  pid_t pid;
  char filePath[256];
  strcpy(filePath, rootAddress);
  strcat(filePath, req.payload[0]);

  printf("filepath : %s\n", filePath);

  // Fork a child process
  pid = fork();
  if (pid == 0) {
    // Child process
    // Open the file to append
    fd = open(filePath, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if(fd < 0){
      perror("open");
      exit(EXIT_FAILURE);
    }
    if(flock(fd, LOCK_EX) == -1){
      printf("can't lock file\n");
      close(fd);
      return;
    }
    if (write(fd, req.payload[1], strlen(req.payload[1])) == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
    if (close(fd) == -1) {
      perror("close");
      exit(EXIT_FAILURE);
    }
    flock(fd, LOCK_UN);
    server_send(req.pid, "text appended...");
  } else if (pid > 0) {
    wait(NULL);
  } else {
    // Fork failed
    perror("fork");
    exit(1);
  }
}

void writeFileLine(Request req){
  FILE * fRead;
  FILE * fWrite;
  int fd, fd2;
  char filename[BUF_SIZE];
  char buffer[BUF_SIZE];
  char newline[BUF_SIZE];

  char filePath[256];
  strcpy(filePath, rootAddress);
  strcat(filePath, req.payload[0]);

  int target = atoi(req.payload[1]);
  strcpy(newline, req.payload[2]);
  strcat(newline, "\n");

  fRead = fopen(filePath, "r");
  fd = fileno(fRead);
  if (flock(fd, LOCK_EX) == -1) {
    perror("can't lock the file...");
    return;
  }
  fWrite = fopen("temp.temp", "w");
  fd2 = fileno(fWrite);
  if (flock(fd2, LOCK_EX) == -1) {
    perror("can't lock the file...");
    return;
  }

  if(fRead == NULL || fWrite == NULL){
    server_send(req.pid, "file doent exists...");
    return;
  }

  int counter = 0;
  while((fgets(buffer, BUF_SIZE, fRead)) != NULL){
    counter++;
    if(counter == target){
        fputs(newline, fWrite);
    }else{
        fputs(buffer, fWrite);
    }
  }

  fclose(fRead);
  fclose(fWrite);

  remove(filePath);

  /* Rename temporary file as original file */
  rename("temp.temp", filePath);
  flock(fd, LOCK_UN);
  flock(fd2, LOCK_UN);

  server_send(req.pid, "text appended...");
}

void downloadFile(Request req) {
  ssize_t bytes_read, bytes_written;

  char* oldpath = req.payload[0];
  int status;
  char filePath[256];

  strcpy(filePath, rootAddress);
  strcat(filePath, req.payload[0]);

  char fifoPath[256];
  char fifoPathTemplate[256] = "/tmp/fifo_%d";
  sprintf(fifoPath, fifoPathTemplate, req.pid);

  // when the address is .
  if(strcmp(rootAddress, "") == 0){
    server_send(req.pid, "SUCCESS");
    return;
  }
  server_send(req.pid, "donwload request received...\n");


  int downloadedFile = open(filePath, O_RDONLY);
  if (flock(downloadedFile, LOCK_EX) == -1) {
    close(downloadedFile);
    return;
  }

  char buffer[256];

  int fd = open(fifoPath, O_WRONLY);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  char uploadBuffer[BUF_SIZE];
  // Read and write block by block
  while ((bytes_read = read(downloadedFile, uploadBuffer, BUF_SIZE)) > 0) {
    bytes_written = write(fd, uploadBuffer, bytes_read);
    if (bytes_written == -1) {
      server_send(req.pid, "Error during upload...\n");
      return;
    }
  }

  flock(downloadedFile, LOCK_UN);
  close(fd);
  close(downloadedFile);
}

// upload <file>
// type: upload
// payload: <file>
void uploadFile(Request req) {
  ssize_t bytes_read, bytes_written;

  char* oldpath = req.payload[0];
  int status;
  char filePath[256];

  strcpy(filePath, rootAddress);
  strcat(filePath, req.payload[0]);

  // when the address is .
  if(strcmp(rootAddress, "") == 0){
    server_send(req.pid, "SUCCESS");
    return;
  }
  server_send(req.pid, "upload request received...\n");


  int uploadedFile = open(filePath, O_WRONLY | O_CREAT , 0666);
  if (flock(uploadedFile, LOCK_EX) == -1) {
    close(uploadedFile);
    return;
  }

  char fifoPath[256];
  char fifoPathTemplate[256] = "/tmp/fifo_%d";
  sprintf(fifoPath, fifoPathTemplate, req.pid);

  char buffer[256];

  int fd = open(fifoPath, O_RDONLY);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  char uploadBuffer[BUF_SIZE];
  // Read and write block by block
  while ((bytes_read = read(fd, uploadBuffer, BUF_SIZE)) > 0) {
    bytes_written = write(uploadedFile, uploadBuffer, bytes_read);
    if (bytes_written == -1) {
      server_send(req.pid, "Error during upload...\n");
      return;
    }
  }

  flock(fd, LOCK_UN);
  close(fd);
  close(uploadedFile);
}

char* getProcessName(int pid){
  pthread_mutex_lock(&mutexClients);
  pthread_mutex_lock(&mutexClientsCounter);
  int index;
  for(int i=0; i<clientsCounter; i++){
    if(clients[i][0] == pid){
      index = clients[i][1];
      break;
    }
  }
  
  char tmp[10];
  sprintf(tmp, "%d", index);
  char *name = (char*)malloc(sizeof(char)*10);
  strcpy(name, "client");
  strcat(name, tmp);

  pthread_mutex_unlock(&mutexClientsCounter);
  pthread_mutex_unlock(&mutexClients);

  return name;
}

void acceptWaitingClient(){
  pthread_mutex_lock(&mutexClientsQueue);
  pthread_mutex_lock(&mutexClientsQueueCounter);
  if(clientsQueueCounter > 0){
    int next_pid = clientsQueue[0];

    for(int i=0; i<clientsQueueCounter-1; i++){
      clientsQueue[i] = clientsQueue[i+1];
    }
    clientsQueueCounter--;

    pthread_mutex_lock(&mutexClientsCounter);
    pthread_mutex_lock(&mutexClients);
    clients[clientsCounter][0] = next_pid;
    clients[clientsCounter][1] = totalCounter++;
    clientsCounter++;
    pthread_mutex_unlock(&mutexClients);
    pthread_mutex_unlock(&mutexClientsCounter);
    
    // prevent zombie processes
    signal(SIGCHLD, SIG_IGN);

    char tempBuffer[256];

    sprintf(tempBuffer, "Client PID %d connected as “%s”\n", next_pid, getProcessName(next_pid));
    writeLog(tempBuffer);

    // send a dummy message to client
    server_send(next_pid, "CONNECTION_ESTABLISHED");
  }


  pthread_mutex_unlock(&mutexClientsQueue);
  pthread_mutex_unlock(&mutexClientsQueueCounter);
}

void clientQuit(Request req){
  // =========================== CRITICAL REGIION =========================== //
  pthread_mutex_lock(&mutexClients);
  pthread_mutex_lock(&mutexClientsCounter);

  char buffer[BUF_SIZE];
  // get process name
  int indexName;
  for(int i=0; i<clientsCounter; i++){
    if(clients[i][0] == req.pid){
      indexName = clients[i][1];
      break;
    }
  }

  // shift rest of the clients to left in clients array
  int index;
  for(int i=0; i<clientsCounter; i++){
    if(req.pid == clients[i][0]){
      index = i;
      break;
    }
  }
  for(int i=index; i<clientsCounter; i++){
    clients[i][0] = clients[i+1][0];
    clients[i][1] = clients[i+1][1];
  }

  sprintf(buffer, "client%d disconnected...\n", indexName);
  writeLog(buffer);

  printf("%s", buffer);
  clientsCounter--;

  server_send(req.pid, "quit");

  pthread_mutex_unlock(&mutexClientsCounter);
  pthread_mutex_unlock(&mutexClients);

  // =========================== CRITICAL REGIION =========================== //

  acceptWaitingClient();

}

void handleRequest(Request req){
  if(strcmp(req.type, "help") == 0){
    printManual(req);
  } else if(strcmp(req.type, "list") == 0) {
    printListOfFiles(req);
  } else if(strcmp(req.type, "quit") == 0){
    clientQuit(req);
  } else if(strcmp(req.type, "killServer") == 0){
    printf("kill signal from %s... terminating...\n", getProcessName(req.pid));
    writeLog("server killed with its child process...\n");
    server_send(req.pid, "server killed with its child process...\n");
    raise(SIGINT);
  } else if(strcmp(req.type, "readF") == 0) {
    if(req.payloadSize == 1){
      readFileFull(req);
    }else if(req.payloadSize == 2 && isNumber(req.payload[1])){
      readFileLine(req, atoi(req.payload[1]));
    }else{
      server_send(req.pid, "invalid payload...");
    }
  } else if(strcmp(req.type, "writeT") == 0){
    if(req.payloadSize == 2){
      writeFileEOF(req);
    }else if(req.payloadSize == 3 && isNumber(req.payload[1])){
      writeFileLine(req);
    }
  } else if(strcmp(req.type, "upload") == 0){
    if(req.payloadSize != 1){
      server_send(req.pid, "invalid payload...");
    }else{
      uploadFile(req);
    }
  } else if(strcmp(req.type, "download") == 0){
    if(req.payloadSize != 1){
      server_send(req.pid, "invalid payload...");
    }else{
      downloadFile(req);
    }
  }
   else {
    server_send(req.pid, "unknown command");
  }
  
}

// ==================================== FOR THREADS ==================================== //
// add a new request to queue
void submitRequest(Request req){
  pthread_mutex_lock(&mutexQueue);
  requestQueue[requestCounter++] = req;
  pthread_mutex_unlock(&mutexQueue);
  pthread_cond_signal(&condQueue);
}

void* startThread(void* arg){
  int t_id = *(int*)arg;
  while(1){
    Request req;
    // ============== CRITICAL REGION ==================== //
    pthread_mutex_lock(&mutexQueue);
    while(requestCounter == 0){
      pthread_cond_wait(&condQueue, &mutexQueue);
    }
    req = requestQueue[0];
    for(int i=0; i<requestCounter-1; i++){
      requestQueue[i] = requestQueue[i+1];
    }
    requestCounter--;
    pthread_mutex_unlock(&mutexQueue);
    // ============== CRITICAL REGION ==================== //
    handleRequest(req);
  }
}

int main(int argc, char *argv[]){
  int fd;
  int fdClient;
  int fdLog;

  if(strcmp(argv[1], "Here") == 0 || strcmp(argv[1], ".") == 0){
    strcpy(rootAddress, "");
  }else{
    strcpy(rootAddress, argv[1]);
    strcat(rootAddress, "/");
  }
  mkdir(rootAddress, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  int MAX_NUMBER_OF_CLIENTS = atoi(argv[2]);
  int POOL_SIZE = atoi(argv[3]);

  clientsQueue = (int*)malloc(sizeof(int)*10);
  clientsQueueCounter = 0;

  clientsCounter = 0;
  clients = malloc(sizeof(*clients) * MAX_NUMBER_OF_CLIENTS);
  totalCounter = 0;
  parent_pid = getpid();

  printf(">> Server Started PID %d...\n", parent_pid);
  printf(">> waiting for clients...\n");

  // set the signal handler for SIGINT
  signal(SIGINT, sigint_handler);

  char fifoPath[256];
  char fifoPathTemplate[256] = "/tmp/fifo_%d";
   
  char myfifo[256];
  sprintf(myfifo, "/tmp/biboServer_%d", (int)getpid());
  mkfifo(myfifo, 0666);

  // just simply create a log file
  fdLog = open("logfile.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
  close(fdLog);
  
  char buffer[BUF_SIZE];
  char tempBuffer[BUF_SIZE];
  int childPid;

  Request req;
  int readRes;

  pthread_mutex_init(&mutexClients, NULL);
  pthread_mutex_init(&mutexClientsCounter, NULL);
  pthread_mutex_init(&mutexClientsQueue, NULL);
  pthread_mutex_init(&mutexClientsQueueCounter, NULL);

  // thread pool oluşturuluyor
  pthread_t* threadPool = (pthread_t*)malloc(sizeof(pthread_t) * POOL_SIZE);
  pthread_mutex_init(&mutexQueue, NULL);
  pthread_cond_init(&condQueue, NULL);
  int i;
  for(i=0; i<5; i++){
    if(pthread_create(&threadPool[i], NULL, &startThread, (void*)&i) != 0){
      perror("failed to create thread");
    }
  }


  while(1){
    fd = open(myfifo, O_RDONLY);
    readRes = read(fd, &req, sizeof(Request));
    close(fd);
    if(readRes == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }else if(readRes == 0){
      continue;
    }

    // a new process trying to connect
    if(strcmp(req.type, "Connect") == 0 || strcmp(req.type, "tryConnect") == 0){
      // =========================== CRITICAL REGIION =========================== //
      
      // sem_wait(clientsCounter_sem);
      pthread_mutex_lock(&mutexClientsCounter);
      if(strcmp(req.type, "tryConnect") == 0 && clientsCounter == MAX_NUMBER_OF_CLIENTS){
        sprintf(fifoPath, fifoPathTemplate, req.pid);
        fdClient = open(fifoPath, O_WRONLY);
        server_send(req.pid, "SERVER_IS_OUT_OF_CAPACITY");
        close(fdClient);

        sprintf(tempBuffer, "Try Connect request PID %d... Que FULL... exiting\n", req.pid);
        writeLog(tempBuffer);

        printf("%s", tempBuffer);
        pthread_mutex_unlock(&mutexClientsCounter);
        continue;
      }
      pthread_mutex_unlock(&mutexClientsCounter);
      
      // =========================== CRITICAL REGIION =========================== //

      // sem_wait(clientsCounter_sem);
      pthread_mutex_lock(&mutexClientsCounter);
      if(strcmp(req.type, "Connect") == 0 && clientsCounter == MAX_NUMBER_OF_CLIENTS){
        sprintf(tempBuffer, "Connection request PID %d... Que FULL\n", req.pid);
        writeLog(tempBuffer);
        printf("%s", tempBuffer);
        
        pthread_mutex_lock(&mutexClientsQueue);
        pthread_mutex_lock(&mutexClientsQueueCounter);
        clientsQueue[clientsQueueCounter++] = req.pid;
        pthread_mutex_unlock(&mutexClientsQueueCounter);
        pthread_mutex_unlock(&mutexClientsQueue);
      }
      pthread_mutex_unlock(&mutexClientsCounter);
      
      // =========================== CRITICAL REGIION =========================== //
      pthread_mutex_lock(&mutexClientsCounter);

      if(clientsCounter < MAX_NUMBER_OF_CLIENTS) {
        pthread_mutex_lock(&mutexClients);
        clients[clientsCounter][0] = req.pid;
        clients[clientsCounter][1] = totalCounter++;
        clientsCounter++;
        pthread_mutex_unlock(&mutexClients);
        pthread_mutex_unlock(&mutexClientsCounter);
      }else{
        pthread_mutex_unlock(&mutexClientsCounter);
        continue;
      }
      // =========================== CRITICAL REGIION =========================== //
      
      signal(SIGCHLD, SIG_IGN);
      sprintf(tempBuffer, "Client PID %d connected as “%s”\n", req.pid, getProcessName(req.pid));
      writeLog(tempBuffer);

      printf("%s", tempBuffer);

      // send a dummy message to client
      server_send(req.pid, "CONNECTION_ESTABLISHED");

    } else {
      // kullanıcı normal istekte bulunuyor
      submitRequest(req);

    }
  }

  pthread_mutex_destroy(&mutexQueue);
  pthread_mutex_destroy(&mutexClients);
  pthread_mutex_destroy(&mutexClientsCounter);
  pthread_mutex_destroy(&mutexClientsQueue);
  pthread_mutex_destroy(&mutexClientsQueueCounter);
  pthread_cond_destroy(&condQueue);

  close(fd);

  return 0;
}


