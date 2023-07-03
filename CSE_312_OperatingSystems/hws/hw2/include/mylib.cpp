#include <iostream>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "MemoryManagement.h"

using namespace std;

extern MemoryManagement *mm;

int linearSearch(int start, int end, int target){
  int idx = start;
  while(idx <= end){
    if(mm->get(idx) == target){
      return 1;
    }
    idx++;
  }
  return 0;
}

int binarySearch(int start, int end, int target){
  if(mm->get(start) == target || mm->get(end) == target) return 1;
  if(end - start <= 1) return 0;

  int mid = (start + end) / 2;
  if(mm->get(mid) == target) return 1;
  else if(mm->get(mid) > target) return binarySearch(start, mid, target);
  else if(mm->get(mid) < target) return binarySearch(mid, end, target);
}

void sortArray(int start, int end){
  int i, j;
  int temp;
  // m1000, m1001, m1002, m1003
  for (i=start; i<end; i++) {
    for(int j=start; j<end; j++){
      // swap
      if(mm->get(j) > mm->get(j+1)){
        temp = mm->get(j);
        mm->set(j, mm->get(j+1));
        mm->set(j+1, temp);
      }
    }
  }
}

void matrixMultiplication(int log, int startOne, int xOne, int yOne, int startTwo, int xTwo, int yTwo, int save){
  
  if(log){
    std::cout << "First Matrix\n";
    for(int i=0; i<xOne; i++){
      for(int j=0; j<yOne; j++){
        std::cout << mm->get(i*yOne+j) << " ";
      }
      std::cout << "\n";
    }   
    std::cout << "\nSecond Matrix\n";
    for(int i=0; i<xTwo; i++){
      for(int j=0; j<yTwo; j++){
        std::cout << mm->get(i*yTwo+j) << " ";
      }
      std::cout << "\n";
    }
  }
  
  int counter = 0;
  int result;
  for(int i=0; i<xOne; i++){
    for(int j=0; j<yTwo; j++){
      result = 0;
      for(int k=0; k<yOne; k++){
        // std::cout << startOne + i*yOne + k << " : " << startTwo + j + k*yTwo << "\n";
        result += mm->get(startOne + i*yOne + k) * mm->get(startTwo + j + k*yTwo);
      }
      // std::cout << "result : " << result << "\n";
      mm->set(save + counter++, result);
    }
  }
}

void multiplicationOne(int log){
  // matrix multiplication 1
  // (1000x3) x (3x1) => 1000x1
  // matrix ranges from 0 to 2999
  // vector ranges from 3000 to 3002
  // Matrix M
  // [
  //   0 1 2
  //   3 4 5
  //   6 7 8
  //   ...
  //   2997 2998 2999
  // ]
  // Vector V 
  // [
  //   3000
  //   3001
  //   3002
  // ]
  // Result R
  // [
  //   0*3000 + 1*3001 + 2*3002
  //   3*3000 + 4*3001 + 5*3002
  //   6*3000 + 7*3001 + 8*3002
  //   ...
  //   2997*3000 + 2998*3001 + 2999*3002
  // ]
  // and result ranges from 12000 to 7999
  // At memory location 12000 = 0*3000 + 1*3001 + 2*3002
  // At memory location 12001 = 3*3000 + 4*3001 + 5*3002
  // ...
  // At memory location 12999 = 2997*3000 + 2998*3001 + 2999*3002

  if(log){
    std::cout << "First Matrix\n";
    for(int i=0; i<999; i++){
      std::cout << mm->get(i*3) << " " << mm->get(i*3+1) << " " << mm->get(i*3+2) << "\n";
    }
    
    std::cout << "\nSecond Matrix\n";
    std::cout << mm->get(3000) << "\n" << mm->get(3001) << "\n" << mm->get(3002) << "\n";
  }


  int t1, t2, t3, t4, t5, t6;
  t4 = mm->get(3000);
  t5 = mm->get(3001);
  t6 = mm->get(3002);

  for(int i=0; i<999; i++){
    t1 = mm->get(i*3); 
    t2 = mm->get(i*3+1);
    t3 = mm->get(i*3+2);
    mm->set(i+12000,
      t1*t4 + t2*t5 + t3*t6
    );
  }

  if(log){
    std::cout << "Result, ranges from 12000 to 12999\n";
    for(int i=12000; i<13000; i++){
      std::cout << mm->get(i) << "\n";
    }  
  }

}

