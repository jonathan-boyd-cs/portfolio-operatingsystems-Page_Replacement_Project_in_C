#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include "paging.h"
#include <getopt.h>
#include <assert.h>
// GENERAL PAGE MANAGER FUNCTIONS

Page * create_Page( const int page_no , const char RDWR ) {

	Page * requested_page = ( Page* )malloc( sizeof(Page) );
    // assert malloc not null
	assert(requested_page != NULL);
        
    requested_page->page_no = page_no;
    requested_page->RDWR = RDWR;

	return requested_page;
}

int pageManager_isFull( const PageManager * const  pageManager ){
	/*
	 * Method determines if a PageManager instance is currently full.
	 *
	 * 	Parameters...
	 * 		-> PageManager object 	: PageManager to query
	 * 	Returns...
	 * 		-> Int					: Boolean (1) if full, else (0)
	 */
    return pageManager->size == pageManager->capcity;
}

int pageManager_isEmpty( const PageManager * const pageManager ){
	/*
	 * Method determines if a PageManager instance is currently empty.
	 *
	 * 	Parameters...
	 * 		-> PageManager object 	: PageManager to query
	 * 	Returns...
	 * 		-> Int					: Boolean (1) if empty, else (0)
	 */
    return pageManager->size == 0;
}

int pageManager_hasPage( const PageManager * const pageManager, const int page_no ) {
    /*
     * Method determines if a provided PageManager instance contains a specified
     * page.
	 *
	 * 	Parameters...
	 * 		-> PageManager object	: PageManager to query
	 * 		-> int page_no			: the page to search for in the manager
	 *	Returns...
	 *		-> Int					: the index of the page if found, else (-1) 
     * */
    for (int i = 0; i < pageManager->size; i++){
        if (pageManager->FRAMES[i]->page_no == page_no){
			// page found
            return i;
        }
    }
    // page not found
    return -1;
}

void pageManager_touchRDWR( PageManager * const pageManager, const int page_index, const char RDWR ) {
	/*
	 * Method will convert the RDWR bit of a specified page is they differ.
	 * Marks the dirty bit if write is set.
	 *
	 * 	Parameters...
	 * 		-> PageManager object	: PageManager to query
	 * 		-> int page_index		: the index of the page in the PageManager->FRAME field
	 * 		-> char RDWR			: the specified RDWR mode to check against. values ('w' or 'r')
	 * 	Returns...
	 * 		None
	 * */
	if (pageManager->FRAMES[page_index]->RDWR != RDWR) { // change from read or write ?
	    pageManager->FRAMES[page_index]->RDWR = RDWR;
	    if (RDWR == 'w' ) {
            	pageManager->FRAMES[page_index]->dirty_bit = 1;
		}
    	}
}

void pageManager_swapPages(PageManager * const pageManager, const int page1, const int page2) {
/*
 * Method swaps the positions of two pages in a PageManager objects FRAMES field.
 *
 * 	Parameters...
 * 		-> PageManager object		: PageManager to manipulate
 *		-> int page1 , page2		: The pages to swap
 *	Returns...
 *		None 
 *
 */
    Page *temp = pageManager->FRAMES[page1];
    pageManager->FRAMES[page1] = pageManager->FRAMES[page2];
    pageManager->FRAMES[page2] = temp;
}

void pageManager_report(const PageManager * const pageManager) {
/*
 * Method prints a report of the PageManager's statistics.
 * Returns with error and no output if no requests have been processed 
 * or if an unknown scheduler mode is set in the anager.
 *
 * 	Parameters...
 * 		-> PageManager object	: the manager on which to report
 * 	Returns...
 * 		None
 *
 * */
    if (pageManager->requests == 0){
        perror("attempting zero division\n");
        return;
    }
    char * mode = NULL;
    switch(pageManager->scheduler_mode) {
        case FIFO:
            mode = "FIFO";
            break;
        case LRU:
            mode = "LRU";
            break;
        case SC:
            mode = "Second Chance";
            break;
        case ESC:
            mode = "Enhanced Second Chance";
            break;
        default:
            perror("unknown scheduler\n");
            return;
    }
    double fault_rate = (
							(double) pageManager->page_faults / (double)pageManager->requests
						)   * 100;
    printf( "\n\t%s\nfaults: %d in %d requests\nfault rate: %.2f%%\n", 
					mode,
					pageManager->page_faults, 
					pageManager->requests, 
					fault_rate );
    printf( "test parameter -> frame capacity: %d\n\n",pageManager->capcity );
}


// FIFO FUNCTIONS

