#include "fat12_substructs.h"


/*
This header file implements all structures used in mountfat12.c
It definies the basic structure of the implementet fileystem.
*/



// --------------------- SEKCTOR 0 --------------------------

struct BOOTSECTOR
{
    struct BIOS_PARAMETER_BLOCK BPB;
    uint8_t BOOTLOADER[512 - sizeof(struct BIOS_PARAMETER_BLOCK) - 2];
    uint8_t SIGNATURE[2];
}__attribute__((packed));


//  --------------------- SECTOR 1 to inc. 18 --------------------------

/*
The Filesystem contains two FATS, with 9 Sectors each.
Reason:
    The system has 2880 sectors. One FAT entry for each sector is 1.5 byte.
    => 2880 * 1.5 = 4230 
    8 * 512 < 4240 < 9 * 512 => 9 Sectors
    
The other FAT is a copy, for safety purposes.
*/

// Since 12 bits cannot be stored, we store duplicate entries.
struct FAT
{
    uint8_t BYTE[4320];
    uint8_t RESERVED[288];

    /*
    A FAT entry is 12 bits in size, or 1.5 bytes.

    The ENTIRE value is always considered and interpreted.

    Value Meaning
    0x000 Cluster free
    0x001 Reserved / not usable
    0x002 to 0xFEF (inclusive) Next cluster
    0xFF0 to 0xFF6 (inclusive) Reserved for special purposes
    0xFF7 Defective
    0xFF8 to 0xFFF (inclusive) End of file

    => Two of these are then combined to form a double entry.
    Therefore, byte1 belongs half to the lower entry and half to the upper one.
    */


}__attribute__((packed));

//  --------------------- SECTOR 19 to incl. 32 --------------------------
// Each entry corresponds to a file and must be located in the root directory.
// The root directory is 14 sectors by default. Therefore, there are 512 * 14 / 32 = 224 entries or files in this system.
// With 2847 data clusters, this equates to approximately 12 clusters per file, or about 6.5 KB.
struct ROOT_DIRECTORY
{
    struct DIR_ENTRY ENTRIES[224];     // 224 * 32 = 7168 = 14 * 512
};



//  ---------------------SECTOR  33 to incl. 2880 --------------------------
// => 2847 Cluster / Sectors
struct DATA
{
    struct CLUSTER CLUSTERS[2847]; // hier 7  
}__attribute__((packed));





// -----------------------------------------------------------------------------------------------------------



struct BIOS_PARAMETER_BLOCK BPB_LAYOUT = {
    .JMP = {0xEB, 0x3C, 0x90},       // We jump forward 60 bytes. Since the rest of the table is only 59 bytes in size,
    .OEM = "MS",                     // to land at the bootloadercode.

    .BYTES_PER_SECTOR     = 512,
    .SECTORS_PER_CLUSTER  = 1,       // FAT12 standard
    .AMOUNT_RESERVED_SECTORS = 1,    // FAT12/16: only the bootsector
    .NUM_FATs             = 2,       // always 2 FATs
    .FAT16_AMOUNT_ROOT_ENTRY = 224,  // Root-Directory: 224 Entrys
    .FAT16_TOTAL_SECTORS  = 2880,    // All Sectors(1.44 MB = 2880 Sectors)
    .MEDIA                = 0xF0,    // FAT12 Floppy Media Descriptor
    .FAT16_SIZE           = 9,       // Size of two FAT in Sectors
    .SECTORS_PER_TRACK    = 18,      // CHS-Layout
    .AMOUNT_HEADS         = 2,
    .HIDDEN_SECTORS       = 0,       // Floppy -> no hidden sectors

    .BIOS_DRIVE_NUMBER      = 0x00,   // Floppy Disk
    .RESERVED1              = 0,
    .EXTENDED_BOOT_SIGNATURE = 0x29,  // 0x29: VolumeID + Label + FS Type valid
    .VOLUME_ID              = 0x12345678,

    .VOLUME_LABEL             = "NO NAME    ",
    .FILE_SYSTEM_TYPE         = "FAT12   "
};  
