extern const int NUM_SECTORS;
extern const int SECTOR_SIZE;

char* memory;

typedef struct SD_Handle {} SD_Handle;

int SD_init();
int SD_initialize(SD_Handle handle);
SD_Handle SD_open(int board, char* idk);
int SD_close(SD_Handle handle);
int SD_write(SD_Handle handle, char* buf, int start_sector, int num_sectors);
int SD_read(SD_Handle handle, char* buf, int start_sector, int num_sectors);
int SD_getNumSectors();
int SD_getSectorSize();
void dump();

