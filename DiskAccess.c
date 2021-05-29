#include "DiskAccess.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern struct SDCard card;

// read when there's nothing left to read and write when out of space errors
// change function names to match sdraw

int da_initialize() {
    SD_init();
    return 0;
}

int da_load() {
    int delimiter = 0;
    card.sdHandle = SD_open(Board_SD0, NULL);
    SD_initialize(card.sdHandle);
    card.sector_size = SD_getSectorSize(card.sdHandle);
    card.num_sectors = SD_getNumSectors(card.sdHandle) - 1;
    card.txn_buffer = (char *) malloc(card.sector_size * sizeof(char));
    card.total_size = card.sector_size * card.num_sectors;
    SD_read(card.sdHandle, card.txn_buffer, 0, 1);

    int i = 0;
    while ( i < card.sector_size) {
        if (card.txn_buffer[i] == ':') {
            delimiter = i;
            break;
        }
        ++i;
    }

    if (delimiter) {
        card.write_pos = atoi(card.txn_buffer);
        card.read_pos = atoi(card.txn_buffer + delimiter + 1);
    }
    else {
        card.write_pos = 0;
        card.read_pos = 0;
    }

    card.cur_sector_num = -1;
    card.dirty = 0;

    return 0;
}

int da_close() {
    //figure out this wrap around shizz
    if (card.dirty) {
        SD_write(card.sdHandle, card.txn_buffer, card.cur_sector_num, 1);
    }
    memset(card.txn_buffer, 0, card.sector_size);
    sprintf(card.txn_buffer, "%d:%d", card.write_pos, card.read_pos);
    SD_write(card.sdHandle, card.txn_buffer, 0, 1);

    free(card.txn_buffer);
    SD_close(card.sdHandle);
    return 0;
}

int da_get_sector(int sector) {
    sector = sector % card.num_sectors + 1;
    if (card.dirty) {
        SD_write(card.sdHandle, card.txn_buffer, sector, 1);
        //write(card.cur_sector_num + 1, card.txn_buffer, card.sector_size);
    }

    sprintf(card.txn_buffer, "%d:%d", card.write_pos, card.read_pos);
    SD_write(card.sdHandle, card.txn_buffer, 0, 1);
    //write(0, card.txn_buffer, card.sector_size);

    SD_read(card.sdHandle, card.txn_buffer, sector, 1);
    //read(sector + 1, card.txn_buffer, card.sector_size);
    card.cur_sector_num = sector;
    //printf("LOADED IN: %s\n", card.txn_buffer);
    return 0;
}

int da_write(char* buffer, int size) {
    //if (size > card.total_size - da_get_data_size()) return -1;
    int totalWritten = 0;
    while (size > 0) {
        if (card.write_pos / card.sector_size != card.cur_sector_num) da_get_sector(card.write_pos / card.sector_size);

        int nwrite = (size > card.sector_size - (card.write_pos % card.sector_size)) ? card.sector_size - (card.write_pos % card.sector_size) : size;
        memcpy(card.txn_buffer + (card.write_pos % card.sector_size), buffer + totalWritten, nwrite);

        card.dirty = 1;
        card.write_pos += nwrite;
        totalWritten += nwrite;
        size -= nwrite;
    }

    return size;
}

int da_read(char* buffer, int size) {
    //if (size > da_get_data_size()) return -1;
    int totalRead = 0;
    while (size > 0) {
        if (card.read_pos / card.sector_size != card.cur_sector_num) da_get_sector(card.read_pos / card.sector_size);

        int nread = (size > card.sector_size - (card.read_pos % card.sector_size)) ? card.sector_size - (card.read_pos % card.sector_size) : size;
        memcpy(buffer + totalRead, card.txn_buffer + (card.read_pos % card.sector_size), nread);

        card.read_pos += nread;
        totalRead += nread;
        size -= nread;
    }

    return size;
}

int da_get_data_size() {
    int size = card.write_pos - card.read_pos;
    if (size < 0) size = card.total_size - size;
    return size;
}
