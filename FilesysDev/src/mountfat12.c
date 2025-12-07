
/*
This program creates a FAT12 disk.img for a floppy disk following the QEMU standard.
It allows the user to install it a boot0, boot1 and kernel binary.
This is not a driver. Its a program to create a img, which a driver will be based upon.

---------------------------------------------------------------------------------------------
run with : ./mountfat12 --bootstage0 <file.bin> --bootstage1 <file.bin> --kernel <file.bin>
---------------------------------------------------------------------------------------------

The resulting file will always be called disk.img.
The bootstage0 file needs to be at least 448 Bytes big. If its bigger,
the values of the Bytes 448 to 512 will be lost, due to the placement of the Bios Parameter Block.
The programm appends the boot signature 0x55 0xAA by itself at the end of sector 0.

QEMU standard: 1440 KB, 512-byte sector size,

 - 80 cylinders (0 to 79)
 - 2 heads (0 to 1)
 - 18 sectors per track on one head (1 to 18)
    -> 36 sectors per cylinder distributed across two heads (seen in 3D).
    -> One ring therefore has 18 sectors, there are 80 rings, and there are two such platters.
*/



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "../header/file.h"
#include "../header/fat12_structs.h"


char *IMGNAME = "disk.img";
const unsigned long long IMAGE_SIZE = 1440ULL * 1024ULL; // 1440KB 
const size_t CLUSTER_SIZE = 512;
const size_t DIR_ENTRY_SIZE = sizeof(struct DIR_ENTRY);

file* img;
size_t offset = 0;
int8_t signature[2] = {0x55, 0xAA};


struct BOOTSECTOR __BOOTSECTOR;
struct FAT __FAT;
struct ROOT_DIRECTORY __ROOT_DIRECTORY;
struct DATA __DATA;


uint16_t FAT_CLUSTER_OFFSET = 2;    // (count from 0)
uint16_t ROOT_DIR_OFFSET = 0;


void CREATE_IMG()
{
    /*
    We create a .img out of zeros with the given size.
    */
    img = file_create("disk.img");
    size_t buffer_size = 1024;
    uint8_t* buffer = calloc(1, 1024);

    for(int i = 0; i < IMAGE_SIZE / buffer_size; i++)
        file_pwrite_buffer(img, buffer, buffer_size, i * buffer_size);
    
}

void INIT_BOOTLOADER(char* bootstage0)
{
    /*
    We write the stage 0 bootloader inside the first sector of our .img and include the 
    Bios Parameter Block.
    */
    __BOOTSECTOR.BPB = BPB_LAYOUT;
    
    file* bootfile = file_init(bootstage0);
    uint8_t* buffer = file_pread_buffer(bootfile, sizeof(__BOOTSECTOR.BOOTLOADER), 0);
    
    memcpy(__BOOTSECTOR.BOOTLOADER, buffer, sizeof(__BOOTSECTOR.BOOTLOADER));
    memcpy(__BOOTSECTOR.SIGNATURE, signature, 2);
}

void INIT_FAT()
{   
    /*
    Initialise first two clusters as reserved.
    */
    __FAT.BYTE[0] = 0xF0; 
    __FAT.BYTE[1] = 0xFF;
    __FAT.BYTE[2] = 0xFF;
}

// ---------------------- FAT FUNCTIONS ----------------------------

void SET_FAT_ENTRY(uint16_t cluster_offset, uint16_t value)
{
    /*
    This functions allows us to change the fat entry for a cluster to value.
    */

    int byte_offset = (cluster_offset * 3) / 2;
    if(cluster_offset % 2 == 0)
    {
        
        __FAT.BYTE[byte_offset]     = value & 0xFF;        
        __FAT.BYTE[byte_offset+1]   = (__FAT.BYTE[byte_offset + 1] & 0xF0) | ((value >> 8) & 0x0F) ; 
    }
    else
    {   
        __FAT.BYTE[byte_offset] = (__FAT.BYTE[byte_offset] & 0x0F ) | ((value >> 4) & 0xF0);
        __FAT.BYTE[byte_offset + 1] = value & 0xFF;
    }

    /* 
    FAT Entrys: 1. =  12A, 2. = 34B
    will be : 
    Byte 0 = 2 A        (low 1.)
    Byte 1 = 3 1        (high 4 bit 2. | high 4 bit 1.) 
    Byte 2 = 4 B        (low 2.)
    */
    
}

