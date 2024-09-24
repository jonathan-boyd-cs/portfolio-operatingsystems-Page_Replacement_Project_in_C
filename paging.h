#ifndef PAGING_H
#define PAGING_H
#include <stdio.h>

#define FIFO 	0
#define LRU 	1
#define SC 	2
#define ESC 	3

//PAGE
struct Page {
    int page_no;
    char RDWR;
    int dirty_bit;
    int visited;
}; typedef struct Page Page;

// PAGE MANAGERS
struct PageManager {
    int scheduler_mode;
    int page_faults;
    int disk_writes;
    int requests;
    int size;
    int capcity;
    Page ** FRAMES;
}; typedef struct PageManager PageManager;

struct FIFOPageManager {
	PageManager * manager;
	int front;
}; typedef struct FIFOPageManager FIFOPageManager;

struct LRUPageManager {
	PageManager * manager;
	int LR;
	int MR;
}; typedef struct LRUPageManager LRUPageManager;

Page * create_Page( const int page_no , const char RDWR );
int pageManager_isFull(const PageManager * const  pageManager);
int pageManager_isEmpty( const PageManager * const pageManager);
void pageManager_touchRDWR(PageManager * const pageManager, const int page_index, const char RDWR);
int pageManager_hasPage(const PageManager * const pageManager, const int page_no);
void pageManager_swapPages(PageManager * const pageManager, const int page1, const int page2);
void pageManager_report(const PageManager * const pageManager);

// FIFO
FIFOPageManager* pageManager_createFIFOManager(const int capacity);
void fifo_add(FIFOPageManager * const fifo_PageManager, const int page_no, const char RDWR);
//LRU
LRUPageManager * pageManager_createLRUManager(const int capacity);
void lru_add(LRUPageManager * const lru_PageManager, const int page_no, const char RDWR);
//SECOND CHANCE
PageManager * pageManager_createSCManager(const int capacity);
void sc_add(PageManager * const scManager, const int page_no, const char mode);
//ENHANCED SECOND CHANCE
PageManager * pageManager_createESCManager(const int capactiy);
int esc_add(PageManager * const escManager, const  int page_no, const char RDWR);
//ACCESSORY
void print_usage();
int parse_argument(const int argc, char *const *  argv,
	       	FILE ** fp,int * const mode, int * const size );

#endif //PAGING_H_
