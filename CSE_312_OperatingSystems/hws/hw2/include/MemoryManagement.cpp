#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <ctime>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "MemoryManagement.h"

using namespace std;

// ================ PageTableEntry ================ //
long PageTableEntry::GLOBAL_COUNTER = 0;
PageTableEntry::PageTableEntry(int pageFrameNumber){
  this->dirty = 0;
  this->present = 0;
  this->timestamp = PageTableEntry::GLOBAL_COUNTER++;
  this->pageFrameNumber = pageFrameNumber;
  // printf("%d, %d, %d\n", this->dirty, this->present, this->pageFrameNumber);
}

PageTableEntry::PageTableEntry(){
  this->dirty = 0;
  this->present = 0;
  this->timestamp = PageTableEntry::GLOBAL_COUNTER++;
  this->pageFrameNumber = -1;
  // printf("%d, %d, %d\n", this->dirty, this->present, this->pageFrameNumber);
}


// ================ PageTable ================ //
PageTable::PageTable() {
  this->size = 16;
  this->table = (PageTableEntry*)malloc(sizeof(PageTableEntry)*this->size);
  for(int i=0; i<this->size; i++){
    this->table[i] = PageTableEntry();
  }
}

PageTable::PageTable(int size){
  this->size = size;
  this->table = (PageTableEntry*)malloc(sizeof(PageTableEntry)*size);
  for(int i=0; i<size; i++){
    // printf("test...\n");
    this->table[i] = PageTableEntry();
  }
}

PageTable::~PageTable(){
  // free(this->table);
}

// ================ MemoryManagement ================ //
long MemoryManagement::Number_Of_Page_Miss = 0;
long MemoryManagement::Number_Of_Page_Replacements = 0;
long MemoryManagement::Number_Of_Disk_Page_Writes = 0;
long MemoryManagement::Number_Of_Disk_Page_Reads = 0;
long MemoryManagement::Number_Of_Memory_Access = 0;
long MemoryManagement::Number_Of_Reads = 0;
long MemoryManagement::Number_Of_Writes = 0;
MemoryManagement::MemoryManagement() {
  printf("\b");
  this->virtualPageCount = 16;
  this->pageFrameCount = 8;
  this->filename = "MyDisk.bin";
  this->pageTable = PageTable(virtualPageCount);
  this->replacementAlgorithm = LRU;
  this->PAGE_SIZE = 4096;
  this->interval = 0;
  for(int i=0; i<this->pageFrameCount; i++){
    this->pageTable.getPageTableEntry(i).setPresent(1);
    this->pageTable.getPageTableEntry(i).setPageFrameNumber(i);
    // printf("----> i : %d", i);
  }
  this->generateMemory();
}

MemoryManagement::MemoryManagement(int virtulPageCount, int pageFrameCount, string filename, PageReplacement replacementAlgorithm, int pageSize, int interval){
  printf("\b");
  this->virtualPageCount = virtulPageCount;
  this->pageFrameCount = pageFrameCount;
  this->filename = filename;
  this->pageTable = PageTable(virtualPageCount);
  this->replacementAlgorithm = replacementAlgorithm;
  this->PAGE_SIZE = pageSize;
  this->interval = interval;
  PageTableEntry entry;
  for(int i=0; i<this->pageFrameCount; i++){
    entry.setPresent(1);
    entry.setPageFrameNumber(i);
    this->pageTable.setPageTableEntry(i, entry);
    // printf("----> i : %d\n", i);
  }
  for(int i=0; i<this->virtualPageCount; i++){
    // printf("for i=%d, pageFrameNumber : %d, present : %d, dirty : %d\n", i, this->pageTable.getPageTableEntry(i).getPageFrameNumber(), this->pageTable.getPageTableEntry(i).getPresent(), this->pageTable.getPageTableEntry(i).getDirty());
  }
  // printf("present : %d\n", this->pageTable.getPageTableEntry(1).getPresent());
  
  this->generateMemory();
}

