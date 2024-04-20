#ifndef DATA_COMPRESSION_H_INCLUDED
#define DATA_COMPRESSION_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#define SPEC "%u"
typedef unsigned char elem_t;

const int MAX_WORD_SIZE          = 25;
const int MAX_LIBRARY_SIZE       = pow (2, 2 * 8);
const int MAX_ONE_BYTE_WORDS_NUM = 128;

enum RESULT
{
    ERROR         = -1,
    SUCCESS       =  1,
    OPEN_ERROR    =  2,
    CALLOC_ERROR  =  3,
    REALLOC_ERROR =  5,
    NULL_PTR      =  4
};

enum Elem_t
{
    one_byte = 1,
    two_byte = 2,
};

union Word_type_num
{
    unsigned char  one_byte_num;
    unsigned short two_byte_num;
};

struct Library_word
{
    char                name[MAX_WORD_SIZE];
    union Word_type_num word_num;
    Elem_t              type;
};

struct Library
{
    struct Library_word*        lib_words;
    //struct Library_word_short* lib_words_short;
    size_t                      size;
    size_t                      capacity;
};

int read_library_file (FILE* library_file, struct Library* library);

size_t file_size_measure (FILE* const file_p);

int compress_data (char* string, unsigned char** buffer, size_t* buffer_size, struct Library* library);

int compressed_data_output (FILE* result_file, unsigned char* buffer, size_t buffer_size);

#endif // DATA_COMPRESSION_H_INCLUDED