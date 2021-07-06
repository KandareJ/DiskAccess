#ifndef DISKACCESS_H
#define DISKACCESS_H

#include <ti/drivers/SD.h>
#include <xdc/runtime/System.h>
#include "Board.h"


// comment out these lines below when putting on board
//#include "SDRaw.h"
//#define Board_SD0 0
// =======================================================

struct SDCard
{
    char* txn_buffer; // all writing is done to txn buffer. Buffer is written when dirty
    int sector_size;
    int num_sectors;
    int write_pos;
    int read_pos;
    int cur_sector_num;
    int dirty;
    int total_size;
    SD_Handle sdHandle;
};

struct SDCard card;

// initializes sd card
int da_initialize();

int da_get_read_pos();
int da_get_write_pos();
int da_get_sector_size();
int da_get_num_sectors();

//free txn buffer
int da_destructor();

int da_clear();

// load SD card info into SD struct
int da_load();
int da_close();

// writes to sd card. We only append.
int da_write(char* buffer, int size);

// reads from position. Position should be initialized to second sector.
// first sector reserved to track file size
int da_read(char* buffer, int size);

int da_get_sector(int sector);

int da_get_data_size();

#endif