void MemoryManagement::generateMemory(){
  const int length = this->filename.length();
  char* char_array = new char[length + 1];
  strcpy(char_array, this->filename.c_str());
  int fd = open(char_array, O_WRONLY | O_CREAT, 0644);

  srand (time(NULL));
  int iSecret;

  // std::cout << "\npage size : " << this->PAGE_SIZE << "\n";
  // std::cout << "\npageFrameCount : " << this->pageFrameCount << "\n";

  this->physicalMemory = (int*)malloc(this->PAGE_SIZE * this->pageFrameCount * 1000);
  // printf("pageFrameCount : %d\n", this->pageFrameCount);

  // std::cout << "size : " << (this->PAGE_SIZE / sizeof(int)) * this->pageFrameCount << "\n";

  // printf("limit : %ld\n", (this->PAGE_SIZE / sizeof(int)) * this->pageFrameCount);
  printf("generating physical memory, this may take a while...\n");
  for(int i=0; i<this->virtualPageCount * (this->PAGE_SIZE / sizeof(int)); i++){
    iSecret = rand() % 100 + 1;
    if(i < (this->PAGE_SIZE / sizeof(int)) * this->pageFrameCount){
      // std::cout << "start : " << i << "\n";
      physicalMemory[i] = iSecret;
      // physicalMemory[i] = i*10;
      // std::cout << "end\n";
    }
    // std::cout << "i : "<< i << ", iSecret : " << iSecret <<  " \n";
    // sizeof int is 4 byte
    write(fd, &iSecret, sizeof(int));
  }
  printf("physical memory succesfully generated...\n");
  close(fd);

}

