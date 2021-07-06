#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SD.h>
#include <ti/display/Display.h>

/* Example/Board Header files */
#include "Board.h"
#include "DiskAccess.h"
#include <xdc/runtime/System.h>

static Display_Handle display;

void *mainThread(void *arg0) {
    char myBuf[256];

    Display_init();
    GPIO_init();
    da_initialize();

    /* Configure the LED pin */
    GPIO_setConfig(Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Open the display for output */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL) {
        /* Failed to open display driver */
        while (1);
    }

    /* Turn on user LED */
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);

    Display_printf(display, 0, 0, "Starting the SD example\n");

    Display_printf(display, 0, 0, "Attempting to load card: ");
    int res = da_load();
    if (res == 0) Display_printf(display, 0, 0, "Success\n");
    else {
        if (res == -2) Display_printf(display, 0, 0, "Failure: status returned failure\n");
        else if (res == -1) Display_printf(display, 0, 0, "Failure: card handle null\n");
        else Display_printf(display, 0, 0, "Failure: unable to read first sector\n");
        while(1);
    }

    System_sprintf(myBuf, "Num Sectors: %d\nSector Size: %d\n", da_get_num_sectors(), da_get_sector_size());
    Display_printf(display, 0, 0, myBuf);

    System_sprintf(myBuf, "Read pos: %d\nwrite pos: %d\n", da_get_read_pos(), da_get_write_pos());
    Display_printf(display, 0, 0, myBuf);

    Display_printf(display, 0, 0, "Attempting to write to card:");


    res = da_write("The quick brown fox jumped over the lazy dog.", 46);
    if (res == 0) Display_printf(display, 0, 0, "Wrote characters to the sd card.\n");
    else {
        System_sprintf(myBuf, "Got back: %d\n", res);
        Display_printf(display, 0, 0, myBuf);
        while(1);
    }

    Display_printf(display, 0, 0, "Attempting to read now\n");

    res = da_read(myBuf, 46);
    if (res == 0) {
        Display_printf(display, 0, 0, "successfully read: ");
        Display_printf(display, 0, 0, myBuf);
    }
    else {
        Display_printf(display, 0, 0, "Error reading data\n");
        while(1);
    }

    //Display_printf(display, 0, 0, "Clearing card\n");
    //da_clear();

    Display_printf(display, 0, 0, "Attempting to close sd card\n");
    res = da_close();

    if (res == 0) {
        Display_printf(display, 0, 0, "SD card closed successfully!\n");
    }
    else {
        Display_printf(display, 0, 0, "Error closing sd card");
        while(1);
    }

    while(1);

    return (NULL);
}
