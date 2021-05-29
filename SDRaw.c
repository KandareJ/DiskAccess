#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDRaw.h"

const int NUM_SECTORS = 3;
const int SECTOR_SIZE = 256;

int SD_init() {
    memory = (char*) malloc(NUM_SECTORS * SECTOR_SIZE * sizeof(char));
    memset(memory, 0, NUM_SECTORS * SECTOR_SIZE * sizeof(char));
    return 0;
}

int SD_initialize(SD_Handle handle) {
    return 0;
}

SD_Handle SD_open(int board, char* idk) {
    SD_Handle dummy;
    return dummy;
}


int SD_close(SD_Handle handle) {
    return 0;
}

int SD_write(SD_Handle handle, char* buf, int start_sector, int num_sectors) {
    if (start_sector >= NUM_SECTORS) return -1;
    memcpy(memory + start_sector * SECTOR_SIZE, buf, SECTOR_SIZE * num_sectors);
    return 0;
}

int SD_read(SD_Handle handle, char* buf, int start_sector, int num_sectors) {
    if (start_sector >= NUM_SECTORS) return -1;
    memcpy(buf, memory + start_sector * SECTOR_SIZE, SECTOR_SIZE * num_sectors);
    return 0;
}

int SD_getNumSectors() {
    return NUM_SECTORS;
}

int SD_getSectorSize() {
    return SECTOR_SIZE;
}

void dump() {
    for(int j = 0; j < NUM_SECTORS; j++) {
        for (int i = 0; i < SECTOR_SIZE; i++) {
            printf("%d ", memory[SECTOR_SIZE * j + i]);
        }
        printf("\n");
    }
}