int MemoryManagement::get(int virtualPage){
  unsigned int n = (virtualPage*sizeof(int)) / this->PAGE_SIZE;
  unsigned int offSet = (virtualPage*sizeof(int)) - this->PAGE_SIZE * n;
  int pageFrameNumber;

  MemoryManagement::Number_Of_Reads++;
  MemoryManagement::Number_Of_Memory_Access++;
  if(interval !=0 && MemoryManagement::Number_Of_Memory_Access % interval == 0){
    this->showPageTable();
  }

  // printf("n : %d, offSet : %d\n", n, offSet);
  
  if(this->pageTable.getPageTableEntry(n).getPresent()){
    // printf("hello world\n");
    pageFrameNumber = this->pageTable.getPageTableEntry(n).getPageFrameNumber();
    // printf("pagframenumber : %d\n", pageFrameNumber);
    for(int i=0; i<100; i++){
      // printf("%d ", this->physicalMemory[i]);
    }

    // update timestamp in current page table entry
    PageTableEntry currentEntry = this->pageTable.getPageTableEntry(n);
    currentEntry.setTimestamp(PageTableEntry::GLOBAL_COUNTER++);
    this->pageTable.setPageTableEntry(n, currentEntry);

    // printf("index : %ld\n", (pageFrameNumber * this->PAGE_SIZE) / sizeof(int) + offSet+sizeof(int));
    return this->physicalMemory[(pageFrameNumber * this->PAGE_SIZE) / sizeof(int) + offSet/sizeof(int)];
  }else{
    // page miss
    MemoryManagement::Number_Of_Page_Miss++;
    MemoryManagement::Number_Of_Page_Replacements++;

    // printf("replacement algorithm : %d\n", this->replacementAlgorithm);
    // printf("equal to LRU : %d\n", this->replacementAlgorithm == LRU);

    if(this->replacementAlgorithm == LRU){
      // execute LRU in this case
      // find the least recently used page
      long oldestTime = MAX_LONG;
      int oldestTimeIndex = -1;
      long currentTime;

      // printf("hello world2\n");

      for (int i = 0; i < this->virtualPageCount; i++) {
        currentTime = this->pageTable.getPageTableEntry(i).getTimestamp();
        if(currentTime < oldestTime && this->pageTable.getPageTableEntry(i).getPresent() == 1){
          oldestTime = currentTime;
          oldestTimeIndex = i;
        }
      }

      // printf("oldestTimeIndex : %d\n", oldestTimeIndex);

      PageTableEntry lruEntry = this->pageTable.getPageTableEntry(oldestTimeIndex);
      PageTableEntry currentEntry = this->pageTable.getPageTableEntry(n);

      // if LRU page is not dirty
      // printf("isdirty : %d\n", lruEntry.getDirty());

      if(lruEntry.getDirty() == 1){
        // eğer replace edilecek page dirty ise, memory'den dosyaya geri yaz
        this->mapMemoryToFile(oldestTimeIndex, lruEntry.getPageFrameNumber());
      }

      lruEntry.setPresent(0);
      lruEntry.setDirty(false);
      currentEntry.setPresent(1);
      // printf("test test...\n");
      currentEntry.setPageFrameNumber(
        lruEntry.getPageFrameNumber()
      );
      currentEntry.setTimestamp(PageTableEntry::GLOBAL_COUNTER++);
      
      this->pageTable.setPageTableEntry(oldestTimeIndex, lruEntry);
      this->pageTable.setPageTableEntry(n, currentEntry);

      // printf("test test...\n");

      // printf("0 -> %d\n", this->physicalMemory[0]);
      // printf("1 -> %d\n", this->physicalMemory[1]);
      // printf("2 -> %d\n", this->physicalMemory[2]);
      // printf("3 -> %d\n", this->physicalMemory[3]);

      for(int i=0; i<this->virtualPageCount; i++){
        // printf("for i=%d, pageFrameNumber : %d, present : %d, dirty : %d\n", i, this->pageTable.getPageTableEntry(i).getPageFrameNumber(), this->pageTable.getPageTableEntry(i).getPresent(), this->pageTable.getPageTableEntry(i).getDirty());
      }

      this->mapFileToMemory(
        n, currentEntry.getPageFrameNumber()
      );

      // printf("0 -> %d\n", this->physicalMemory[0]);
      // printf("1 -> %d\n", this->physicalMemory[1]);
      // printf("2 -> %d\n", this->physicalMemory[2]);
      // printf("3 -> %d\n", this->physicalMemory[3]);

      // printf("pageFrameNumber : %d\n", lruEntry.getPageFrameNumber());
      // printf("index : %ld\n", (lruEntry.getPageFrameNumber() * this->PAGE_SIZE) / sizeof(int) + offSet/sizeof(int));
      return this->physicalMemory[(lruEntry.getPageFrameNumber() * this->PAGE_SIZE) / sizeof(int) + offSet/sizeof(int)];
    }else if(this->replacementAlgorithm == WSClock) {
      // printf("it is wsclock before\n");
      // this->showPageTable();
      int index = 0;
      bool looped = false;
      while(1){
        if(
          this->pageTable.getPageTableEntry(index).getDirty() == 0 &&
          this->pageTable.getPageTableEntry(index).getTimestamp() - PageTableEntry::GLOBAL_COUNTER >= this->THRESHOLD
        ){
          PageTableEntry oldPage = this->pageTable.getPageTableEntry(index);
          PageTableEntry newPage = this->pageTable.getPageTableEntry(n);
          oldPage.setPresent(0);
          newPage.setPresent(1);
          newPage.setPageFrameNumber(oldPage.getPageFrameNumber());
          newPage.setTimestamp(PageTableEntry::GLOBAL_COUNTER++);
          this->pageTable.setPageTableEntry(index, oldPage);
          this->pageTable.setPageTableEntry(n, newPage);
          this->mapFileToMemory(
            n, newPage.getPageFrameNumber()
          );
          // printf("ispresent : %d\n", oldPage.getPresent());
          // printf("it is wsclock after\n");
          this->showPageTable();
          return this->physicalMemory[(oldPage.getPageFrameNumber() * this->PAGE_SIZE) / sizeof(int) + offSet/sizeof(int)];
        }else if(
          this->pageTable.getPageTableEntry(index).getDirty() == 1
        ){
          // printf("aaaa\n");
          PageTableEntry oldPage = this->pageTable.getPageTableEntry(index);
          this->mapMemoryToFile(index, oldPage.getPageFrameNumber());
          oldPage.setDirty(0);
          this->pageTable.setPageTableEntry(index, oldPage);
        }else if(looped){
          PageTableEntry oldPage = this->pageTable.getPageTableEntry(index);
          PageTableEntry newPage = this->pageTable.getPageTableEntry(n);
          oldPage.setPresent(0);
          newPage.setPresent(1);
          newPage.setPageFrameNumber(oldPage.getPageFrameNumber());
          newPage.setTimestamp(PageTableEntry::GLOBAL_COUNTER++);
          this->pageTable.setPageTableEntry(index, oldPage);
          this->pageTable.setPageTableEntry(n, newPage);
          this->mapFileToMemory(
            n, newPage.getPageFrameNumber()
          );
          // printf("ispresent : %d\n", oldPage.getPresent());
          // printf("it is wsclock after\n");
          this->showPageTable();
          return this->physicalMemory[(oldPage.getPageFrameNumber() * this->PAGE_SIZE) / sizeof(int) + offSet/sizeof(int)];
        }
        index++;
        if(index == this->virtualPageCount){
          looped = true;
          index %= this->virtualPageCount;
        }
      }
    }else if(this->replacementAlgorithm == SC) {
      while(1){
        long oldestTime = MAX_LONG;
        int oldestTimeIndex = -1;
        long currentTime;

        for (int i = 0; i < this->virtualPageCount; i++) {
          currentTime = this->pageTable.getPageTableEntry(i).getTimestamp();
          if(currentTime < oldestTime && this->pageTable.getPageTableEntry(i).getPresent() == 1){
            oldestTime = currentTime;
            oldestTimeIndex = i;
          }
        }

        if(this->pageTable.getPageTableEntry(oldestTimeIndex).getDirty() == 1){
          PageTableEntry oldPage = this->pageTable.getPageTableEntry(oldestTimeIndex);
          this->mapMemoryToFile(oldestTimeIndex, oldPage.getPageFrameNumber());
          oldPage.setTimestamp(PageTableEntry::GLOBAL_COUNTER++);
          oldPage.setDirty(0);
          this->pageTable.setPageTableEntry(oldestTimeIndex, oldPage);
        }else if(this->pageTable.getPageTableEntry(oldestTimeIndex).getDirty() == 0){
          PageTableEntry oldPage = this->pageTable.getPageTableEntry(oldestTimeIndex);
          PageTableEntry newPage = this->pageTable.getPageTableEntry(n);
          oldPage.setPresent(0);
          newPage.setPresent(1);
          newPage.setPageFrameNumber(oldPage.getPageFrameNumber());
          newPage.setTimestamp(PageTableEntry::GLOBAL_COUNTER++);
          this->pageTable.setPageTableEntry(oldestTimeIndex, oldPage);
          this->pageTable.setPageTableEntry(n, newPage);
          this->mapFileToMemory(n, newPage.getPageFrameNumber());
          // this->showPageTable();
          return this->physicalMemory[(oldPage.getPageFrameNumber() * this->PAGE_SIZE) / sizeof(int) + offSet/sizeof(int)];;
        }
      }
    }

  }
}

