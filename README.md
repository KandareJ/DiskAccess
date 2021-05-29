<!-- Readme needs to be polished. Kinda just dumped all my thoughts here without proofreading or organizing :) -->
# Disk Access
The disk access library is a simple c library to make writing to an SD card easier for the TI microcontroller for the BACPAC project. 

## Motivation
Without access to a more robust filesystem like FAT, we implemented a simple library that utilizes TI's SDRaw library. In our project, we don't need to organize data from our sensor into files. Instead, it is all treated as one file and analyzed later on a more powerful computer.

## Transaction Buffer
To limit writes to the SD card, we write to a transaction buffer the size of a single sector in memory. Once we fill the transaction buffer, we write the data to the SD card using SDRaw.

## Circular, Two-Pointer
In our use case, we write everything to the SD card that comes in from the sensor. We only store that data temporarily. Once the data is read, we no longer need it. Because of this, it made sense to treat the SD card as a circular, two-pointer data structure. we track how much has been written and how much has been read. This removes any need to shift the data to keep it localized.

## Reserved Sector
For simplicity, we reserve the first sector to keep track of our read position pointer and our write position pointer.

## Test Cases
The dummy SDRaw.h and SDRaw.c are provided to run test cases without needing a microcontroller. The test cases just write to a byte array. Test cases are found in the main.c file.