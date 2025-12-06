

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

typedef struct file
{
    int descriptor;
    char* path;
} file;

size_t file_size(struct file* self);

void file_pwrite_buffer(struct file *self,  unsigned char* buffer, size_t buffer_size, size_t offset);

void file_pwrite_struct(struct file *self, const void* strc, size_t strc_size, size_t offset);

unsigned char* file_pread_buffer(struct file *self, size_t buffer_size, size_t offset);

file* file_init(char* path);

file* file_create(char* path);

void file_close(struct file* self);
