#include <iostream>
#include <string.h>
#include <ctime>

#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H

// #define PAGE_SIZE 4096
#define MAX_LONG 9223372036854775807

using namespace std;

enum PageReplacement {
  LRU,
  WSClock,
  SC
};
class PageTableEntry {
  private:
    int dirty;
    int present;
    long timestamp;
    int pageFrameNumber;
  public:
    static long GLOBAL_COUNTER;
    int getDirty(){return dirty;}
    int getPresent(){return present;}
    int getPageFrameNumber(){return pageFrameNumber;}
    std::time_t getTimestamp(){return timestamp;}
    void setDirty(int dirty){this->dirty = dirty;}
    void setPresent(int present){this->present = present;}
    void setPageFrameNumber(int pageFrameNumber){this->pageFrameNumber = pageFrameNumber;}
    void setTimestamp(std::time_t timestamp){this->timestamp = timestamp;}
    PageTableEntry();
    PageTableEntry(int pageFrameNumber);
};

class PageTable {
  private:
    int size;
    PageTableEntry* table;
  public:
    PageTable();
    PageTable(int size);
    ~PageTable();
    PageTableEntry getPageTableEntry(int index){return this->table[index];}
    void setPageTableEntry(int index, PageTableEntry pte){this->table[index] = pte;}
};

// virtual page is an entry in the virtual memory
// page frame is an entry in the physical memory
class MemoryManagement {
  private:
    PageTable pageTable;
    int virtualPageCount;
    int pageFrameCount;
    int* physicalMemory;
    int interval;
    std::string filename;
    void generateMemory();
    void mapFileToMemory(int virtualPage, int pageFrame);
    void mapMemoryToFile(int virtualPage, int pageFrame);
    PageReplacement replacementAlgorithm;
    int THRESHOLD = 1;
    int PAGE_SIZE;
  public:
    static long Number_Of_Reads;
    static long Number_Of_Writes;
    static long Number_Of_Page_Miss;
    static long Number_Of_Page_Replacements;
    static long Number_Of_Disk_Page_Writes;
    static long Number_Of_Disk_Page_Reads;
    static long Number_Of_Memory_Access;
    MemoryManagement();
    MemoryManagement(int virtulPageCount, int pageFrameCount, std::string filename, PageReplacement replacementAlgorithm, int pageSize, int interval);
    ~MemoryManagement();
    void showPageTable();
    void forceDiskWrite();
    int get(int virtualPage);
    void set(int virtualPage, int num);
};



int linearSearch(int start, int end, int target);
int binarySearch(int start, int end, int target);
void sortArray(int start, int end);
void multiplicationOne(int log);
void multiplicationTwo(int log);
void arraySummation(int log, int startOne, int endOne, int startTwo, int endTwo);
void *testSearchAlgorithms(int log, int start, int end);
void saveFile(char* name, int virtualPageSize);

void matrixMultiplication(int log, int startOne, int xOne, int yOne, int startTwo, int xTwo, int yTwo, int save);


#endif
