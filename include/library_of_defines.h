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

#endif // LIBRARY_OF_DEFINES_H_INCLUDED