FIFOPageManager * pageManager_createFIFOManager(const int capacity) {
/*
 * Method creates and returns a FIFOPageManager instance configured for FIFO
 * scheduling.
 *
 * 	Parameters...
 * 		int capacity	: the designated maximum size of the manager memory
 * 	Returns...
 * 		FIFOPageManager		: FIFO manager instance
 * */
    FIFOPageManager * fifo_PageManager = ( FIFOPageManager * )malloc( sizeof(FIFOPageManager) );
	assert(fifo_PageManager != NULL);


	fifo_PageManager->manager = (PageManager*)malloc(sizeof(PageManager));
    assert(fifo_PageManager->manager != NULL);

	fifo_PageManager->manager->FRAMES = (Page**)malloc(sizeof(Page)*capacity);
	assert(fifo_PageManager->manager->FRAMES != NULL);


	fifo_PageManager->front = 0;
    fifo_PageManager->manager->scheduler_mode = FIFO;
    fifo_PageManager->manager->page_faults = 0;
    fifo_PageManager->manager->capcity = capacity;
    fifo_PageManager->manager->size = 0;
	

    return fifo_PageManager;
}

void fifo_add(FIFOPageManager * const fifo_PageManager, const int page_no, const char RDWR){
/*
 * Method adds a page to a PageManager configured for FIFO page replacement.
 * The function frees a page if necessary according to the FIFO algorithm.
 * The function updates the RDWR bit of a page if already present in memory.
 *
 * 	Parameters...
 *		PageManager object		: a FIFO page manager object
 *		int page_no				: the page number requested
 *		char RDWR				: the access mode of the request ('w' or 'r')
 * 	Returns...
 * 		None
 * */	
	// assert manager not null
	assert(fifo_PageManager != NULL);
	assert(fifo_PageManager->manager != NULL);	
	// assert FIFO
	assert(fifo_PageManager->manager->scheduler_mode == FIFO);
	// assert RDWR valid
	assert( (RDWR =='w') || (RDWR == 'r') );
	
	fifo_PageManager->manager->requests++;
	int page_index;
    // if page not already in memory...
    if ( ( page_index = pageManager_hasPage(fifo_PageManager->manager,page_no) ) < 0){
		fifo_PageManager->manager->page_faults++;
	//  creating new page
        Page * requested_page = create_Page( page_no, RDWR);

        if (!pageManager_isFull(fifo_PageManager->manager)) {
            fifo_PageManager->manager->FRAMES[fifo_PageManager->manager->size++] = requested_page;
        } else {
            //	fifo replacement policy implementation
            free(fifo_PageManager->manager->FRAMES[fifo_PageManager->front]);
            fifo_PageManager->manager->FRAMES[fifo_PageManager->front] = requested_page;
            fifo_PageManager->front = (fifo_PageManager->front + 1) % fifo_PageManager->manager->capcity;
        }
    } else {
		// update existing page's RDWR if necessary.
		pageManager_touchRDWR(fifo_PageManager->manager, page_index, RDWR);
	}
}

// LRU FUNCTIONS

LRUPageManager * pageManager_createLRUManager(const int capacity) {
/*
 * Method creates and returns a LRUPageManager instance configured for LRU
 * scheduling.
 *
 * 	Parameters...
 * 		int capacity	: the designated maximum size of the manager memory
 * 	Returns...
 * 		LRUPageManager		: LRU  manager instance
 * */
	LRUPageManager * lru_PageManager = ( LRUPageManager* )malloc( sizeof(LRUPageManager) );
   	assert(lru_PageManager != NULL);



	lru_PageManager->manager = ( PageManager* )malloc( sizeof(PageManager) );
   	assert(lru_PageManager->manager != NULL);

	lru_PageManager->manager->FRAMES = (Page**)malloc(sizeof(Page)*capacity);
	assert(lru_PageManager->manager->FRAMES != NULL);
	
	lru_PageManager->LR = 0;
    lru_PageManager->MR = 0;
    lru_PageManager->manager->scheduler_mode = LRU;
    lru_PageManager->manager->page_faults = 0;
    lru_PageManager->manager->capcity = capacity;
    lru_PageManager->manager->size = 0;


	return lru_PageManager;
}