uint16_t GET_FAT_ENTRY(uint16_t cluster_offset)
{
    /*
    Get an entry for a cluster.
    */
    int byte_offset = (cluster_offset * 3) / 2;

    uint16_t res;
    if(cluster_offset % 2 == 0)
    {
        res = (__FAT.BYTE[byte_offset] & 0xFF ) | ((__FAT.BYTE[byte_offset+1] & 0x0F) << 8 );
    }
    else
    {
        res = ((__FAT.BYTE[byte_offset] & 0xF0) << 4) | (__FAT.BYTE[byte_offset+1] & 0xFF );
    }
    return res & 0x0FFF;
}


// ---------------------- DIR ENTRY FUNCTIONS ---------------------------

struct DIR_ENTRY* GET_ROOT_DIR_ENTRY(char* filename)
{
    /*
    Returns a root dir entry as struct root dir. This function uses malloc.
    Filename has to be 8 Byte big.
    */

    struct DIR_ENTRY* root_entry = malloc(DIR_ENTRY_SIZE);
    for(int j = 0; j < sizeof(struct ROOT_DIRECTORY) / DIR_ENTRY_SIZE ; j++)
    {
        if(memcmp(filename, __ROOT_DIRECTORY.ENTRIES[j].FILENAME, 8) == 0)
        {   
            *root_entry = __ROOT_DIRECTORY.ENTRIES[j];
            return root_entry;
        }
    } 
    return NULL;
}

struct DIR_ENTRY* GET_DIR_ENTRY(char* filename, char* end, struct CLUSTER* cluster)       
{
    
    /*
    Reads an entire cluster an checks if it contains a file named 'filename'(8 Byte) with the ending 'end' (3 Byte)
    */
    struct DIR_ENTRY *entry = malloc(DIR_ENTRY_SIZE);       
    for(int i = 0; i < CLUSTER_SIZE ; i += DIR_ENTRY_SIZE )
    {
        memcpy(entry, &cluster->bytes[i], DIR_ENTRY_SIZE );
        
        if(memcmp(entry->FILENAME, filename, 8) == 0 && memcmp(entry->END, end, 3) == 0)
            return entry;   
    }
    
    free(entry);

    return NULL;
}

struct CLUSTER* GET_CLUSTER(char* PATH, char* END, char* filename) // hier noch mit char* end
{
    /*
    This functions accepts a path, which contains the filename without the end, the file-ending and the filename.
    It returns      
    */

    char dirname[8];
    memset(dirname, ' ', 8);
    
    char* last_slash = PATH;
    char* current_slash = PATH + 1;
    for(current_slash; *current_slash != '/' && *current_slash; current_slash++)
            ;

    memcpy(dirname, last_slash + 1, current_slash  - (last_slash + 1));
    
   
    struct DIR_ENTRY* entry = GET_ROOT_DIR_ENTRY(dirname);      
    struct CLUSTER* cluster = NULL;

    while(entry != NULL)
    {
        last_slash = current_slash;
        current_slash += 1;
        for(current_slash; *current_slash != '/' && *current_slash; current_slash++)
            ;
        memset(dirname, ' ', 8);
        memcpy(dirname, last_slash + 1, current_slash  - (last_slash + 1));

        cluster = &__DATA.CLUSTERS[entry->FIRST_CLUSTER];
        entry = GET_DIR_ENTRY(dirname, END, cluster);
    }

    if(cluster == NULL)
        return NULL;

    return (memcmp(dirname, filename, 8) == 0) ? cluster : NULL;
    
}

void SET_NEW_DIR_ENTRY(struct DIR_ENTRY* new_entry, struct CLUSTER* cluster)
{
    //printf("--- \n");
    uint8_t zeros[DIR_ENTRY_SIZE ];
    memset(zeros, 0, DIR_ENTRY_SIZE );
    for(int i = 0; i < CLUSTER_SIZE ; i += DIR_ENTRY_SIZE )
    {
        if(memcmp(&cluster->bytes[i], &zeros, DIR_ENTRY_SIZE) == 0)     
        {
            //printf("%d \n",i);
            memcpy(&cluster->bytes[i], new_entry, DIR_ENTRY_SIZE);
            break;
        }
    }
}

// ---------------------- HELPING FUNCTIONS ------------------------------
char* GET_FILENAME(char* PATH)
{
    char* dirname = malloc(8 * sizeof(char));
    memset(dirname, ' ', 8);

    size_t start = 0;
    size_t end = 0;
    for(char* ptr = PATH; *ptr; ptr++)
    {
        end++;
        if(*ptr == '/')
            start = end;
    }

    memcpy(dirname, PATH + start, end - start);
    return dirname;
}

