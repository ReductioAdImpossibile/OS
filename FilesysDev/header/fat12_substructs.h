#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


/*
This file definies substructures, which are used for definitions of the structures in fat12_structs.h
*/

// 62 Byte
struct BIOS_PARAMETER_BLOCK 
{
    uint8_t  JMP[3];
    uint8_t  OEM[8];              

    uint16_t BYTES_PER_SECTOR;     
    uint8_t  SECTORS_PER_CLUSTER;  
    uint16_t AMOUNT_RESERVED_SECTORS;  
    uint8_t  NUM_FATs;             
    uint16_t FAT16_AMOUNT_ROOT_ENTRY; 
    uint16_t FAT16_TOTAL_SECTORS;  
    uint8_t  MEDIA;                
    uint16_t FAT16_SIZE;          
    uint16_t SECTORS_PER_TRACK;    
    uint16_t AMOUNT_HEADS;         
    uint32_t HIDDEN_SECTORS;       
    uint32_t TOTAL_SECTORS_32;     
    // 25

    uint8_t  BIOS_DRIVE_NUMBER;
    uint8_t  RESERVED1;
    uint8_t  EXTENDED_BOOT_SIGNATURE; 
    uint32_t VOLUME_ID;
    uint8_t  VOLUME_LABEL[11];
    uint8_t  FILE_SYSTEM_TYPE[8];   
    // 26


} __attribute__((packed));


// 32 Byte
struct DIR_ENTRY
{
    char FILENAME[8];
    char END[3];
    uint8_t ATTRIBUTES;
    uint8_t RESERVED;

    uint8_t CREATION_TIME_IN_10THSEC;
    uint16_t CREATE_TIME;
    uint16_t CREATION_DATE;
    uint16_t LAST_ACCESS_DATE;
    uint16_t FAT32_HIGH_DATE;

    uint16_t LAST_WRITE_TIME;
    uint16_t LAST_WRITE_DATE;

    u_int16_t FIRST_CLUSTER;
    uint32_t FILE_SIZE;

}__attribute__((packed));

// 512 Byte
struct CLUSTER
{
    uint8_t bytes[512];

}__attribute__((packed));

