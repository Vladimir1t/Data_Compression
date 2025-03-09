#include "../include/data_compression.h"

static FILE* error_file = fopen ("log/file_error_decoder.txt", "w");

static void print_library (struct Library* library);

int main (int argc, char* argv[])
{
    if (argc < 3)
        fprintf (error_file, "<< too few command line arguments >>\n");

    struct Library library = {0};
    struct Buffer  buffer  = {0};

    FOPEN (library_file, argv[1], "r");
    FOPEN (sourse_file, argv[2], "rb");
    FOPEN (result_file, argv[3], "wb");

    read_library_file (library_file, &library);

    decode_data (sourse_file, &library, &buffer);

    decoded_data_output (result_file, &buffer);

    fclose (error_file);
    fclose (library_file);
    fclose (result_file);
    fclose (sourse_file);

    return 0;
}

int read_library_file (FILE* library_file, struct Library* library)
{
    CHECK_PTR (library_file);
    CHECK_PTR (library);

    library->capacity = 30;
    library->size     = 0;

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

int decode_data (FILE* sourse_file, struct Library* library, struct Buffer* buffer)
{
    CHECK_PTR (sourse_file);
    CHECK_PTR (library);
    CHECK_PTR (buffer);

    char* text_data = NULL;
    size_t file_size = file_size_measure (sourse_file);        
    CALLOC (text_data, char, file_size + 1);
    size_t factual_size = fread (text_data, sizeof (char), file_size, sourse_file);

    buffer->capacity = 1000;
    CALLOC (buffer->data, unsigned char, buffer->capacity);

    int ptr  = 0;
    int flag = 1;

    unsigned short num = 0xDEAD;
    char new_library_word[MAX_WORD_SIZE] = {0};
    char word[MAX_WORD_SIZE] = {0};

    while (ptr < factual_size)
    {
        DECODE_WORD (num, text_data[ptr]);

        if (buffer->size + MAX_WORD_SIZE > buffer->capacity)
            INCREASE_BUFFER_CAPACITY (buffer);

        if (num <= library->size)                               
        {
            strcat ((char*) buffer->data, library->lib_words[num].name);
            buffer->size += strlen (library->lib_words[num].name);
        }

        char new_library_word[MAX_WORD_SIZE] = {0};
        char word[MAX_WORD_SIZE]             = {0};
        char next_word[MAX_WORD_SIZE]        = {0};

        memcpy (new_library_word, library->lib_words[num].name, strlen (library->lib_words[num].name));
        memcpy (word, library->lib_words[num].name, strlen (library->lib_words[num].name));
        int n = strlen (library->lib_words[num].name);
        int j = 0;
        flag = 1;

        DECODE_NEXT_WORD (num, text_data[ptr]);
        memcpy (next_word, library->lib_words[num].name, strlen (library->lib_words[num].name));
        new_library_word[n] = next_word[j];

        while (flag == 1 && ptr < factual_size)
        {
            word[n++] = next_word[j];
            for (int i = 0; i < library->size; i++)
            {
                flag = 0;
                if (!strcmp (word, library->lib_words[i].name))
                {
                    new_library_word[n] = next_word[++j];
                    flag = 1;
                    break;
                }
            }
        }

        if (library->size == library->capacity)
            INCREASE_CAPACITY (library);

        if (library->size < MAX_LIBRARY_SIZE)
            ADD_IN_LIBRARY (new_library_word);
    }

    free (text_data);
    free (library->lib_words);

    return SUCCESS;
}

int decoded_data_output (FILE* result_file, struct Buffer* buffer)
{
    CHECK_PTR (result_file);
    CHECK_PTR (buffer);

    printf ("\n-------------- Decoded data -----------------\n\n");
    for (int i = 0; i < buffer->size; i++)
        printf ("%c", buffer->data[i]);
    printf ("\n\n");
    printf ("-- Also decoded data is located in 'file_decoded.txt' --\n");
    printf ("-- Size of a decoded file = %d byte --\n", buffer->size);
    printf ("-----------------------------------------------\n");

    fwrite (buffer->data, sizeof (unsigned char), buffer->size, result_file);

    free (buffer->data);

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
    printf ("------------------ Library ------------------\n\n");
    for (int i = 0; i < library->size; i++)
        printf ("[%s]", library->lib_words[i].name);

    printf ("\n");
    printf ("size of library = %d\n\n", library->size);
    printf ("---------------------------------------------\n");
}