void MemoryManagement::set(int virtualPage, int num){
  unsigned int n = (virtualPage*sizeof(int)) / this->PAGE_SIZE;
  unsigned int offSet = (virtualPage*sizeof(int)) - this->PAGE_SIZE * n;
  int pageFrameNumber;

  // printf("inside set...\n");
  MemoryManagement::Number_Of_Writes++;
  MemoryManagement::Number_Of_Memory_Access++;
  if(interval !=0 && MemoryManagement::Number_Of_Memory_Access % interval == 0){
    this->showPageTable();
  }


  for(int i=0; i<this->virtualPageCount; i++){
    // printf("for i=%d, pageFrameNumber : %d, present : %d, dirty : %d\n", i, this->pageTable.getPageTableEntry(i).getPageFrameNumber(), this->pageTable.getPageTableEntry(i).getPresent(), this->pageTable.getPageTableEntry(i).getDirty());
  }

  // printf("n : %d, offSet : %d\n", n, offSet);
  // printf("%d\n", this->pageTable.getPageTableEntry(n).getPresent());
  if(this->pageTable.getPageTableEntry(n).getPresent()){
    // printf("page hit...\n");
    pageFrameNumber = this->pageTable.getPageTableEntry(n).getPageFrameNumber();
    physicalMemory[(pageFrameNumber * this->PAGE_SIZE) / sizeof(int) + offSet/sizeof(int)] = num;
    PageTableEntry currentEntry = this->pageTable.getPageTableEntry(n);
    currentEntry.setTimestamp(PageTableEntry::GLOBAL_COUNTER++);
    currentEntry.setDirty(1);
    this->pageTable.setPageTableEntry(n, currentEntry);
    // printf("isdirty....%d\n", this->pageTable.getPageTableEntry(n).getDirty());
    return;
  }else{
    long oldestTime = MAX_LONG;
    int oldestTimeIndex = -1;
    long currentTime;

    // printf("timenow : %ld\n", oldestTime);

    // // printf("hello world2\n");

    for (int i = 0; i < this->virtualPageCount; i++) {
      currentTime = this->pageTable.getPageTableEntry(i).getTimestamp();
      // printf("currentTime : %ld, oldestTime : %ld\n", currentTime, oldestTime);
      if(currentTime < oldestTime && this->pageTable.getPageTableEntry(i).getPresent() == 1){
        oldestTime = currentTime;
        oldestTimeIndex = i;
      }
    }

    // TODO: oldestTimeIndex == -1 olma durumu kritik
    // printf("oldestTimeIndex : %d\n", oldestTimeIndex);

    PageTableEntry lruEntry = this->pageTable.getPageTableEntry(oldestTimeIndex);
    PageTableEntry currentEntry = this->pageTable.getPageTableEntry(n);

    // if LRU page is not dirty
    // printf("isdirty : %d\n", lruEntry.getDirty());
    if(lruEntry.getDirty()){
      // eğer replace edilecek page dirty ise, memory'den dosyaya geri yaz
      this->mapMemoryToFile(oldestTimeIndex, lruEntry.getPageFrameNumber());
    }

    if(this->replacementAlgorithm == LRU){
      lruEntry.setPresent(0);
      lruEntry.setDirty(false);
      currentEntry.setPresent(1);
      // printf("test test...\n");
      currentEntry.setPageFrameNumber(
        lruEntry.getPageFrameNumber()
      );
      currentEntry.setDirty(1);
      
      this->pageTable.setPageTableEntry(oldestTimeIndex, lruEntry);
      this->pageTable.setPageTableEntry(n, currentEntry);

      // printf("test test...\n");

      // printf("0 -> %d\n", this->physicalMemory[0]);
      // printf("1 -> %d\n", this->physicalMemory[1]);
      // printf("2 -> %d\n", this->physicalMemory[2]);
      // printf("3 -> %d\n", this->physicalMemory[3]);

      for(int i=0; i<this->virtualPageCount; i++){
        // printf("for i=%d, pageFrameNumber : %d, present : %d, dirty : %d\n", i, this->pageTable.getPageTableEntry(i).getPageFrameNumber(), this->pageTable.getPageTableEntry(i).getPresent(), this->pageTable.getPageTableEntry(i).getDirty());
      }

      this->mapFileToMemory(
        n, currentEntry.getPageFrameNumber()
      );

      // printf("0 -> %d\n", this->physicalMemory[0]);
      // printf("1 -> %d\n", this->physicalMemory[1]);
      // printf("2 -> %d\n", this->physicalMemory[2]);
      // printf("3 -> %d\n", this->physicalMemory[3]);

      // printf("pageFrameNumber : %d\n", lruEntry.getPageFrameNumber());
      // printf("index : %ld\n", (lruEntry.getPageFrameNumber() * this->PAGE_SIZE) / sizeof(int) + offSet/sizeof(int));
      this->physicalMemory[(lruEntry.getPageFrameNumber() * this->PAGE_SIZE) / sizeof(int) + offSet/sizeof(int)] = num;
      return;
    }else if(this->replacementAlgorithm == WSClock) {
      printf("it is wsclock\n");
      int index = 0;
      bool looped = false;
      while(1){
        if(
          this->pageTable.getPageTableEntry(index).getDirty() == 0 &&
          this->pageTable.getPageTableEntry(index).getTimestamp() - PageTableEntry::GLOBAL_COUNTER >= this->THRESHOLD
        ){
          PageTableEntry oldPage = this->pageTable.getPageTableEntry(index);
          PageTableEntry newPage = this->pageTable.getPageTableEntry(n);
          oldPage.setPresent(0);
          newPage.setPresent(1);
          newPage.setDirty(1);
          newPage.setPageFrameNumber(oldPage.getPageFrameNumber());
          newPage.setTimestamp(PageTableEntry::GLOBAL_COUNTER++);
          this->pageTable.setPageTableEntry(index, oldPage);
          this->pageTable.setPageTableEntry(n, newPage);
          this->mapFileToMemory(
            n, newPage.getPageFrameNumber()
          );
          this->physicalMemory[(oldPage.getPageFrameNumber() * this->PAGE_SIZE) / sizeof(int) + offSet/sizeof(int)] = num;
          return;
        }else if(
          this->pageTable.getPageTableEntry(index).getDirty() == 1
        ){
          PageTableEntry oldPage = this->pageTable.getPageTableEntry(index);
          this->mapMemoryToFile(index, oldPage.getPageFrameNumber());
          oldPage.setDirty(0);
          this->pageTable.setPageTableEntry(index, oldPage);
        }else if(looped){
          PageTableEntry oldPage = this->pageTable.getPageTableEntry(index);
          PageTableEntry newPage = this->pageTable.getPageTableEntry(n);
          oldPage.setPresent(0);
          newPage.setPresent(1);
          newPage.setDirty(1);
          newPage.setPageFrameNumber(oldPage.getPageFrameNumber());
          newPage.setTimestamp(PageTableEntry::GLOBAL_COUNTER++);
          this->pageTable.setPageTableEntry(index, oldPage);
          this->pageTable.setPageTableEntry(n, newPage);
          this->mapFileToMemory(
            n, newPage.getPageFrameNumber()
          );
          this->physicalMemory[(oldPage.getPageFrameNumber() * this->PAGE_SIZE) / sizeof(int) + offSet/sizeof(int)] = num;
          return;
        }
        index++;
        if(index == this->virtualPageCount){
          looped = true;
          index %= this->virtualPageCount;
        }
      }
    }else if(this->replacementAlgorithm == SC) {
      while(1){
        long oldestTime = MAX_LONG;
        int oldestTimeIndex = -1;
        long currentTime;

        for (int i = 0; i < this->virtualPageCount; i++) {
          currentTime = this->pageTable.getPageTableEntry(i).getTimestamp();
          if(currentTime < oldestTime && this->pageTable.getPageTableEntry(i).getPresent() == 1){
            oldestTime = currentTime;
            oldestTimeIndex = i;
          }
        }
        // printf("test.. : %d\n", oldestTimeIndex);
        // this->showPageTable();

        if(this->pageTable.getPageTableEntry(oldestTimeIndex).getDirty() == 1){
          PageTableEntry oldPage = this->pageTable.getPageTableEntry(oldestTimeIndex);
          this->mapMemoryToFile(oldestTimeIndex, oldPage.getPageFrameNumber());
          oldPage.setTimestamp(PageTableEntry::GLOBAL_COUNTER++);
          oldPage.setDirty(0);
          this->pageTable.setPageTableEntry(oldestTimeIndex, oldPage);
        }else if(this->pageTable.getPageTableEntry(oldestTimeIndex).getDirty() == 0){
          PageTableEntry oldPage = this->pageTable.getPageTableEntry(oldestTimeIndex);
          PageTableEntry newPage = this->pageTable.getPageTableEntry(n);
          oldPage.setPresent(0);
          newPage.setPresent(1);
          newPage.setDirty(1);
          newPage.setPageFrameNumber(oldPage.getPageFrameNumber());
          newPage.setTimestamp(PageTableEntry::GLOBAL_COUNTER++);
          this->pageTable.setPageTableEntry(oldestTimeIndex, oldPage);
          this->pageTable.setPageTableEntry(n, newPage);
          this->mapFileToMemory(n, newPage.getPageFrameNumber());
          // this->showPageTable();
          this->physicalMemory[(oldPage.getPageFrameNumber() * this->PAGE_SIZE) / sizeof(int) + offSet/sizeof(int)] = num;
          return;
        }
      }
    }


  }
} 

