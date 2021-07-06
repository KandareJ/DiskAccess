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
    int result = 0;
    card.sdHandle = SD_open(Board_SD0, NULL);
    int_fast8_t status = SD_initialize(card.sdHandle);

    if (card.sdHandle == NULL) return -1;
    else if (status != SD_STATUS_SUCCESS) return -2;

    card.sector_size = SD_getSectorSize(card.sdHandle);
    card.num_sectors = SD_getNumSectors(card.sdHandle) - 1;
    card.txn_buffer = (char *) malloc(card.sector_size * sizeof(char));
    card.total_size = card.sector_size * card.num_sectors;
    result = SD_read(card.sdHandle, card.txn_buffer, 0, 1);
    if (result < 0) return -3;

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

int da_clear() {
    card.write_pos = 0;
    card.read_pos = 0;
    memset(card.txn_buffer, 0, card.sector_size);
    return 0;
}

int da_close() {
    int_fast8_t result;

    if (card.dirty) {
        result = SD_write(card.sdHandle, card.txn_buffer, card.cur_sector_num, 1);
        if (result != SD_STATUS_SUCCESS) return -1;
    }
    memset(card.txn_buffer, 0, card.sector_size);
    System_sprintf(card.txn_buffer, "%d:%d", card.write_pos, card.read_pos);
    result = SD_write(card.sdHandle, card.txn_buffer, 0, 1);
    if (result != SD_STATUS_SUCCESS) return -1;

    free(card.txn_buffer);
    SD_close(card.sdHandle);
    return 0;
}

int da_get_sector(int sector) {
    int_fast8_t result;
    if (card.sdHandle == NULL) return -4;
    sector = sector % card.num_sectors + 1;

    if (card.dirty != 0) {
        result = SD_write(card.sdHandle, card.txn_buffer, sector, 1);
        if (result != SD_STATUS_SUCCESS) return -1;
    }

    System_sprintf(card.txn_buffer, "%d:%d", card.write_pos, card.read_pos);
    result = SD_write(card.sdHandle, card.txn_buffer, 0, 1);
    if (result != SD_STATUS_SUCCESS) return -2;

    result = SD_read(card.sdHandle, card.txn_buffer, sector, 1);
    if (result != SD_STATUS_SUCCESS) return -3;
    card.cur_sector_num = sector;

    return 0;
}

int da_write(char* buffer, int size) {
    int result = 0;
    //if (size > card.total_size - da_get_data_size()) return -1;
    int totalWritten = 0;
    while (size > 0) {
        if (card.write_pos / card.sector_size != card.cur_sector_num) {
            result = da_get_sector(card.write_pos / card.sector_size);
            if (result < 0) return -1;
        }
        int nwrite = (size > card.sector_size - (card.write_pos % card.sector_size)) ? card.sector_size - (card.write_pos % card.sector_size) : size;
        memcpy(card.txn_buffer + (card.write_pos % card.sector_size), buffer + totalWritten, nwrite);

        card.dirty = 1;
        card.write_pos += nwrite;
        totalWritten += nwrite;
        size -= nwrite;
    }

    return 0;
}

int da_read(char* buffer, int size) {
    int result = 0;
    //if (size > da_get_data_size()) return -1;
    int totalRead = 0;
    while (size > 0) {
        if (card.read_pos / card.sector_size != card.cur_sector_num) {
            result = da_get_sector(card.read_pos / card.sector_size);
            if (result < 0) return -1;
        }

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

int da_get_read_pos() {
    return card.read_pos;
}

int da_get_write_pos() {
    return card.write_pos;
}

int da_get_sector_size() {
    return card.sector_size;
}

int da_get_num_sectors(){
    return card.num_sectors;
}
