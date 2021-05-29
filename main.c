#include <stdio.h>
#include <string.h>
#include "DiskAccess.h"

int test1();
int test2();
int test3();
int test4();
int test5();
int test6();
int test7();
int test8();
int test9();
int test10();
int test11();

int main() {
    printf("Simple read and write test 1......................");
    if (test1()) printf("FAILED\n");
    else printf("PASSED\n");

    printf("Simple read and write test 2......................");
    if (test2()) printf("FAILED\n");
    else printf("PASSED\n");

    printf("Simple read and write test 3......................");
    if (test3()) printf("FAILED\n");
    else printf("PASSED\n");

    printf("Read and write over sector bounds test 1..........");
    if (test4()) printf("FAILED\n");
    else printf("PASSED\n");

    printf("Read and write over sector bounds test 2..........");
    if (test5()) printf("FAILED\n");
    else printf("PASSED\n");

    printf("Read and write over sector bounds test 3..........");
    if (test6()) printf("FAILED\n");
    else printf("PASSED\n");

    printf("Read and write over sd bounds test 1..............");
    if (test7()) printf("FAILED\n");
    else printf("PASSED\n");

    printf("Read and write over sd bounds test 2..............");
    if (test8()) printf("FAILED\n");
    else printf("PASSED\n");

    printf("Write then read small chunks 1....................");
    if (test9()) printf("FAILED\n");
    else printf("PASSED\n");

    printf("Write then read small chunks 2....................");
    if (test10()) printf("FAILED\n");
    else printf("PASSED\n");

    printf("Load card, write, close, open and read 1..........");
    if (test11()) printf("FAILED\n");
    else printf("PASSED\n");

    // add test for reading more than is written and make sure it doesn't break

    return 0;
}

int test1() {
    char test[28] = "First test will be this one";
    char write[50];
    char read[50];

    sprintf(write, "%s", test);
    da_initialize();
    da_load();

    da_write(write, 28);
    da_read(read, 28);

    da_close();

    return strcmp(test, read);
}

int test2() {
    char test[18] = "NOw we double it?";
    char write[50];
    char read[50];

    sprintf(write, "%s%s", test, test);
    da_initialize();
    da_load();

    da_write(write, 36);
    da_read(read, 17);

    da_close();

    return memcmp(test, read, 17);
}

int test3() {
    char test[1] = "";
    char write[50];
    char read[50];

    sprintf(write, "%s", test);
    da_initialize();
    da_load();

    da_write(write, 1);
    da_read(read, 1);

    da_close();

    return strcmp(test, read);
}

int test4() {
    char write[65] = "The quick brown fox jumped over the lazy dog-no one cared though";
    char read[100];

    da_initialize();
    da_load();

    for (int i = 0; i < 16; i++) {
        da_write(write, 64);
    }

    da_write(write, 64);
    da_read(read, 64);
    
    da_close();

    return memcmp(read, write, 64);
}

int test5() {
    char write[65] = "The quick brown fox jumped over the lazy dog-no one cared though";
    char read[100];

    da_initialize();
    da_load();

    for (int i = 0; i < 16; i++) {
        da_write(write, 64);
    }

    for (int i = 0; i < 16; i++) {
        da_read(read, 64);
    }

    da_write(write, 64);
    da_read(read, 64);
    
    da_close();

    return memcmp(read, write, 64);
}

int test6() {
    char write[65] = "The quick brown fox jumped over the lazy dog-no one cared though";
    char read[100];

    da_initialize();
    da_load();

    da_write(write, 1);

    for (int i = 0; i < 16; i++) {
        da_write(write, 64);
    }

    for (int i = 0; i < 16; i++) {
        da_read(read, 64);
    }

    da_write(write, 64);
    da_read(read, 64);
    
    da_close();

    return memcmp(read+1, write, 63);
}

int test7() {
    char write[65] = "The quick brown fox jumped over the lazy dog-no one cared though";
    char read[100];

    da_initialize();
    da_load();

    for (int j = 0; j < NUM_SECTORS - 1; j++) {
        for (int i = 0; i < 16; i++) {
            da_write(write, 64);
        }

        for (int i = 0; i < 16; i++) {
            da_read(read, 64);
        }
    }
    

    da_write(write, 64);
    da_read(read, 64);
    
    da_close();

    return memcmp(read, write, 64);
}

int test8() {
    char write[65] = "The quick brown fox jumped over the lazy dog-no one cared though";
    char read[100];

    da_initialize();
    da_load();

    da_write(write, 1);

    for (int j = 0; j < NUM_SECTORS - 1; j++) {
        for (int i = 0; i < 16; i++) {
            da_write(write, 64);
        }

        for (int i = 0; i < 16; i++) {
            da_read(read, 64);
        }
    }
    

    da_write(write, 64);
    da_read(read, 64);
    
    da_close();

    return memcmp(read+1, write, 63);
}

int test9() {
    char write[65] = "The quick brown fox jumped over the lazy dog-no one cared though";
    char read[100];

    da_initialize();
    da_load();

    for (int i = 0; i < 64; i++) {
        da_write(write + i, 1);
    }

    da_read(read, 64);
    
    da_close();

    return memcmp(read, write, 64);
}

int test10() {
    char write[65] = "The quick brown fox jumped over the lazy dog-no one cared though";
    char read[100];

    da_initialize();
    da_load();

    da_write(write, 64);

    for (int i = 0; i < 64; i++) {
        da_read(read + i, 1);
    }
    
    da_close();

    return memcmp(read, write, 64);
}

int test11() {
    char write[65] = "The quick brown fox jumped over the lazy dog-no one cared though";
    char read[100];

    da_initialize();
    da_load();

    da_write(write, 64);
    da_close();

    da_load();

    da_read(read, 64);
    read[65] = 0;

    da_close();

    return memcmp(read, write, 64);
}