void MemoryManagement::mapMemoryToFile(int virtualPage, int pageFrame){
  MemoryManagement::Number_Of_Disk_Page_Writes++;
  // printf("============ DEBUG\n");
  const int length = this->filename.length();
  const char* char_array = this->filename.c_str();

  int newValue;
  int counter = 0;
  int fileDescriptor = open(char_array, O_RDWR);

  if (fileDescriptor != -1) {
    // Move the cursor to the specified position
    off_t offset = lseek(fileDescriptor, virtualPage * sizeof(int) * 1024, SEEK_SET);

    if (offset != -1) {
      // Change the integer at the specified position
      for(int i=0; i<this->PAGE_SIZE / sizeof(int); i++){
        ssize_t bytesRead;
        int currentValue;
        bytesRead = read(fileDescriptor, &currentValue, sizeof(int));

        newValue = this->physicalMemory[pageFrame * 1024 + counter++];

        if (bytesRead == sizeof(int)) {
          lseek(fileDescriptor, -sizeof(int), SEEK_CUR);
          write(fileDescriptor, &newValue, sizeof(int));
        } else {
          // printf("Failed to read the current integer value in file: %s\n", char_array);
        }
      }
    } else {
        // printf("Failed to seek to the specified position in file: %s\n", char_array);
    }

    close(fileDescriptor);
  } else {
    // printf("Failed to open the file: %s\n", char_array);
  }

}

