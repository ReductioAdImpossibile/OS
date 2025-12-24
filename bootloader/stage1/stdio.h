#pragma once
#include "stdint.h"
#include "x86.h"

void putc(char c)
{
    x86_write_char(c);
}

void puts(const char* str)
{
   while(*str)
   {
        putc(*str);
        str++;
   }
}