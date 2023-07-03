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

// ./part2 12 5 10 LRU classic 1000 diskFileName.dat
int main(int argc, char* argv[]){
  double pageSize = pow(2,atoi(argv[1])+2);
  int pageSizeInt = (int)pageSize;
  double virtualPageCount = pow(2, atoi(argv[3]));
  double pageFrameCount = pow(2, atoi(argv[2]));
  PageReplacement pr;
  if(strcmp(argv[4], "LRU") == 0){
    pr = LRU;
  }else if(strcmp(argv[4], "WSClock") == 0){
    pr = WSClock;
  }else if(strcmp(argv[4], "SC") == 0){
    pr = SC;
  }
  printf("virtualPageCount : %f\n", virtualPageCount);
  printf("pageFrameCount : %f\n", pageFrameCount);
  printf("%s\n", argv[7]);
  printf("%d\n", pr);
  printf("%d\n", pageSizeInt);
  printf("%d\n", atoi(argv[6]));
  // mm = new MemoryManagement(virtualPageCount, pageFrameCount, argv[7], pr, (int)pageSize, atoi(argv[6]));
  mm = new MemoryManagement((int)virtualPageCount, (int)pageFrameCount, argv[7], pr, (int)pageSize, atoi(argv[6]));
  // mm->showPageTable();

  int n1[6];
  int n2[6];
  int n3[6];
  int n4[6];

  matrixMultiplication(0, 1000, 1000, 3, 3000, 3, 1, 30000);
  n1[0] = MemoryManagement::Number_Of_Reads;
  n1[1] = MemoryManagement::Number_Of_Writes;
  n1[2] = MemoryManagement::Number_Of_Page_Miss;
  n1[3] = MemoryManagement::Number_Of_Page_Replacements;
  n1[4] = MemoryManagement::Number_Of_Disk_Page_Writes;
  n1[5] = MemoryManagement::Number_Of_Disk_Page_Reads;
  clearStaticVariables();
  // printf("--------------------------------\n");
  matrixMultiplication(0, 22000, 1, 1000, 27000, 1000, 1, 17000);
  n2[0] = MemoryManagement::Number_Of_Reads;
  n2[1] = MemoryManagement::Number_Of_Writes;
  n2[2] = MemoryManagement::Number_Of_Page_Miss;
  n2[3] = MemoryManagement::Number_Of_Page_Replacements;
  n2[4] = MemoryManagement::Number_Of_Disk_Page_Writes;
  n2[5] = MemoryManagement::Number_Of_Disk_Page_Reads;
  clearStaticVariables();
  // printf("--------------------------------\n");
  arraySummation(0, 30000, 31000, 17000, 17000);
  n3[0] = MemoryManagement::Number_Of_Reads;
  n3[1] = MemoryManagement::Number_Of_Writes;
  n3[2] = MemoryManagement::Number_Of_Page_Miss;
  n3[3] = MemoryManagement::Number_Of_Page_Replacements;
  n3[4] = MemoryManagement::Number_Of_Disk_Page_Writes;
  n3[5] = MemoryManagement::Number_Of_Disk_Page_Reads;
  clearStaticVariables();
  // printf("--------------------------------\n");
  testSearchAlgorithms(0, 0, 999);
  n4[0] = MemoryManagement::Number_Of_Reads;
  n4[1] = MemoryManagement::Number_Of_Writes;
  n4[2] = MemoryManagement::Number_Of_Page_Miss;
  n4[3] = MemoryManagement::Number_Of_Page_Replacements;
  n4[4] = MemoryManagement::Number_Of_Disk_Page_Writes;
  n4[5] = MemoryManagement::Number_Of_Disk_Page_Reads;
  clearStaticVariables();
  // printf("--------------------------------\n");

  std::cout << "Statistic for the first multiplication function" << "\n";
  std::cout << "o Number of reads : " << n1[0] << "\n";
  std::cout << "o Number of writes : " << n1[1] << "\n";
  std::cout << "o Number of page misses : " << n1[2] << "\n";
  std::cout << "o Number of page replacements : " << n1[3] << "\n";
  std::cout << "o Number of disk page writes : " << n1[4] << "\n";
  std::cout << "o Number of disk page reads : " << n1[5] << "\n\n";

  std::cout << "Statistic for the second multiplication function" << "\n";
  std::cout << "o Number of reads : " << n2[0] << "\n";
  std::cout << "o Number of writes : " << n2[1] << "\n";
  std::cout << "o Number of page misses : " << n2[2] << "\n";
  std::cout << "o Number of page replacements : " << n2[3] << "\n";
  std::cout << "o Number of disk page writes : " << n2[4] << "\n";
  std::cout << "o Number of disk page reads : " << n2[5] << "\n\n";

  std::cout << "Statistic for the summation function" << "\n";
  std::cout << "o Number of reads : " << n3[0] << "\n";
  std::cout << "o Number of writes : " << n3[1] << "\n";
  std::cout << "o Number of page misses : " << n3[2] << "\n";
  std::cout << "o Number of page replacements : " << n3[3] << "\n";
  std::cout << "o Number of disk page writes : " << n3[4] << "\n";
  std::cout << "o Number of disk page reads : " << n3[5] << "\n\n";

  std::cout << "Statistic for the search function" << "\n";
  std::cout << "o Number of reads : " << n4[0] << "\n";
  std::cout << "o Number of writes : " << n4[1] << "\n";
  std::cout << "o Number of page misses : " << n4[2] << "\n";
  std::cout << "o Number of page replacements : " << n4[3] << "\n";
  std::cout << "o Number of disk page writes : " << n4[4] << "\n";
  std::cout << "o Number of disk page reads : " << n4[5] << "\n\n";

  // saveFile(argv[7], virtualPageCount);
  // mm->showPageTable();
  // mm->forceDiskWrite();
  // mm->showPageTable();


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
