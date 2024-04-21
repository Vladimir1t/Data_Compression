#ifndef DATA_COMPRESSION_H_INCLUDED
#define DATA_COMPRESSION_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "library_of_defines.h"

const int     MAX_WORD_SIZE          = 35;     // max size of each library word
const int     MAX_LIBRARY_SIZE       = 32767;  // 2^16 / 2
const int     MAX_ONE_BYTE_WORDS_NUM = 128;    // max number of 1 byte words
unsigned char MASK                   = 128;    // byte mask
unsigned char ONE_BYTE_WORD          = 0;      // 'word' which has a size 1 byte

enum RESULT
{
    ERROR         = -1,
    SUCCESS       =  1,
    OPEN_ERROR    =  2,
    CALLOC_ERROR  =  3,
    REALLOC_ERROR =  5,
    NULL_PTR      =  4
};

struct Library_word
{
    char           name[MAX_WORD_SIZE];
    unsigned short word_num;
};

struct Library
{
    struct Library_word* lib_words;
    size_t               size;
    size_t               capacity;
};

int read_library_file (FILE* library_file, struct Library* library);

size_t file_size_measure (FILE* const file_p);

int compress_data (char* string, unsigned char** buffer, size_t* buffer_size, struct Library* library);

int decode_data (FILE* sourse_file, struct Library* library, unsigned char** buffer, size_t* buffer_size);

int decoded_data_output (FILE* result_file, unsigned char* buffer, size_t buffer_size);


#endif // DATA_COMPRESSION_H_INCLUDED
