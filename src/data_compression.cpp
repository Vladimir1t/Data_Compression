#include "../include/data_compression.h"

static FILE* error_file = fopen ("log/file_error.txt", "w");

static int  compressed_data_output (FILE* result_file, struct Buffer* buffer, size_t string_size);
static int  get_data               (FILE* sourse_file, char** data);
static void print_library          (struct Library* library);

int main (int argc, char* argv[])
{
    if (argc < 3)
        fprintf (error_file, "<< too few command arguments >>\n");

    struct Library library     = {0};
    char*          data        = NULL;
    int            data_size   = 0;
    struct Buffer  buffer      = {0};

    FOPEN (library_file, argv[1], "r");
    FOPEN (sourse_file, argv[2], "r");
    FOPEN (result_file, argv[3], "wb");

    read_library_file (library_file, &library);

    data_size = get_data (sourse_file, &data);

    compress_data (data, &buffer, &library);

    printf ("New size of library = %d\n", library.size);
    compressed_data_output (result_file, &buffer, data_size);

    fclose (error_file);
    fclose (library_file);
    fclose (result_file);

    free (buffer.data);
    free (library.lib_words);

    return 0;
}

int get_data (FILE* sourse_file, char** data)
{
    CHECK_PTR (sourse_file);
    CHECK_PTR (data);

    size_t file_size = file_size_measure (sourse_file);
    //printf ("size of file: %d\n", file_size);

    CALLOC (*data, char, file_size + 1);
    size_t factual_size = fread (*data, sizeof (char), file_size, sourse_file);

    return factual_size;
}

int read_library_file (FILE* library_file, struct Library* library)
{
    CHECK_PTR (library_file);
    CHECK_PTR (library);

    library->capacity  = 30;
    library->size      = 0;
    CALLOC (library->lib_words, struct Library_word, library->capacity);

    size_t file_size = file_size_measure (library_file);
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

int compress_data (char* string, struct Buffer* buffer, struct Library* library)
{
    CHECK_PTR (string);
    CHECK_PTR (buffer);
    CHECK_PTR (library);

    buffer->capacity = 1000;
    CALLOC (buffer->data, unsigned char, buffer->capacity);

    int ptr  = 0;
    int flag = 1;
    int j    = 0;
    int num  = 0xDEAD;

    while (string[ptr] != '\0')
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
                    new_library_word[j] = string[++ptr];
                    flag = 1;
                    num = library->lib_words[i].word_num;
                    break;
                }
            }
        }
        if (buffer->size + MAX_WORD_SIZE > buffer->capacity)
            INCREASE_BUFFER_CAPACITY (buffer);

        if (library->size == library->capacity)
            INCREASE_CAPACITY (library);

        if (library->size < MAX_LIBRARY_SIZE)
            ADD_IN_LIBRARY (new_library_word);

        if (num < MAX_ONE_BYTE_WORDS_NUM)                               // add one_byte_word in library (0 - 127)
            ENCODE_WORD_AS_ONE_BYTE (num)

        if (MAX_ONE_BYTE_WORDS_NUM <= num && num < MAX_LIBRARY_SIZE)    // add two_byte_word in library (128 - 32767)
            ENCODE_WORD_AS_TWO_BYTES (num)
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

int compressed_data_output (FILE* result_file, struct Buffer* buffer, size_t string_size)
{
    CHECK_PTR (result_file);
    CHECK_PTR (buffer);

    printf ("\n-- Size before %d bytes --\n"
            "--      after  %d bytes --\n\n", string_size, buffer->size);

    fwrite (buffer->data, sizeof (unsigned char), buffer->size, result_file);

    return SUCCESS;
}

void print_library (struct Library* library)
{
    printf ("----------------- Library -----------------\n\n");
    for (int i = 0; i < library->size; i++)
        printf ("[%s]", library->lib_words[i].name);

    printf ("\n");
    printf ("Size of library = %d\n\n", library->size);
    printf ("-------------------------------------------\n");
}
