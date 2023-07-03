#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>   // for directory handling
#include <sys/stat.h> // for file information
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>

#define FILE_NAME_LENGTH 256

typedef struct {
  char source[FILE_NAME_LENGTH];
  char destination[FILE_NAME_LENGTH];
} ThreadArgs;

typedef struct {
  int source_fd;
  int destination_fd;
  char source_file_name[FILE_NAME_LENGTH];
  char destination_file_name[FILE_NAME_LENGTH];
} Files;

typedef struct {
  Files *buffer;
  int counter;
  int cap;
  pthread_mutex_t mutex;
  pthread_cond_t full;
  pthread_cond_t empty;
} TaskQueue;

TaskQueue task_queue;
int finished = 0;
int num_of_files_copied = 0;
// int active_consumers = 0;

void destroyTaskQueue() {
  pthread_mutex_destroy(&task_queue.mutex);
  pthread_cond_destroy(&task_queue.full);
  pthread_cond_destroy(&task_queue.empty);
  free(task_queue.buffer);
}

void sigintHandler(int sig_num) {
  printf("signal received, bye...\n");
  exit(0);
}

void initTaskQueue(int buffer_size) {
  task_queue.buffer = (Files*)malloc(sizeof(Files) * buffer_size);
  task_queue.counter = 0;
  task_queue.cap = buffer_size;
  pthread_mutex_init(&task_queue.mutex, NULL);
  pthread_cond_init(&task_queue.full, NULL);
  pthread_cond_init(&task_queue.empty, NULL);
}

void* produce(void* arg) {
  ThreadArgs* args = (ThreadArgs*)arg;
  char* source_dir = args->source;
  char* destination_dir = args->destination;

  DIR* dir;
  struct dirent* entry;
  struct stat file_stat;
  char fullpath[512];

  dir = opendir(source_dir);
  if (dir == NULL) {
    perror("can't open directory");
    return NULL;
  }

  mkdir(destination_dir, 0777);

  while ((entry = readdir(dir)) != NULL) {
    snprintf(fullpath, sizeof(fullpath), "%s/%s", source_dir, entry->d_name);
    if (stat(fullpath, &file_stat) == -1) {
      perror("stat function error\n");
      continue;
    }

    Files new_file;
    snprintf(new_file.source_file_name, sizeof(new_file.source_file_name), "%s/%s", source_dir, entry->d_name);
    snprintf(new_file.destination_file_name, sizeof(new_file.destination_file_name), "%s/%s", destination_dir, entry->d_name);

    if (S_ISDIR(file_stat.st_mode)) {
      // Skip . and ..
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
      }
      // create the directory first
      mkdir(new_file.destination_file_name, 0777);
      // Recursively list files in subdirectories

      ThreadArgs* argsInner = (ThreadArgs*)malloc(sizeof(ThreadArgs));
      pthread_t thread;
      strcpy(argsInner->source, new_file.source_file_name);
      strcpy(argsInner->destination, new_file.destination_file_name);
      pthread_create(&thread, NULL, (void*)produce, (void*)argsInner);
      // pthread_join(thread, NULL);

      // produce(new_file.source_file_name, new_file.destination_file_name);

    } else {
      // handle produce
      new_file.source_fd = open(new_file.source_file_name, O_RDONLY, 0666);
      if (new_file.source_fd == -1) {
        perror("open source file error1");
        continue;
      }
      new_file.destination_fd = open(new_file.destination_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
      if (new_file.destination_fd == -1) {
        perror("open source file error2");
        close(new_file.source_fd);
        continue;
      }

      pthread_mutex_lock(&task_queue.mutex);

      // task queue is full
      while (task_queue.counter == task_queue.cap)
        pthread_cond_wait(&task_queue.full, &task_queue.mutex);

      task_queue.buffer[task_queue.counter++] = new_file;

      pthread_cond_signal(&task_queue.empty);
      pthread_mutex_unlock(&task_queue.mutex);
    }
  }

  // printf("finished...\n");
  finished = 1;

  closedir(dir);

  return NULL;
}

void* consume(void* args) {
  while (1) {

    printf("finished : %d, counter : %d\n", finished, task_queue.counter);

    printf("before lock...\n");
    pthread_mutex_lock(&task_queue.mutex);
    printf("after lock..\n");

    if (task_queue.counter == 0 && finished) {
      // active_consumers--;
      pthread_cond_signal(&task_queue.empty);
      pthread_mutex_unlock(&task_queue.mutex);
      break;
    }

    while (task_queue.counter == 0 && !finished) {
      pthread_cond_wait(&task_queue.empty, &task_queue.mutex);
    }

    Files files = task_queue.buffer[--task_queue.counter];
    pthread_cond_signal(&task_queue.full);
    pthread_mutex_unlock(&task_queue.mutex);

    char buffer[4096];
    ssize_t bytesRead;

    if (files.source_fd == -1 || files.destination_fd == -1) {
      continue;
    }

    while ((bytesRead = read(files.source_fd, buffer, 4096)) > 0) {
      ssize_t bytesWritten = write(files.destination_fd, buffer, bytesRead);
      if (bytesWritten == -1) {
        perror("write error");
        close(files.source_fd);
        close(files.destination_fd);
        return NULL;
      }
    }
    if (bytesRead == -1) {
      perror("read error");
      close(files.source_fd);
      close(files.destination_fd);
      return NULL;
    }

    close(files.source_fd);
    close(files.destination_fd);
    num_of_files_copied++;
  }

  // printf("out of thread function..\n");

  return NULL;
}

// buffer size, number of consumers, source, destination
int main(int argc, char* argv[]) {

  if(argc != 5){
    printf("Wrong format...\n");
    printf("./main <buffer size> <number of consumers> <source> <destination>\n");
    return 1;
  }

  int buffer_size = atoi(argv[1]);
  int number_of_consumers = atoi(argv[2]);

  if(buffer_size < 0 || number_of_consumers < 0){
    printf("Buffer size and number of consumers should be greater than 0\n");
    return 1;
  }

  signal(SIGINT, sigintHandler);
  initTaskQueue(buffer_size);


  ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
  strcpy(args->source, argv[3]);
  strcpy(args->destination, argv[4]);

  struct timeval start_time, end_time;
  gettimeofday(&start_time, NULL);

  pthread_t producer_thread;
  pthread_create(&producer_thread, NULL, &produce, (void*)args);

  pthread_t consumer_threads[2];
  for (int i = 0; i < 2; i++) {
    pthread_create(&consumer_threads[i], NULL, &consume, NULL);
    // active_consumers++;
  }

  for(int i=0; i<2; i++){
    pthread_join(consumer_threads[i], NULL);
  }

  // printf("test2...\n");

  gettimeofday(&end_time, NULL);
  double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
  
  destroyTaskQueue();


  printf("\n\n\n\nResults....\n");


  printf("Number of files copied : %d\n", num_of_files_copied);
  printf("Total time taken: %.6f seconds\n", elapsed_time);

  return 0;
}