char* GET_ROOT_DIR_NAME(char* PATH)
{
    char* dirname = malloc(8 * sizeof(char));
    memset(dirname, ' ', 8);

    size_t pos = 0;
    for(char* ptr = PATH + 1; *ptr && *ptr != '/'; ptr++)
    {
        pos++;
    }

    memcpy(dirname, PATH + 1, pos);
    return dirname;
}



// ----------------------- INSTALLING FUNCTIONS -------------------------

void MKROOTDIR(char* NAME)
{   

    NAME++;
    SET_FAT_ENTRY(FAT_CLUSTER_OFFSET,  0x0FFF);

    char filename[8];
    memset(filename, ' ', 8);

    
    size_t size = 0;
    for(size; *(NAME + size); size++);
    memcpy(filename, NAME, size);
    

    struct DIR_ENTRY entry = {
        .ATTRIBUTES = 0x10,
        .RESERVED = 0x00,
        .CREATION_TIME_IN_10THSEC = 0,
        .CREATE_TIME = 0,
        .CREATION_DATE = 0,
        .LAST_ACCESS_DATE = 0,
        .FAT32_HIGH_DATE = 0,
        .LAST_WRITE_DATE = 0,
        .LAST_WRITE_TIME = 0,
        .FIRST_CLUSTER = FAT_CLUSTER_OFFSET,
        .FILE_SIZE = 0
    };

    memcpy(entry.FILENAME, filename, 8);
    memcpy(entry.END, "   ", 3);

    __ROOT_DIRECTORY.ENTRIES[ROOT_DIR_OFFSET] = entry;
    FAT_CLUSTER_OFFSET++;
    ROOT_DIR_OFFSET++;

}

void MKDIR(char* PATH)
{

    char* filename = GET_FILENAME(PATH);
    char* root_dir_name = GET_ROOT_DIR_NAME(PATH);
    if(memcmp(filename, root_dir_name, 8) == 0)
    {
        MKROOTDIR(PATH);
        return;
    }


    struct CLUSTER* cluster = GET_CLUSTER(PATH, "   ", filename);
    if(cluster == NULL)
    {
        printf("Path : %s does not exist.", PATH);
        return;
    }

    struct DIR_ENTRY new_entry = {
        .ATTRIBUTES = 0x10,
        .RESERVED = 0x00,
        .CREATION_TIME_IN_10THSEC = 0,
        .CREATE_TIME = 0,
        .CREATION_DATE = 0,
        .LAST_ACCESS_DATE = 0,
        .FAT32_HIGH_DATE = 0,
        .LAST_WRITE_DATE = 0,
        .LAST_WRITE_TIME = 0,
        .FIRST_CLUSTER = FAT_CLUSTER_OFFSET,
        .FILE_SIZE = 0
    };
    memcpy(new_entry.FILENAME, filename, 8);
    memcpy(new_entry.END, "   ", 3);

    SET_NEW_DIR_ENTRY(&new_entry, cluster);

    SET_FAT_ENTRY(FAT_CLUSTER_OFFSET,  0x0FFF);
    FAT_CLUSTER_OFFSET++;

    free(filename);
    free(root_dir_name);        
}   

void MKFILE(char* PATH, char* END, size_t size)
{

    size_t sectors = size / 512;  
    if(size % 512 > 0)
        sectors++;

    char* filename = GET_FILENAME(PATH);
    char* root_dir_name = GET_ROOT_DIR_NAME(PATH);
    
    char end[3];
    memset(end, ' ', 3);
    for(int j = 0; *(END + j); j++)
        end[j] =  *(END + j);


    struct CLUSTER* cluster = GET_CLUSTER(PATH, "   ", filename);
    
    struct DIR_ENTRY new_entry = {
        .ATTRIBUTES = 0x10,
        .RESERVED = 0x00,
        .CREATION_TIME_IN_10THSEC = 0,
        .CREATE_TIME = 0,
        .CREATION_DATE = 0,
        .LAST_ACCESS_DATE = 0,
        .FAT32_HIGH_DATE = 0,
        .LAST_WRITE_DATE = 0,
        .LAST_WRITE_TIME = 0,
        .FIRST_CLUSTER = FAT_CLUSTER_OFFSET,
        .FILE_SIZE = 0
    };
    memcpy(new_entry.FILENAME, filename, 8);
    memcpy(new_entry.END, END, 3);

    SET_NEW_DIR_ENTRY(&new_entry, cluster);

    for(int i = 0; i < sectors-1; i++)
    {
        SET_FAT_ENTRY(FAT_CLUSTER_OFFSET,  FAT_CLUSTER_OFFSET + 1);
        FAT_CLUSTER_OFFSET++;
    }
    SET_FAT_ENTRY(FAT_CLUSTER_OFFSET++,  0x0FFF);

    free(filename);
    free(root_dir_name);
}

