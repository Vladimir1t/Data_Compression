#include "..\include\data_compression.h"

static FILE* error_file = fopen ("log\\file_error.txt", "w");

static int compressed_data_output (FILE* result_file, unsigned char* buffer, size_t buffer_size, size_t string_size);

static int get_data (FILE* sourse_file, char** str);

static void print_library (struct Library* library);

int main (int argc, char* argv[])
{
    if (argc < 3)
        fprintf (error_file, "<< too few command arguments >>\n");

    struct Library library     = {0};
    char*          string      = NULL;
    unsigned char* buffer      = NULL;
    size_t         buffer_size = 0;

    FOPEN (library_file, argv[1], "r");
    FOPEN (sourse_file, argv[2], "r");
    FOPEN (result_file, argv[3], "wb");

    read_library_file (library_file, &library);

    get_data (sourse_file, &string);

    compress_data (string, &buffer, &buffer_size, &library);

    printf ("size of library = %d\n", library.size);
    compressed_data_output (result_file, buffer, buffer_size, strlen (string));

    fclose (error_file);
    fclose (library_file);
    fclose (result_file);
    free (buffer);
    free (library.lib_words);

    return 0;
}

int get_data (FILE* sourse_file, char** string)
{
    CHECK_PTR (sourse_file);
    CHECK_PTR (string);

    size_t file_size = file_size_measure (sourse_file);
    //printf ("size of file: %d\n", file_size);

    CALLOC (*string, char, file_size + 1);
    size_t factual_size = fread (*string, sizeof (char), file_size, sourse_file);

    return SUCCESS;
}

int read_library_file (FILE* library_file, struct Library* library)
{
    CHECK_PTR (library_file);
    CHECK_PTR (library);

    library->capacity  = 30;
    library->size      = 0;
    CALLOC (library->lib_words, struct Library_word, library->capacity);

    size_t file_size = file_size_measure (library_file);
    //printf ("size of file: %d\n", file_size);
    char* text_data = NULL;
    CALLOC (text_data, char, file_size + 1);
    size_t factual_size = fread (text_data, sizeof (char), file_size, library_file);

    size_t ptr = 0;
    char* sep = "|\n";

    char* word = strtok (text_data, sep);
    while (word != NULL)
    {
        if (library->size == library->capacity)
        {
            library->capacity *= 2;
            REALLOC (library->lib_words, struct Library_word, library->capacity);
        }
        memcpy (library->lib_words[library->size].name, word, strlen (word));
        library->lib_words[library->size].word_num = library->size;
        library->size += 1;

        word = strtok (NULL, sep);
    }
    word = "\n";
    memcpy (library->lib_words[library->size].name, word, strlen (word));
    library->lib_words[library->size].word_num = library->size;
    library->size += 1;

    print_library (library);
    free (text_data);

    return SUCCESS;
}

int compress_data (char* string, unsigned char** buffer, size_t* buffer_size, struct Library* library)
{
    CHECK_PTR (string);
    CHECK_PTR (buffer);
    CHECK_PTR (buffer_size);
    CHECK_PTR (library);

    CALLOC (*buffer, unsigned char, 1000); //

    int ptr  = 0;
    int flag = 1;
    int j = 0;
    int num  = 0xDEAD;

    while (string[ptr] != '\0')  //
    {
        num  = 0xDEAD;
        flag = 1;
        j    = 0;

        char new_library_word[MAX_WORD_SIZE] = {0};
        char word[MAX_WORD_SIZE] = {0};

        new_library_word[j] = string[ptr];

        while (flag == 1 && string[ptr] != '\0')
        {
            word[j++] = string[ptr];

            for (int i = 0; i < library->size; i++)
            {
                flag = 0;
                if (!strcmp (word, library->lib_words[i].name))
                {
                    //printf ("<<%s>>\n", word);
                    new_library_word[j] = string[++ptr];
                    flag = 1;
                    num = library->lib_words[i].word_num;
                    break;
                }
            }
        }
        if (library->size == library->capacity)         // make capaity of the library bigger
        {
            library->capacity *= 2;
            REALLOC (library->lib_words, struct Library_word, library->capacity);
        }
        memcpy (library->lib_words[library->size].name, new_library_word, strlen (new_library_word));   // add new word in library
        library->lib_words[library->size].word_num= library->size;
        library->size += 1;

        if (num < MAX_ONE_BYTE_WORDS_NUM)   // add one_byte_word in library (max = 128 words)
        {
            (*(buffer))[*buffer_size] = (unsigned char) num;
            //printf ("num = %u (one byte code)\n", (*buffer)[*buffer_size]);
            *buffer_size += 1;
        }
        else if (num < MAX_LIBRARY_SIZE)    // add two_byte_word in library (max = 32767)
        {
            unsigned short mask     = 128;
            unsigned short add_mask = 32896;
            unsigned short zero     = 0;
            //printf ("num = %u (two byte code)\n", (num & mask));
            //unsigned short res = num & mask;
            if ((num & mask) == zero)
                num = num | add_mask;

            memcpy (*buffer + *buffer_size, &num, sizeof (unsigned short));

            *buffer_size += 2;
        }
    }

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

int compressed_data_output (FILE* result_file, unsigned char* buffer, size_t buffer_size, size_t string_size)
{
    CHECK_PTR (result_file);
    CHECK_PTR (buffer);

    printf ("Before %d bytes\n"
            "After  %d bytes\n", string_size, buffer_size);

    fwrite (buffer, sizeof(unsigned char), buffer_size, result_file);
    return SUCCESS;
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
