#ifndef LIBRARY_OF_DEFINES_H_INCLUDED
#define LIBRARY_OF_DEFINES_H_INCLUDED

#define CHECK_PTR(ptr)                              \
    if (ptr == NULL)                                \
    {                                               \
        fprintf (error_file, "<< NULL ptr >>\n");   \
        return NULL_PTR;                            \
    }

#define REALLOC(elem, type, size)                            \
    do                                                       \
    {                                                        \
        void* clTmp = realloc (elem, size * sizeof (type));  \
        if (clTmp == NULL)                                   \
            return REALLOC_ERROR;                            \
        elem = (type*) clTmp;                                \
    }                                                        \
    while (0)

#define CALLOC(elem, type, size)                     \
    do                                               \
    {                                                \
        void* clTmp = calloc (size, sizeof (type));  \
        if (clTmp == NULL)                           \
            return CALLOC_ERROR;                     \
        elem = (type*) clTmp;                        \
    }                                                \
    while (0)

#define FOPEN(ptr, name, mode)                               \
    FILE* ptr = fopen (name, mode);                          \
    if (ptr == NULL)                                         \
    {                                                        \
        fprintf (error_file, "<< file wasn't read >>\n");    \
        return OPEN_ERROR;                                   \
    }

#define INCREASE_CAPACITY(elem)                                                                         \
    {                                                                                                   \
        library->capacity *= 2;                                                                         \
        REALLOC (library->lib_words, struct Library_word, library->capacity);                           \
    }

#define INCREASE_BUFFER_CAPACITY(elem)                                                                   \
    {                                                                                                    \
        (buffer->capacity) *= 2;                                                                         \
        REALLOC (buffer->data, unsigned char, buffer->capacity);                                         \
    }

#define ADD_IN_LIBRARY(new_library_word)                                                                    \
    {                                                                                                       \
        memcpy (library->lib_words[library->size].name, new_library_word, strlen (new_library_word));       \
        library->lib_words[library->size].word_num = library->size;                                         \
        library->size += 1;                                                                                 \
    }

#define ENCODE_WORD_AS_ONE_BYTE(num)                                                  \
    {                                                                                 \
        (buffer->data)[buffer->size] = (unsigned char) num;                           \
        buffer->size += 1;                                                            \
    }                                                                                 \

#define ENCODE_WORD_AS_TWO_BYTES(num)                                                 \
    {                                                                                 \
        unsigned short mask     = 128;        /*0000 0000 1000 0000*/                 \
        unsigned short add_mask = 32896;      /*1000 0000 1000 0000*/                 \
        unsigned short zero     = 0;                                                  \
        if ((num & mask) == zero)                                                     \
            num = num | add_mask;                                                     \
        memcpy (buffer->data + buffer->size, &num, sizeof (unsigned short));          \
        buffer->size += 2;                                                            \
    }

#define DECODE_WORD(num, ...)                                       \
    if ((text_data[ptr] & MASK) == ONE_BYTE_WORD)                   \
        num = text_data[ptr++];                                     \
    else                                                            \
    {                                                               \
        unsigned short mask = 32896;     /*1000 0000 1000 0000*/    \
        num = *((unsigned short*) (text_data + ptr));               \
        unsigned short res = num & mask;                            \
        if (res == mask)                                            \
            num = num ^ mask;                                       \
        ptr += 2;                                                   \
    }

#define DECODE_NEXT_WORD(num, ...)                                  \
    if ((text_data[ptr] & MASK) == ONE_BYTE_WORD)                   \
        num = text_data[ptr];                                       \
    else                                                            \
    {                                                               \
        unsigned short mask = 32896;     /*1000 0000 1000 0000*/    \
        num = *((unsigned short*) (text_data + ptr));               \
        unsigned short res = num & mask;                            \
        if (res == mask)                                            \
            num = num ^ mask;                                       \
    }

#endif // LIBRARY_OF_DEFINES_H_INCLUDED
