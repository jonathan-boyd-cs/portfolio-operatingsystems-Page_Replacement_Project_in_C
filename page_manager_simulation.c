#include "paging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
int main(int argc, char ** argv) {
    

    // PARSE ARGUMENT LINE
    int mode;
    int size;
    FILE *fp = NULL;
    if (parse_argument(argc,argv, &fp, &mode, &size) < 0 ){
        perror("error in parsing\n");
        return -1;
    }
	
    //READ PAGE FEED
    size_t MAXLINE = 256;
    char *buffer = (char *)malloc(sizeof(char)*MAXLINE);
	assert(buffer != NULL);
    
    switch (mode){
        case FIFO:;
            FIFOPageManager * fifoManager = pageManager_createFIFOManager(size);
			assert(fifoManager != NULL);
            while ( (getline(&buffer,&MAXLINE,fp)) >= 0 ) { // simulating page requests
                int page_no = -1;
                char rw = '\0';
                sscanf(buffer,"%d %c", &page_no, &rw);
                if ( (page_no == -1) || (rw == '\0') ){
                    fprintf(stderr,"invalid page feed entry @ line: %d\n",fifoManager->manager->requests+1);
                    return -1;
                }
                fifo_add(fifoManager,page_no,rw);
            }
			// result
            pageManager_report(fifoManager->manager);
            break;
            
        case LRU:;
            LRUPageManager * lruManager = pageManager_createLRUManager(size);
			assert(lruManager != NULL);
            while ( (getline(&buffer,&MAXLINE,fp)) >= 0 ) { // simulating page requests
                int page_no = -1;
                char rw = '\0';
                sscanf(buffer,"%d %c", &page_no, &rw);
                if ( (page_no == -1) || (rw == '\0') ){
                    fprintf(stderr,"invalid page feed entry @ line: %d\n",lruManager->manager->requests+1);
                    return -1;
                }
                lru_add(lruManager,page_no,rw);
            }
			// result
            pageManager_report(lruManager->manager);
            break;

        case SC:; //SECOND CHANCE
            PageManager * scManager = pageManager_createSCManager(size);
			assert(scManager != NULL);
            while ( (getline(&buffer,&MAXLINE,fp)) >= 0 ) { // simulating page requests
                int page_no = -1;
                char rw = '\0';
                sscanf(buffer,"%d %c", &page_no, &rw);
                if ( (page_no == -1) || (rw == '\0') ){
                    fprintf(stderr,"invalid page feed entry @ line: %d\n",scManager->requests+1);
                    return -1;
                }
                sc_add(scManager,page_no,rw);
            }
            // result
            pageManager_report(scManager);
            break;

        case ESC:; // ENHANCED SECOND CHANCE
            PageManager * escManager = pageManager_createESCManager(size);
			assert(escManager != NULL);
            while ( (getline(&buffer,&MAXLINE,fp)) >= 0 ) { // simulating page requests
                int page_no = -1;
                char rw = '\0';
                sscanf(buffer,"%d %c", &page_no, &rw);
                if ( (page_no == -1) || (rw == '\0') ){
                    fprintf(stderr,"invalid page feed entry @ line: %d\n",escManager->requests+1);
                    return -1;
                }
                esc_add(escManager,page_no,rw);
            }
            // result
            pageManager_report(escManager);
            printf("disk writes on page_faults: %d\n",escManager->disk_writes);
            break;

        default:
            perror("unknown error\n");
            return -1;
    }


    return 0;
}
