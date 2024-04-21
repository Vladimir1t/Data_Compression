#include "..\include\data_compression.h"

static FILE* error_file = fopen ("log\\file_error_decoder.txt", "w");

static void print_library (struct Library* library);

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

// надо сделать декодер, читалку из файла
// генератор слов
//
int main (int argc, char* argv[])
{
    if (argc < 3)
        fprintf (error_file, "<< too few command arguments >>\n");

    struct Library library     = {0};
    unsigned char* buffer      = NULL;
    size_t         buffer_size = 0;

    FOPEN (library_file, argv[1], "r");
    FOPEN (sourse_file, argv[2], "rb");
    FOPEN (result_file, argv[3], "wb");//

    read_library_file (library_file, &library);

    decode_data (sourse_file, &library, &buffer, &buffer_size);

    decoded_data_output (result_file, buffer, buffer_size);

    fclose (error_file);
    fclose (library_file);
    fclose (result_file);
    fclose (sourse_file);
    free (buffer);
    free (library.lib_words);

    return 0;
}

int read_library_file (FILE* library_file, struct Library* library)
{
    CHECK_PTR (library_file);
    CHECK_PTR (library);

    library->capacity  = 30;
    library->size      = 0;

    char* text_data = NULL;
    CALLOC (library->lib_words, struct Library_word, library->capacity);
    size_t file_size = file_size_measure (library_file);
    printf ("size of file: %d\n", file_size);
    CALLOC (text_data, char, file_size + 1);
    size_t factual_size = fread (text_data, sizeof (char), file_size, library_file);

    size_t ptr = 0;
    const char* sep = "|\n";

    char* word = strtok (text_data, sep);
    while (word != NULL)
    {
        if (library->size == library->capacity)
        {
            library->capacity *= 2;
            REALLOC (library->lib_words, struct Library_word, library->capacity);
        }
        memcpy (library->lib_words[library->size].name, word, strlen (word));
        //library->lib_words[library->size].type = one_byte;
        library->lib_words[library->size].word_num = library->size;
        library->size += 1;

        word = strtok (NULL, sep);
    }
    word = "\n";
    memcpy (library->lib_words[library->size].name, word, strlen (word));
    //library->lib_words[library->size].type = one_byte;
    library->lib_words[library->size].word_num = library->size;
    library->size += 1;
    print_library (library);

    free (text_data);

    return SUCCESS;
}
// 0001 0000 00001 = 257
int decode_data (FILE* sourse_file, struct Library* library, unsigned char** buffer, size_t* buffer_size)
{
    CHECK_PTR (sourse_file);
    CHECK_PTR (library);
    CHECK_PTR (buffer_size);

    char* text_data = NULL;
    size_t file_size = file_size_measure (sourse_file);            // measures the size of a text
    //printf ("size of file: %d\n", file_size);
    CALLOC (text_data, char, file_size + 1);
    size_t factual_size = fread (text_data, sizeof (char), file_size, sourse_file);

    CALLOC (*buffer, unsigned char, 1000);

    int ptr = 0;
    int flag = 1;

    unsigned short num = 0xDEAD;
    char new_library_word[MAX_WORD_SIZE] = {0};
    char word[MAX_WORD_SIZE] = {0};
    //printf ("[[%d]]\n", MASK);

    while (ptr < factual_size)                    // $ - end of file
    {
        unsigned char type_of_symbol = text_data[ptr] & MASK;
        //printf ("type_of_symbol = %u\n", type_of_symbol);
        if (type_of_symbol == ONE_BYTE_WORD)
        {
            //printf ("1\n");
            num = text_data[ptr];
            //printf ("num = %u.\n", num);
            ptr++;
        }
        else
        {
            unsigned short mask = 32896;

            num = *((unsigned short*) (text_data + ptr));
            //printf ("before num = %u\n", num);
            unsigned short res = num & mask;
            if (res == mask)
                num = num ^ mask;

            //printf ("num = %u\n", num);
            ptr += 2;
        }
        //printf ("lb size = %d\n", library->size);
        if (num <= library->size)
        {
            //printf ("- %s\n", library->lib_words[num].name);
            strcat ((char*) *buffer, library->lib_words[num].name);
            *buffer_size += strlen (library->lib_words[num].name);
        }

        char new_library_word[MAX_WORD_SIZE] = {0};
        char word[MAX_WORD_SIZE]             = {0};
        char next_word[MAX_WORD_SIZE]        = {0};

        memcpy (new_library_word, library->lib_words[num].name, strlen (library->lib_words[num].name));
        memcpy (word, library->lib_words[num].name, strlen (library->lib_words[num].name));
        int n = strlen (library->lib_words[num].name);
        int j = 0;
        flag = 1;

        if ((text_data[ptr] & MASK) == ONE_BYTE_WORD)
        {
            num = text_data[ptr];
            //printf ("'num' = %u\n", num);
        }
        else
        {
            unsigned short mask = 32896;
            num = *((unsigned short*) (text_data + ptr));
            //printf ("before num = %u\n", num);
            unsigned short res = num & mask;
            if (res == mask)
                num = num ^ mask;

            //printf ("num = %u\n", num);
        }
        memcpy (next_word, library->lib_words[num].name, strlen (library->lib_words[num].name));
        new_library_word[n] = next_word[j];
        while (flag == 1 && ptr < factual_size)
        {
            word[n++] = next_word[j];
            //printf ("[%s]\n", word);
            for (int i = 0; i < library->size; i++)
            {
                flag = 0;
                if (!strcmp (word, library->lib_words[i].name))
                {

                    new_library_word[n] = next_word[++j];
                    //printf ("<<%s>>\n", new_library_word);
                    flag = 1;
                    break;
                }
            }
        }
        if (library->size == library->capacity)
        {
            library->capacity *= 2;
            REALLOC (library->lib_words, struct Library_word, library->capacity);
        }
        memcpy (library->lib_words[library->size].name, new_library_word, strlen (new_library_word));   // add new word in library
        library->lib_words[library->size].word_num = library->size;
        library->size += 1;
    }

    free (text_data);

    return SUCCESS;
}

int decoded_data_output (FILE* result_file, unsigned char* buffer, size_t buffer_size)
{
    CHECK_PTR (result_file);
    CHECK_PTR (buffer);
    printf ("\n--- Decoded data ---\n\n");
    for (int i = 0; i < buffer_size; i++)
        printf ("%c", buffer[i]);
    printf ("\n\n");
    printf ("Also this decoded data is located in 'file_decoded.txt'");
    printf ("\n--------------------\n");
    fwrite (buffer, sizeof (unsigned char), buffer_size, result_file);

    return SUCCESS;
}

size_t file_size_measure (FILE* const file_p)
{
    CHECK_PTR (file_p);

    int start_position = ftell (file_p);
    fseek (file_p, 0, SEEK_END);
    size_t len = (size_t) ftell (file_p);

    fseek (file_p, start_position, SEEK_SET);

    return len;
}

void print_library (struct Library* library)
{
    printf ("----------------- Library -----------------\n\n");
    for (int i = 0; i < library->size; i++)
        printf ("[%s]", library->lib_words[i].name);

    printf ("\n");
    printf ("size of library = %d\n\n", library->size);
    printf ("-------------------------------------------\n");
}
