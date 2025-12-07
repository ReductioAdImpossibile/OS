#include "fat12_substructs.h"





// --------------------- SEKTOR 0 --------------------------

struct BOOTSECTOR
{
    struct BIOS_PARAMETER_BLOCK BPB;
    uint8_t BOOTLOADER[512 - sizeof(struct BIOS_PARAMETER_BLOCK) - 2];
    uint8_t SIGNATURE[2];
}__attribute__((packed));


//  --------------------- SEKTOR 1 bis inkl. 18 --------------------------

// Es gibt zwei FATS, mit jeweils 9 Sektoren größe.
// Grund : 2880 Sektoren = 2880 Cluster => 1.5 Byte entry pro Cluster * 2880 = 4320
// Beide FATS decken den gleichen Bereich ab.

// Da 12 Bit nicht implementierbar sind nutzen wir doppeleinträge mit größe 3 Byte : 
struct FAT
{
    uint8_t BYTE[4320];
    uint8_t RESERVED[288];


    /*
    Ein Eintrag der FAT ist 12 Bit groß. Also 1.5 Byte. 
    Hier wird immer der GESAMTE Wert beachtet und interpretiert. 

        Wert                        Bedeutung
        0x000                       Cluster frei
        0x001                       reseviert / nicht nutzbar
        0x002 bis inkl. 0xFEF       nächstes Cluster
        0xFF0 bis inkl. 0XFF6       reserviert für spezielle zwecke
        0xFF7                       defekt
        0xFF8 bis inkl. 0xFFF       end of file

    => zwei davon werden dann zu einer double entry zusammengefügt.
        Also gehört byte1 zur hälfte zum unteren Entry und zum oberen.
    */


}__attribute__((packed));

//  --------------------- SEKTOR 19 bis inkl. 32 --------------------------
// Jeder Eintrag entspricht einer Datei und muss in Root Directory liegen.
// Das Root Directory ist standarmäßig 14 Sektoren groß. Also gibt es 512 * 14 / 32  = 224 Einträge bzw Dateien in diesem System.
// Bei 2847 Data Clustern ist das eine größe von ca. 12 Clustern/Datei also ca 6.5KB.
struct ROOT_DIRECTORY
{
    struct DIR_ENTRY ENTRIES[224];     // 224 * 32 = 7168 = 14 * 512
};



//  ---------------------SEKTOR  33 bis 2880 --------------------------
// => 2847 Cluster bzw Sektoren
struct DATA
{
    struct CLUSTER CLUSTERS[2847]; // hier 7  
}__attribute__((packed));





// -----------------------------------------------------------------------------------------------------------



struct BIOS_PARAMETER_BLOCK BPB_LAYOUT = {
    .JMP = {0xEB, 0x3C, 0x90},       // Wir springen 60 Byte nach vorne. Da die restliche Tabelle nur 59 Byte groß ist, 
    .OEM = "MS",               // landen wir beim bootloadercode.

    .BYTES_PER_SECTOR     = 512,
    .SECTORS_PER_CLUSTER  = 1,       // FAT12 Standard
    .AMOUNT_RESERVED_SECTORS = 1,    // FAT12/16: nur der Bootsektor selbst
    .NUM_FATs             = 2,       // immer 2 FATs
    .FAT16_AMOUNT_ROOT_ENTRY = 224,  // Root-Verzeichnis: 224 Einträge
    .FAT16_TOTAL_SECTORS  = 2880,    // Gesamtsektoren (1.44 MB)
    .MEDIA                = 0xF0,    // FAT12 Floppy Media Descriptor
    .FAT16_SIZE           = 9,       // Größe einer FAT in Sektoren
    .SECTORS_PER_TRACK    = 18,      // CHS-Layout
    .AMOUNT_HEADS         = 2,
    .HIDDEN_SECTORS       = 0,       // Floppy → keine Hidden Sectors

    .BIOS_DRIVE_NUMBER      = 0x00,   // Floppy A:
    .RESERVED1              = 0,
    .EXTENDED_BOOT_SIGNATURE = 0x29,  // 0x29: VolumeID + Label + FS Type gültig
    .VOLUME_ID              = 0x12345678,

    .VOLUME_LABEL             = "NO NAME    ",
    .FILE_SYSTEM_TYPE         = "FAT12   "
};  
