#include "..\include\data_compression.h"

static FILE* error_file = fopen ("..\\log\\file_error.txt", "w");

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


int main (int argc, char* argv[])
{
    if (argc < 3)
        fprintf (error_file, "<< too few command arguments >>\n");

    struct Library library     = {0};
    char           string[]    = "aaabaabbabbabbcbabbccddeytqtweytweutqwuetyytvestuqqqqrbxerttttttttetrcvvvvvvvvvvvvvvvcutnxxxxxxxxxxxxxieoiuwyzebrnycywexrywuryerxrghjagdjajkahtyuqtetvxwtetqzeuqwtxqqwutbzewnqoimexirpociwuroqunxerubyxerqzgggiexuasjdkuirouwefeworiuroweirpmmqweqwxasweqzsqeaqxztiuworpowpoqmrcbxewryqzweppppplmkiuhucgytyfwtdfywelecmqxkefmoqircqupxmrigecnrjkwmdiweiowiofuihfreirfhwoijdowieifehwuifdmcncxzhfhsdjakhuyeraduihjhfjhasdkjfhjksadhfiawehjasdhfjkhavbnabbbaeiopdnjbdahdkljfncbjdshklfjskdlnuiii";
    //iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiicsbcjsddljaiosxxaqrqredsgaehuldjfklgotldkvnmxbncbzmxnmnczmjskljdalsjdoepaiodsgjdgiriogdhghjghjgjhgbnhiuuphfjdhfjkhsjkdhsjkhdjhfskjfowiueuyiuyriqowoiuk";
    unsigned char* buffer      = NULL;
    size_t         buffer_size = 0;

    FOPEN (library_file, argv[1], "r");
    FOPEN (result_file, argv[2], "wb"); //

    read_library_file (library_file, &library);

    compress_data (string, &buffer, &buffer_size, &library);
    //int i = 0;
    /*for (int i = 0; i < buffer_size; i++)
    {
        printf (".." SPEC "..\n", buffer[i]);
        //i++;
    }*/
    printf ("size of library = %d\n", library.size);
    printf ("Before %d bytes\n"
            "After  %d bytes\n", strlen (string), buffer_size * sizeof (char));

    compressed_data_output (result_file, buffer, buffer_size);

    fclose (error_file);
    fclose (library_file);
    fclose (result_file);
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
    CALLOC (library->lib_words, struct Library_word, library->capacity);
    //library->lib_words = (struct Library_word*) calloc (library->capacity, sizeof (struct Library_word));

    size_t file_size = file_size_measure (library_file);            // measures the size of a text
    printf ("size of file: %d\n", file_size);

    char* text_data = NULL;
    CALLOC (text_data, char, file_size + 1);

    size_t factual_size = fread (text_data, sizeof (char), file_size, library_file);
   // for (int i = 0; i < file_size; i++)
    //printf (" %c \n", text_data[i]);
    if (factual_size != file_size)
        fprintf (error_file, "<< Error. Wrong size of data_file >>\n");

    size_t ptr = 0;
    char* sep = " ,\n";

    char* word = strtok (text_data, sep);
    while (word != NULL)
    {
        //printf ("lib size = %d\n", library->size);
        if (library->size == library->capacity)
        {
            library->capacity *= 2;
            REALLOC (library->lib_words, struct Library_word, library->capacity);
            //library->lib_words = (struct Library_word*) realloc (library->lib_words, library->capacity * sizeof (char));
            if (library->lib_words == NULL)
                return ERROR;
        }
        memcpy (library->lib_words[library->size].name, word, strlen (word) * sizeof (char));
        printf ("word = %s\n", library->lib_words[library->size].name);
        //printf ("word = %s\n", word);
        library->lib_words[library->size].type = one_byte;
        library->lib_words[library->size].word_num.one_byte_num = library->size;
        library->size += 1;

        word = strtok (NULL, sep);
    }

    for (int i = 0; i < library->size; i++)
        printf ("[%s]\n", library->lib_words[i].name);

    free (text_data);

    return SUCCESS;
}

int compress_data (char* string, unsigned char** buffer, size_t* buffer_size, struct Library* library)
{
    CHECK_PTR (string);
    CHECK_PTR (buffer);
    CHECK_PTR (buffer_size);
    CHECK_PTR (library);

    CALLOC (*buffer, unsigned char, 1000);

    int ptr  = 0;
    int flag = 1;
    int num  = 0xDEAD;
    int j = 0;

    while (string[ptr] != '\0')
    {
        num  = 0xDEAD;
        flag = 1;
        j    = 0;

        char word_1[MAX_WORD_SIZE] = {0};
        char word_2[MAX_WORD_SIZE] = {0};

        word_1[j] = string[ptr];

        while (flag == 1 && string[ptr] != '\0')
        {
            word_2[j++] = string[ptr];
            //printf ("[[%s]]\n", word_1);
            //j++;
            for (int i = 0; i < library->size; i++)
            {
                flag = 0;
                if (!strcmp (word_2, library->lib_words[i].name))
                {
                    printf ("<<%s>>\n", word_2);
                    //ptr++;
                    word_1[j] = string[++ptr];
                    flag = 1;
                    //if (library->lib_words[i].type == one_word)
                    if (library->lib_words[i].type == one_byte)
                    {
                        num = library->lib_words[i].word_num.one_byte_num;
                    }
                    else
                        num = library->lib_words[i].word_num.two_byte_num;
                    break;
                }

            }
        }
        if (library->size == library->capacity)
        {
            library->capacity *= 2;
            REALLOC (library->lib_words, struct Library_word, library->capacity);
        }
        memcpy (library->lib_words[library->size].name, word_1, strlen (word_1) * sizeof (char));
        if (library->size < MAX_ONE_BYTE_WORDS_NUM)
        {
            library->lib_words[library->size].word_num.one_byte_num = library->size;
            library->lib_words[library->size].type = one_byte;
            library->size += 1;
        }
        else
        {
            library->lib_words[library->size].word_num.two_byte_num = library->size;
            library->lib_words[library->size].type = two_byte;
            library->size += 1;
        }

        if (num < MAX_ONE_BYTE_WORDS_NUM)
        {
            (*(buffer))[*buffer_size] = (unsigned char) num;
            printf ("num = %u (one byte code)\n", (*buffer)[*buffer_size]);
            *buffer_size += 1;
        }
        else if (num < MAX_LIBRARY_SIZE)
        {
           // printf ("num = %u (two byte code)\n",  num);
            memcpy (*buffer + *buffer_size, &num, sizeof (unsigned short));
            printf ("num = %u (two byte code)\n",  *((unsigned short*) (*buffer + *buffer_size)));
            printf ("%u and %u\n",  (*(buffer))[*buffer_size],  (*(buffer))[*buffer_size + 1]);
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

int compressed_data_output (FILE* result_file, unsigned char* buffer, size_t buffer_size)
{
    CHECK_PTR (result_file);
    CHECK_PTR (buffer);

    //printf ("ok\n");
    fwrite (buffer, sizeof(unsigned char), buffer_size, result_file);
    return SUCCESS;
}
