#include <iostream>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "include/MemoryManagement.h"

using namespace std;

MemoryManagement *mm = NULL;

void clearStaticVariables();

int main(int argc, char* argv[]){
  // mm = new MemoryManagement(virtualPageCount, pageFrameCount, argv[7], pr, (int)pageSize, atoi(argv[6]));
  
  // mm->showPageTable();

  int n1[5];

  srand (time(NULL));
  int randomNumber;
  int pageSize;
  int virtualMemory = 512*1024;
  int physicalMemory = 64*1024;

  // physical memory has the size 64kb and virtual memory has 512kb

  for(int i=0; i<8; i++){
    pageSize = (int)(128 * pow(2,i));
    mm = new MemoryManagement(virtualMemory/pageSize, physicalMemory/pageSize, "MyDisk.bin", LRU, pageSize, 0);
    // for(int j=0; j<7; j++){
      matrixMultiplication(0, 0, 1000, 3, 3000, 3, 1, 12000);
      matrixMultiplication(0, 4000, 1, 1000, 5000, 1000, 1, 15000);
      arraySummation(0, 12000, 12999, 15000, 15000);
      testSearchAlgorithms(0, 20000, 21000);
    // }
    std::cout << "For page size : " << pageSize << ", " << "Number of page misses : " << MemoryManagement::Number_Of_Page_Miss << "\n";
    clearStaticVariables();
    mm = NULL;
  }



  return 0;
}

void clearStaticVariables() {
  MemoryManagement::Number_Of_Reads = 0;
  MemoryManagement::Number_Of_Writes = 0;
  MemoryManagement::Number_Of_Page_Miss = 0;
  MemoryManagement::Number_Of_Page_Replacements = 0;
  MemoryManagement::Number_Of_Disk_Page_Writes = 0;
  MemoryManagement::Number_Of_Disk_Page_Reads = 0;
}