void MemoryManagement::mapFileToMemory(int virtualPage, int pageFrame){
  MemoryManagement::Number_Of_Disk_Page_Reads++;
  // printf("hellooooo\n");
  const int length = this->filename.length();
  const char* char_array = this->filename.c_str();
  // char* char_array = new char[length + 1];
  // printf("helloooo2\n");
  // strcpy(char_array, this->filename.c_str());
  // printf("%s\n", char_array);
  int fd = open(char_array, O_RDONLY, 0644);

  // printf("virtualPage : %d, pageFrame : %d\n", virtualPage, pageFrame);

  int fileDescriptor = open(char_array, O_RDWR);
  int phyMemoryIndex = pageFrame * 1024;

  // printf("helloooo3\n");
  int tmp;

  if (fileDescriptor != -1) {
    // Move the cursor to the specified position
    off_t offset = lseek(fileDescriptor, virtualPage * sizeof(int) * 1024, SEEK_SET);

    if (offset != -1) {
      for(int i=0; i<(this->PAGE_SIZE/sizeof(int)); i++){
        read(fileDescriptor, &tmp, sizeof(int));
        this->physicalMemory[phyMemoryIndex] = tmp;
        // // printf("%d ... %d\n", phyMemoryIndex, this->physicalMemory[phyMemoryIndex]);
        phyMemoryIndex++;
      }
    } else {
      // printf("Failed to seek to the specified position in file: %s\n", char_array);
    }

    close(fileDescriptor);
  } else {
    // printf("Failed to open the file: %s\n", char_array);
  }


}