void lru_add(LRUPageManager * const lru_PageManager, const int page_no, const char RDWR){
/*
 * Method adds a page to a PageManager configured for LRU page replacement.
 * The function frees a page if necessary according to the LRU algorithm.
 * The function updates the RDWR bit of a page if already present in memory.
 *
 * 	Parameters...
 *		LRUPageManager object		: a FIFO page manager object
 *		int page_no				: the page number requested
 *		char RDWR				: the access mode of the request ('w' or 'r')
 * 	Returns...
 * 		None
 * */	
    // VARIABLE ('index') serves to provide index of a page in the FRAME array, if present

	// assert manager not null
	assert(lru_PageManager != NULL);
	assert(lru_PageManager->manager != NULL);
	// assert LRU
	assert(lru_PageManager->manager->scheduler_mode == LRU);
	// assert valid RDWR
	assert( (RDWR == 'w') || (RDWR == 'r'));

	int index;
    lru_PageManager->manager->requests++;
    
    // if page not already in memory...
    if ((index = pageManager_hasPage(lru_PageManager->manager,page_no)) < 0){
		lru_PageManager->manager->page_faults++;
        // creating a page
        Page * requested_page = create_Page( page_no, RDWR);

        if (!pageManager_isFull(lru_PageManager->manager)){
            // update the pointer to the most recently used page -> to the position of the upcoming entry
            lru_PageManager->MR = lru_PageManager->manager->size;
			// place new page
			lru_PageManager->manager->FRAMES[lru_PageManager->manager->size++] = requested_page;
        } else {
            // lru page replacement implementation
            free(lru_PageManager->manager->FRAMES[lru_PageManager->LR]); // free the least recently used page
            lru_PageManager->manager->FRAMES[lru_PageManager->LR] = requested_page; // add new page
            lru_PageManager->MR = lru_PageManager->LR; // update the most recent pointer to the new page
            lru_PageManager->LR = (lru_PageManager->LR + 1) % lru_PageManager->manager->capcity; // increment the lru to next lru
        }
    } else {
		// update RDWR if necessary
		pageManager_touchRDWR(lru_PageManager->manager, index, RDWR);
        // if the page in question is the current lru
        if (lru_PageManager->LR == index){
            lru_PageManager->LR = (lru_PageManager->LR + 1) % lru_PageManager->manager->capcity; // increment lru
            lru_PageManager->MR = index; // update mru
        }
        // assuming the page in question is not the most recently used...
        if (!(lru_PageManager->MR == index)){
            // swap with most recently used
            pageManager_swapPages( lru_PageManager->manager, lru_PageManager->MR, index );
            int i = index;
			// correct memory order
            while( i < ( ( lru_PageManager->MR-1 ) % lru_PageManager->manager->capcity ) ) {
                int next = ( i + 1 ) % lru_PageManager->manager->capcity;
                pageManager_swapPages( lru_PageManager->manager, i , next );
                i = next;
            }
        }
    }
}


// SECOND CHANCE FUNCTIONS

PageManager * pageManager_createSCManager(const int capacity){
/*
 * Method creates and returns a PageManager instance configured for Second Chance
 * scheduling.
 *
 * 	Parameters...
 * 		int capacity	: the designated maximum size of the manager memory
 * 	Returns...
 * 		PageManager		: Second Chance  manager instance
 * */
    PageManager * scManager = (PageManager*)malloc(sizeof(PageManager));
    scManager->scheduler_mode = SC;
    scManager->page_faults = 0;
    scManager->capcity = capacity;
    scManager->size = 0;
    scManager->FRAMES = (Page**)malloc(sizeof(Page)*capacity);
	// assert malloc not null
	assert(scManager->FRAMES != NULL);

    return scManager;
}

void sc_add(PageManager * const scManager, const int page_no, const char RDWR) {
/*
 * Method adds a page to a PageManager configured for Second Chance page replacement.
 * The function frees a page if necessary according to the Second Chance algorithm.
 * The function updates the RDWR bit of a page if already present in memory.
 *
 * 	Parameters...
 *		PageManager object		: a page manager object
 *		int page_no				: the page number requested
 *		char RDWR				: the access mode of the request ('w' or 'r')
 * 	Returns...
 * 		None
 * */	
	scManager->requests++;
    int index;
    // if page not already in memory...
    if ( (index = pageManager_hasPage( scManager , page_no )) < 0 ) {
		
		scManager->page_faults++;
		// creating new page
        Page * requested_page = create_Page( page_no, RDWR);

        // need to write changes to disk
        if (RDWR == 'w' ) {
            requested_page->dirty_bit = 1;
        }
        // setting second chance reference bit
        requested_page->visited = 1;

        if ( !pageManager_isFull(scManager) ) {
            scManager->FRAMES[scManager->size++] = requested_page;
        } else {
            int i = 0;
            // seek first second chance recipient
            while ( scManager->FRAMES[i]->visited ) {
                scManager->FRAMES[i]->visited = 0; // clear reference bit
                i = (i + 1) % scManager->capcity;
            }
            free(scManager->FRAMES[i]);
            scManager->FRAMES[i] = requested_page;
        }
    } else {
		// update RDWR if necessary on existing page in memory
		pageManager_touchRDWR(scManager , index, RDWR);
        // ensure this recently referenced page is marked as such
        if ( !scManager->FRAMES[index]->visited ){
         scManager->FRAMES[index]->visited = 1;
        }
    }
}

