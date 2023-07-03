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

pid_t parent_pid;
char rootAddress[256];

void writeLog(char* message){
  int fd;

  fd = open("logfile.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
  // Lock the log file for writing
  if (flock(fd, LOCK_EX) == -1) {
    printf("Error: could not lock log file.\n");
    close(fd);
    return;
  }
  write(fd, message, strlen(message));

  // Unlock the log file
  flock(fd, LOCK_UN);

  // Close the log file
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

void server_send(Request req, char* message) {
  char fifoPath[256];
  char fifoPathTemplate[256] = "/tmp/fifo_%d";
  sprintf(fifoPath, fifoPathTemplate, req.pid);

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

  // printf("fifoPath : %s\n", fifoPath);

  int fd;
  fd = open(fifoPath, O_WRONLY);
  if(fd < 0){
    perror("open");
    exit(1);
  }

  // dup2(fd, STDOUT_FILENO);
  char* message;
  
  message = getManualText(req);
  server_send(req, message);

  close(fd);

}

void printListOfFiles(Request req){
  char fifoPath[256];
  char fifoPathTemplate[256] = "/tmp/fifo_%d";
  sprintf(fifoPath, fifoPathTemplate, req.pid);

  char *args[] = {"ls", rootAddress, NULL};

  // printf("inside printlistoffiles...\n");

  // fork a child process
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

    // execute the command
    // execl("/bin/ls", "/bin/ls", NULL);
    execvp(args[0], args);
  } else {
    // parent process
    // wait for the child to finish
    wait(NULL);
  }
}

int createFifo(int pid){
  // printf("beginning of create fifo\n");
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

  // Convert string to long integer
  val = strtol(number, &endptr, 10);

  // Check if conversion was successful
  if (errno == ERANGE || *endptr != '\0') {
    return false;
  } else {
    return true;
  }
}

void readFileFull(Request req){

  // printf("inside readFileFull...\n");

  char fifoPath[256];
  char fifoPathTemplate[256] = "/tmp/fifo_%d";
  sprintf(fifoPath, fifoPathTemplate, req.pid);
  char fileName[256];
  int fd;
  pid_t pid;

  // printf("rf : %s\n", req.payload[0]);
  char filePath[256];
  strcpy(filePath, rootAddress);
  strcat(filePath, req.payload[0]);

  // Fork a child process
  pid = fork();
  if (pid == 0) {
    // Child process
    // Open the named pipe for writing
    fd = open(fifoPath, O_WRONLY);
    if(flock(fd, LOCK_EX) == -1){
      perror("can't lock file\n");
      close(fd);
      return;
    }
    // Redirect stdout to the named pipe
    dup2(fd, STDOUT_FILENO);
    close(fd);
    flock(fd, LOCK_UN);
    // Execute the cat command to read the file and send its output to the named pipe
    execvp("cat", (char *[]){"cat", filePath, NULL});
    // If execvp returns, an error occurred

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
    server_send(req, "file doesn't exists...");
    exit(EXIT_FAILURE);
  }

  fd = fileno(fp);
  if (flock(fd, LOCK_EX) == -1) {
    printf("can't lock file\n");
    return;
  }

  while ((read = getline(&line, &len, fp)) != -1) {
    if(counter == target){
      server_send(req, line);
      break;
    }else{
      counter++;
    }
  }
  if(counter < target){
    server_send(req, "requested file is smaller than the target size...");
  }

  flock(fd, LOCK_UN);
  fclose(fp);
  if (line) {
    free(line);
  }
}

// ["file.txt", "helloworld.txt"]
void writeFileEOF(Request req){
  // printf("inside writeFileEOF...\n");

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

  // printf("rf : %s\n", req.payload[0]);

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
    server_send(req, "text appended...");
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
  int fd;
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

  if(fRead == NULL || fWrite == NULL){
    server_send(req, "file doent exists...");
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

  flock(fd, LOCK_UN);
  fclose(fRead);
  fclose(fWrite);

  remove(filePath);

  /* Rename temporary file as original file */
  rename("temp.temp", filePath);

  server_send(req, "text appended...");
}

// upload <file>
// type: upload
// payload: <file>
void downloadFile(Request req) {
  int source_fd, dest_fd;
  char buffer[BUF_SIZE];
  ssize_t bytes_read, bytes_written;

  int status;
  char filePath[256];

  strcpy(filePath, rootAddress);
  strcat(filePath, req.payload[0]);

  // Open the source file for reading
  source_fd = open(filePath, O_RDONLY);
  if (source_fd == -1) {
    server_send(req, "Error during download...\n");
    return;
  }
  if (flock(source_fd, LOCK_EX) == -1) {
    close(source_fd);
    return;
  }

  // Open the destination file for writing
  dest_fd = open(req.payload[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (dest_fd == -1) {
    perror("Error opening destination file");
    server_send(req, "Error during download...\n");
    return;
  }
  if (flock(dest_fd, LOCK_EX) == -1) {
    close(dest_fd);
    return;
  }


  // Read and write block by block
  while ((bytes_read = read(source_fd, buffer, BUF_SIZE)) > 0) {
    bytes_written = write(dest_fd, buffer, bytes_read);
    if (bytes_written == -1) {
      server_send(req, "Error during download...\n");
      return;
    }
  }

  // Check for read error
  if (bytes_read == -1) {
    server_send(req, "Error during download...\n");
    return;
  }

  flock(source_fd, LOCK_UN);
  flock(dest_fd, LOCK_UN);

  // Close the files
  close(source_fd);
  close(dest_fd);

  printf("File copied successfully.\n");
  server_send(req, "file downloaded successfully..\n");
}

// upload <file>
// type: upload
// payload: <file>
void uploadFile(Request req) {
  Block block;

  char* oldpath = req.payload[0];
  int status;
  char filePath[256];

  strcpy(filePath, rootAddress);
  strcat(filePath, req.payload[0]);

  // when the address is .
  if(strcmp(rootAddress, "") == 0){
    server_send(req, "file uploaded successfully\n");
    return;
  }
  server_send(req, "upload request received...\n");


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

  while(1) {
    read(fd, &block, sizeof(Block));
    if(block.status == 0){
      server_send(req, "done...\n");
      break;
    }else{
      write(uploadedFile, block.buffer, strlen(block.buffer));
      // printf("buffer : %s\n", block.buffer);
    }
  }

  flock(fd, LOCK_UN);
  close(fd);
  close(uploadedFile);
}

char* getProcessName(int pid, int (*clients)[2], int* clientsCounter, sem_t* clients_sem, sem_t* clientsCounter_sem){
  sem_wait(clients_sem);
  sem_wait(clientsCounter_sem);
  int index;
  for(int i=0; i<clientsCounter[0]; i++){
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

  sem_post(clients_sem);
  sem_post(clientsCounter_sem);

  return name;
}

void clientQuit(Request req, int* clientsCounter, int (*clients)[2], sem_t* clients_sem, sem_t* clientsCounter_sem, sem_t* isAvailable_sem){
  // =========================== CRITICAL REGIION =========================== //
  sem_wait(clientsCounter_sem);  

  char buffer[BUF_SIZE];
  // get process name
  int indexName;
  for(int i=0; i<clientsCounter[0]; i++){
    if(clients[i][0] == req.pid){
      indexName = clients[i][1];
      break;
    }
  }

  // shift rest of the clients to left in clients array
  int index;
  for(int i=0; i<clientsCounter[0]; i++){
    if(req.pid == clients[i][0]){
      index = i;
      break;
    }
  }
  for(int i=index; i<clientsCounter[0]; i++){
    clients[i][0] = clients[i+1][0];
    clients[i][1] = clients[i+1][1];
  }

  sprintf(buffer, "client%d disconnected...\n", indexName);
  writeLog(buffer);

  printf("%s", buffer);
  clientsCounter[0]--;
  server_send(req, "quit");

  sem_post(clientsCounter_sem); 
  sem_post(isAvailable_sem);
  // =========================== CRITICAL REGIION =========================== //

}

void handleRequest(Request req, int* clientsCounter, int (*clients)[2], sem_t* clients_sem, sem_t* clientsCounter_sem, sem_t* isAvailable_sem){
  // printf("inside handle request...\n");
  if(strcmp(req.type, "help") == 0){
    printManual(req);
    // server_send(req, "abc\n");
  } else if(strcmp(req.type, "list") == 0) {
    printListOfFiles(req);
    // printf("done...\n");
  } else if(strcmp(req.type, "quit") == 0){
    clientQuit(req, clientsCounter, clients, clients_sem, clientsCounter_sem, isAvailable_sem);
  } else if(strcmp(req.type, "killServer") == 0){
    printf("kill signal from %s... terminating...\n", getProcessName(req.pid, clients, clientsCounter, clients_sem, clientsCounter_sem));
    writeLog("server killed with its child process...\n");
    server_send(req, "server killed with its child process...\n");
    raise(SIGINT);
  } else if(strcmp(req.type, "readF") == 0) {
    // printf("inside readF condition...\n");
    // printf("paylaod size : %d\n", req.payloadSize);
    if(req.payloadSize == 1){
      readFileFull(req);
    }else if(req.payloadSize == 2 && isNumber(req.payload[1])){
      readFileLine(req, atoi(req.payload[1]));
    }else{
      server_send(req, "invalid payload...");
    }
  } else if(strcmp(req.type, "writeT") == 0){
    // printf("inside writeT condition...\n");
    // printf("paylaod size : %d\n", req.payloadSize);
    if(req.payloadSize == 2){
      writeFileEOF(req);
    }else if(req.payloadSize == 3 && isNumber(req.payload[1])){
      writeFileLine(req);
    }
  } else if(strcmp(req.type, "upload") == 0){
    if(req.payloadSize != 1){
      server_send(req, "invalid payload...");
    }else{
      uploadFile(req);
    }
  } else if(strcmp(req.type, "download") == 0){
    if(req.payloadSize != 1){
      server_send(req, "invalid payload...");
    }else{
      downloadFile(req);
    }
  }
   else {
    server_send(req, "unknown command");
  }
  
}

int main(int argc, char *argv[]){
  int fd;
  int fdClient;
  int fdLog;
  int (*clients)[2];

  if(strcmp(argv[1], "Here") == 0 || strcmp(argv[1], ".") == 0){
    strcpy(rootAddress, "");
  }else{
    strcpy(rootAddress, argv[1]);
    strcat(rootAddress, "/");
  }
  mkdir(rootAddress, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  // printf("%s\n", rootAddress);
  int MAX_NUMBER_OF_CLIENTS = atoi(argv[2]);
  // printf("%s\n", argv[2]);

  int* clientsCounter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  clients = mmap(NULL, sizeof(*clients) * MAX_NUMBER_OF_CLIENTS, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  int* totalCounter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  // Initialize semaphores
  sem_t* clients_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_t* clientsCounter_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_t* isAvailable_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_t* totalCounter_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_t* request_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_init(clients_sem, 1, 1);    // Initialize to 1 for mutual exclusion
  sem_init(clientsCounter_sem, 1, 1);  // Initialize to 1 for mutual exclusion
  sem_init(isAvailable_sem, 1, MAX_NUMBER_OF_CLIENTS);
  sem_init(totalCounter_sem, 1, 1);
  sem_init(request_sem, 1, 1);

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

  // printf("hello2\n");
  fd = open(myfifo, O_RDONLY);

  // just simply create a log file
  fdLog = open("logfile.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
  close(fdLog);
  
  char buffer[BUF_SIZE];
  char tempBuffer[BUF_SIZE];
  int childPid;

  Request req;
  int readRes;

  // printf("fd_main : %d\n", fd);
  // printf("hello3\n");

  while(1){
    // printf("hello4\n");
    sem_wait(request_sem);
    readRes = read(fd, &req, sizeof(Request));
    if(readRes == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }else if(readRes == 0){
      continue;
    }
    // printf("==== readRes : %d\n", readRes);

    // printf("req.type : %s\n", req.type);
    // printf("main_process_id : %d\n", (int)getpid());

    // a new process trying to connect
    if(strcmp(req.type, "Connect") == 0 || strcmp(req.type, "tryConnect") == 0){
      // sleep(2);
      // =========================== CRITICAL REGIION =========================== //
      
      sem_wait(clientsCounter_sem);
      if(strcmp(req.type, "tryConnect") == 0 && clientsCounter[0] == MAX_NUMBER_OF_CLIENTS){
        // printf("bu condition içerisindeyimm.....\n");
        // printf("size : %d\n", clientsCounter[0]);
        sprintf(fifoPath, fifoPathTemplate, req.pid);
        fdClient = open(fifoPath, O_WRONLY);
        server_send(req, "SERVER_IS_OUT_OF_CAPACITY");
        close(fdClient);

        sprintf(tempBuffer, "Try Connect request PID %d... Que FULL... exiting\n", req.pid);
        writeLog(tempBuffer);

        printf("%s", tempBuffer);
        sem_post(clientsCounter_sem);
        continue;
      }
      sem_post(clientsCounter_sem);
      
      // =========================== CRITICAL REGIION =========================== //

      sem_wait(clientsCounter_sem);
      if(strcmp(req.type, "Connect") == 0 && clientsCounter[0] == MAX_NUMBER_OF_CLIENTS){
        sprintf(tempBuffer, "Connection request PID %d... Que FULL\n", req.pid);
        writeLog(tempBuffer);
        printf("%s", tempBuffer);
      }
      sem_post(clientsCounter_sem);
      
      // =========================== CRITICAL REGIION =========================== //
      sem_wait(isAvailable_sem);
      sem_wait(clientsCounter_sem);

      
      // printf("number of clients before : %d\n", clientsCounter[0]);
      if(clientsCounter[0] < MAX_NUMBER_OF_CLIENTS) {
        sem_wait(totalCounter_sem);
        sem_wait(clients_sem);
        clients[clientsCounter[0]][0] = req.pid;
        clients[clientsCounter[0]][1] = totalCounter[0]++;
        clientsCounter[0]++;
        sem_post(clients_sem);
        sem_post(totalCounter_sem);
        sem_post(clientsCounter_sem);
      }else{
        sem_post(clientsCounter_sem);
        continue;
      }
      sem_wait(clientsCounter_sem);
      // değilse bekleyecek
      // printf("number of clients after : %d\n", clientsCounter[0]);
      sem_post(clientsCounter_sem);
      // =========================== CRITICAL REGIION =========================== //
      
      // prevent zombie processes
      signal(SIGCHLD, SIG_IGN);
      sem_post(request_sem);
      childPid = fork();
      // printf("childPid : %d\n", childPid);
      if(childPid == 0){
        sprintf(tempBuffer, "Client PID %d connected as “%s”\n", req.pid, getProcessName(req.pid, clients, clientsCounter, clients_sem, clientsCounter_sem));
        writeLog(tempBuffer);
        printf("%s", tempBuffer);

        // CONNENCT başarılı bir şekilde gerçekleşti
        sprintf(fifoPath, fifoPathTemplate, req.pid);
        fdClient = open(fifoPath, O_WRONLY);
        server_send(req, "CONNECTION_ESTABLISHED");
        close(fdClient);

        // inside child process
        // create fifo for client
        // TODO: add validations
        // createFifo(req.pid);
        while(1){
          // printf("debugger_childPid : %d\n", childPid);
          sprintf(fifoPath, fifoPathTemplate, req.pid);
          fdClient = open(fifoPath, O_RDONLY);
          if(read(fdClient, &req, sizeof(Request)) == -1){
            perror("read");
            exit(EXIT_FAILURE);
          }
          close(fdClient);
          // printf("child_process req.type : %s\n", req.type);

          if(strcmp(req.type, "upload") == 0){
            uploadFile(req);
            continue;
          }

          if(strcmp(req.type, "quit") == 0){
            clientQuit(req, clientsCounter, clients, clients_sem, clientsCounter_sem, isAvailable_sem);
            // printf("before kill...\n");
            kill(getpid(), SIGTERM);
            // printf("after kill...\n");
            break;
          }
          handleRequest(req, clientsCounter, clients, clients_sem, clientsCounter_sem, isAvailable_sem);
        }
      }else{
        // wait(NULL);
      }
    }

  }

  close(fd);
  munmap(clients, sizeof(int) * 5);
  munmap(clientsCounter, sizeof(int));

  return 0;
}