void multiplicationTwo(int log){
  // matrix multiplication 2
  // (1x1000) x (1000x1) => 1x1
  // matrix ranges from 4000 to 4999
  // vector ranges from 5000 to 5999
  // Vector V1
  // [
  //   4000, 4001, ..., 4999
  // ]
  // Vector V2
  // [
  //   5000,
  //   5001,
  //   ...,
  //   5999
  // ]
  // Result R
  // [
  //   4000*5000 + 4001*5001 + 4002*5002 + ... + 4999*5999
  // ]
  // and result is at 15000
  // At memory location 15000 = 4000*5000 + 4001*5001 + 4002*5002 + ... + 4999*5999
  
  if(log){
    std::cout << "First Matrix\n";
    for(int i=4000; i<5000; i++){
      std::cout << mm->get(i) << " ";
    }
    std::cout << "\n";
    
    std::cout << "\nSecond Matrix\n";
    for(int i=5000; i<6000; i++){
      std::cout << mm->get(i) << "\n";
    }
  }
  
  int t1 = 0;
  for(int i=0; i<999; i++){
    t1 += mm->get(i+4000) * mm->get(i+5000);
  }
  mm->set(15000, t1);

  if(log){
    std::cout << "Result, at the location 15000 : " << mm->get(15000);
  }

}

void arraySummation(int log, int startOne, int endOne, int startTwo, int endTwo){

  if(log){
    std::cout << "First array : \n";
    for(int i=startOne; i<=endOne; i++){
      std::cout << mm->get(i) << " ";
    }
    std::cout << "\nSecond array : \n";
    for(int i=startTwo; i<=endTwo; i++){
      std::cout << mm->get(i) << " ";
    };
    std::cout << "\n";
  }

  int total = 0;
  int idx;
  idx = startOne;
  while(idx <= endOne){
    total += mm->get(idx++);
  }
  idx = startTwo;
  while(idx <= endTwo){
    total += mm->get(idx++);
  }

  if(log){
    std::cout << "total : " << total << "\n";
  }
}

void *testSearchAlgorithms(int log, int start, int end){
  // first, sort the array
  sortArray(start, end);

  if(log){
    // 2 numbers that exists in the array
    std::cout << "Is " << mm->get(start) << " in the array according to linear search : " << linearSearch(start, end, mm->get(start)) << "\n";
    std::cout << "Is " << mm->get(start) << " in the array according to binary search : " << binarySearch(start, end, mm->get(start)) << "\n";
    std::cout << "Is " << mm->get(end) << " in the array according to linear search : " << linearSearch(start, end, mm->get(end)) << "\n";
    std::cout << "Is " << mm->get(end) << " in the array according to binary search : " << binarySearch(start, end, mm->get(end)) << "\n";

    // 3 numbers that doesnt exist in the array
    std::cout << "Is -1 in the array according to linear search : " << linearSearch(start, end, -1) << "\n";
    std::cout << "Is -1 in the array according to linear search : " << binarySearch(start, end, -1) << "\n";
    std::cout << "Is -2 in the array according to linear search : " << linearSearch(start, end, -2) << "\n";
    std::cout << "Is -2 in the array according to linear search : " << binarySearch(start, end, -2) << "\n";
    std::cout << "Is -3 in the array according to linear search : " << linearSearch(start, end, -3) << "\n";
    std::cout << "Is -3 in the array according to linear search : " << linearSearch(start, end, -3) << "\n";
  }
}

void saveFile(char* name, int virtualPageSize){
  int num;
  int fd = open(name, O_RDONLY);
  int fd2 = open("test.txt", O_WRONLY | O_CREAT, 0666);
  char buffer[256];

  for(int i=0; i<1024*virtualPageSize; i++){
    read(fd, &num, sizeof(int));
    sprintf(buffer, "i : %d, num : %d\n", i, num);
    write(fd2, buffer, strlen(buffer));
  }

  close(fd);
  close(fd2);
}