PageManager * pageManager_createESCManager( const int capacity) {
/*
 * Decorator method creates a Enhanced Second Chance specific page scheduler.
 * Utilizes the pageManager_createSCManager function.
 *
 * 	Parameters...
 * 		int capacity	: the designated maximum size of the manager memory
 *
 *	Returns...
 * 		PageManager		: Second Chance  manager instance
 * */
    PageManager * escManager = pageManager_createSCManager(capacity);
    escManager->scheduler_mode = ESC;
    return escManager;
}

int esc_add(PageManager * const escManager, const int page_no, const char RDWR) { //incorporates dirty bit
/*
 * Method adds a page to a PageManager configured for Enhanced Second Chance page replacement.
 * The function frees a page if necessary according to the Enhanced Second Chance algorithm.
 * The function updates the RDWR bit of a page if already present in memory.
 *
 * 	Parameters...
 *		PageManager object		: a page manager object
 *		int page_no				: the page number requested
 *		char RDWR				: the access mode of the request ('w' or 'r')
 * 	Returns...
 * 		None
 * */	
    escManager->requests++;
    int index;
    
    if ((index = pageManager_hasPage(escManager,page_no)) < 0){
		escManager->page_faults++;
		// creating new page
        Page * requested_page = create_Page( page_no, RDWR);
		
        // need to write changes to disk
        if (RDWR == 'w' ) {
            requested_page->dirty_bit = 1;
            escManager->disk_writes++; // chart disk write on page fault
        }

        // setting second chance reference bit
        requested_page->visited = 1;

        if ( !pageManager_isFull(escManager) ){
            escManager->FRAMES[escManager->size++] = requested_page;
        } else {
            int i = 0;
            // seeking page to replace which demonstrates cleared reference bit and finished disk writing 
            while ( escManager->FRAMES[i]->visited || escManager->FRAMES[i]->dirty_bit ) {
            	
				if (escManager->FRAMES[i]->visited) {
                    escManager->FRAMES[i]->visited = 0; // clear reference bit
                    i = (i + 1) % escManager->capcity;
                    continue;
                }
                if ( escManager->FRAMES[i]->dirty_bit ) {
                    escManager->FRAMES[i]->dirty_bit = 0; //simulate write finish
                    i = (i + 1) % escManager->capcity;
                }
               
            }
			// if here ... at page to replace
            free(escManager->FRAMES[i]);
            escManager->FRAMES[i] = requested_page;
        }
    } else {
		// update RDWR if necessary for page in memory
		pageManager_touchRDWR( escManager, index , RDWR);
        if ( !escManager->FRAMES[index]->visited ) {
         escManager->FRAMES[index]->visited = 1;
        }
    }
}


// ACCESSORY FUNCTIONS
void print_usage() {
    perror("argument line usage:\n\t ./a.out -f filename -m mode_of_page_replacement -s number_of_frames\n");
}
int parse_argument(const int argc, char *const * argv, FILE ** fp, int * const mode, int * const size ){
    if (argc < 7){
        print_usage();
        return -1;
    }
    int option;
    int fflag = 0;
    int mflag = 0;
    int sflag = 0;
    while((option = getopt(argc, argv, "f:m:s:")) != -1) { 
        switch (option) {
            case 'f': // PAGE FEED FILE
                if (!fflag){
                    *fp = fopen(optarg,"r");
                    fflag = 1;
                }
                break;
            case 'm': // MODE OF PAGE REPLACEMENT
                if (!mflag){
                    if (strcmp(optarg,"FIFO") == 0){
                        *mode = FIFO;
                    } else if (strcmp(optarg,"LRU") == 0) {
                        *mode = LRU;
                    } else if (strcmp(optarg,"SC") == 0) {
                        *mode = SC;
                    } else if (strcmp(optarg,"ESC") == 0) {
                        *mode = ESC;
                    } else {
                        perror("Incorrect mode in argument line\noptions: 'FIFO', 'LRU', 'SC', 'ESC'\n");
                        print_usage();
                        return -1;
                    }
                    mflag = 1;
                }
                break;
            case 's': // NUMBER OF FRAMES
                if (!sflag){
                    *size = atoi(optarg);
                    sflag = 1;
                }
                break;
            default:
                perror("invalid option\n");
                print_usage();
                return -1;
        }
    }
    return 0;
}

