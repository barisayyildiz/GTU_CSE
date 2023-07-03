#include <iostream>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "include/MemoryManagement.h"

using namespace std;

MemoryManagement *mm = NULL;

int main(){
  int virtualPageCount = 16;
  int pageFrameCount = 8;
  // with 4kb page size
  mm = new MemoryManagement(virtualPageCount, pageFrameCount, "MyDisk.bin", SC, 4096, 0);

  // matrixMultiplication(1, 1000, 2, 3, 2000, 3, 2, 6000);

  matrixMultiplication(1, 0, 1000, 3, 3000, 3, 1, 12000);
  // printf("--------------------------------\n");
  matrixMultiplication(1, 4000, 1, 1000, 5000, 1000, 1, 15000);
  // printf("--------------------------------\n");
  arraySummation(1, 12000, 12999, 15000, 15000);
  // printf("--------------------------------\n");
  testSearchAlgorithms(1, 0, 999);
  // printf("--------------------------------\n");

  saveFile("MyDisk.bin", virtualPageCount);
  mm->showPageTable();
  mm->forceDiskWrite();
  mm->showPageTable();


  return 0;
}
