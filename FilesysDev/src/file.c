
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


/*
Simple file IO Programm.

Example :     

    struct example
    {
        uint8_t a;
        uint8_t b;
        uint8_t c;
    };

    struct example ex;
    ex.a = 0b1;
    ex.b = 0b10;
    ex.c = 0b11;

    file* f = file_create("test.bin");
    
    file_pwrite_struct(f, &ex, sizeof(ex), 0);  // offset = 0
    
    unsigned char* buffer = file_pread_buffer(f, sizeof(ex), 0);
    size_t size = file_size(f);
    
    printf("file size : %ld Byte \n", size);
    file_close(f);

*/

typedef struct file
{
    int descriptor;
    char* path;
} file;


size_t file_size(struct file* self)
{
    struct stat st;
    if(stat(self->path, &st) == 0)
        return st.st_size;
    return -1;

}

void file_pwrite_buffer(struct file *self,  unsigned char* buffer, size_t buffer_size, size_t offset)
{
    ssize_t written = pwrite(self->descriptor, buffer, buffer_size, offset);
    if(written < 0)
    {
        perror("file.pwrite_buffer : write not successful.");
    }
}

void file_pwrite_struct(struct file *self, const void* strc, size_t strc_size, size_t offset)
{
    ssize_t written = pwrite(self->descriptor,strc, strc_size, offset);
    if(written < 0)
    {
        perror("file.pwrite_struct : write not successful.");
    }

    if ((size_t)written != strc_size) {
        fprintf(stderr,
                "file.pwrite_struct: only %zd of %zu bytes written\n",
                written, strc_size);
    }

}


unsigned char* file_pread_buffer(struct file *self, size_t buffer_size, size_t offset)
{
    unsigned char* buffer = malloc(buffer_size);
    ssize_t n = pread(self->descriptor, buffer, buffer_size, offset);
    if(n < 0)
    {
        perror("file.pread_buffer : read not successful.");
    } 
    return buffer;
}



file* file_init(char* path)
{
    file* self = malloc(sizeof(file));

    self->path = path;
    self->descriptor = open(path, O_RDWR);
    if(self->descriptor < 0)
    {
        fprintf(stderr, "file_init: %s not found. ", path);
        exit(1);
    }

    return self;

}

file* file_create(char* path)
{
    file* self = malloc(sizeof(file));

    self->path =  strdup(path);
    self->descriptor = open(path, O_TRUNC | O_CREAT | O_RDWR, 0644);
    if(self->descriptor < 0)
    {
        perror("file_create : file creation not possible");
        exit(1);
    }

    return self;
}

void file_close(struct file* self)
{
    free(self);
}