void MemoryManagement::showPageTable() {
  printf("============ PAGE TABLE ============\n");
  for(int i=0; i<this->virtualPageCount; i++){
    printf("for i=%d, pageFrameNumber : %d, present : %d, dirty : %d, timestamp : %ld\n", i, this->pageTable.getPageTableEntry(i).getPageFrameNumber(), this->pageTable.getPageTableEntry(i).getPresent(), this->pageTable.getPageTableEntry(i).getDirty(), this->pageTable.getPageTableEntry(i).getTimestamp());
  }
  printf("====================================\n");
}

void MemoryManagement::forceDiskWrite() {
  for(int i=0; i<this->virtualPageCount; i++){
    if(this->pageTable.getPageTableEntry(i).getDirty() == 1){
      this->mapMemoryToFile(i, this->pageTable.getPageTableEntry(i).getPageFrameNumber());
      PageTableEntry currentPage = this->pageTable.getPageTableEntry(i);
      currentPage.setDirty(0);
      this->pageTable.setPageTableEntry(i, currentPage);
    }
  }
}

MemoryManagement::~MemoryManagement(){
  // // write dirty pages to disk before exiting
  // // this->showPageTable();

  // for(int i=0; i<this->virtualPageCount; i++){
  //   if(this->pageTable.getPageTableEntry(i).getDirty() == 1){
  //     std::cout << "save...\n";
  //     this->mapMemoryToFile(i, this->pageTable.getPageTableEntry(i).getPageFrameNumber());
  //   }
  // }


  // std::cout << "bye..\n";
}