void WFILE(char* PATH, char* END, char* stream, size_t stream_size)
{   

    char* filename = GET_FILENAME(PATH);
    char* root_dir_name = GET_ROOT_DIR_NAME(PATH);
    
    
    char end[3];
    memset(end, ' ', 3);
    for(int j = 0; *(END + j); j++)
        end[j] =  *(END + j);

    
    struct CLUSTER* cluster = GET_CLUSTER(PATH, "   ", filename);
    struct DIR_ENTRY* entry = GET_DIR_ENTRY(filename, end, cluster);
    

    uint16_t cluster_offset = entry->FIRST_CLUSTER;

    char buffer[512];
    ssize_t left2write = stream_size; 

    for(left2write; left2write > 0 && cluster_offset != 0x0FFF; left2write -= 512)
    {

        size_t buffer_size = left2write >= sizeof(buffer) ? sizeof(buffer) : left2write;

        memset(buffer, 0, sizeof(buffer));
        memcpy(buffer, stream + (stream_size - left2write), buffer_size); 
        
        
        memcpy(&__DATA.CLUSTERS[cluster_offset], buffer, buffer_size);

        cluster_offset = GET_FAT_ENTRY(cluster_offset); 
    }

}

void WRITE_IMG()
{

    file_pwrite_struct(img, &__BOOTSECTOR, sizeof(__BOOTSECTOR), offset);
    offset += sizeof(__BOOTSECTOR);

    file_pwrite_struct(img, &__FAT, sizeof(__FAT), offset);
    offset += sizeof(__FAT);

    file_pwrite_struct(img, &__FAT, sizeof(__FAT), offset);
    offset += sizeof(__FAT);

    file_pwrite_struct(img, &__ROOT_DIRECTORY, sizeof(__ROOT_DIRECTORY), offset);
    offset += sizeof(__ROOT_DIRECTORY);

    file_pwrite_struct(img, &__DATA, sizeof(__DATA), offset);

}


void main(int argc, char *argv[])
{
    char *boot0_path = NULL;
    char *boot1_path = NULL;
    char *kernel_path = NULL;
    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "--bootstage0") == 0) 
        {
            if (i + 1 < argc) 
                boot0_path = argv[++i];
        }
        else if (strcmp(argv[i], "--bootstage1") == 0) 
        {
            if (i + 1 < argc) 
                boot1_path = argv[++i];
        }
        else if (strcmp(argv[i], "--kernel") == 0)
        {
            if (i + 1 < argc) 
                kernel_path = argv[++i];
        }
    }

    if(boot0_path == NULL || boot1_path == NULL || kernel_path == NULL)
    {
        printf("Wrong Input.");
        exit(1);
    }

    struct BOOTSECTOR __BOOTSECTOR;
    __FAT = (struct FAT){0};
    __ROOT_DIRECTORY = (struct ROOT_DIRECTORY) {0};
    __DATA = (struct DATA){0};

    CREATE_IMG();
    INIT_BOOTLOADER(boot0_path);
    INIT_FAT();

    MKDIR("/boot");
    MKDIR("/kernel");


    file* boot1_file = file_init(boot1_path);
    size_t boot1_size = file_size(boot1_file);
    MKFILE("/boot/bootst1", "bin", boot1_size);
        
    char* buffer_b1 = file_pread_buffer(boot1_file, boot1_size, 0);
    WFILE("/boot/bootst1", "bin", buffer_b1, boot1_size);



    file* kernel_file = file_init(kernel_path);
    size_t kernel_size = file_size(kernel_file);
    MKFILE("/kernel/kcode", "bin", kernel_size);
    
    char* buffer_k = file_pread_buffer(kernel_file, kernel_size, 0);
    WFILE("/kernel/kcode", "bin", buffer_k, kernel_size);

    WRITE_IMG();
}


