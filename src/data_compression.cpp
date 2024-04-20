#include "..\include\data_compression.h"

static FILE* error_file = fopen ("log\\file_error.txt", "w");

static int compressed_data_output (FILE* result_file, unsigned char* buffer, size_t buffer_size, size_t string_size);

int main (int argc, char* argv[])
{
    if (argc < 3)
        fprintf (error_file, "<< too few command arguments >>\n");

    struct Library library     = {0};
    char           string[]    = "hello my friend how are you, i am fine, do you hear that great music, i love it very much, i have a dog jack and mom and father. english texts for beginners to practice reading and comprehension online and for free. practicing your comprehension of written english will both improve your vocabulary and understanding of grammar and word . order the texts below are designed to help you develop while giving you an instant evaluation of program.tqtweytweutqwuetyytvestuqqqqrbxerttttttttetrcvvvvvvvvvvvvvvvcutnxxxxxxxxxxxxxieoiuwyzebrnycywexrywuryerxrghjagdjajkahtyuqtetvxwtetqzeuqwtxqqwutbzewnqoimexirpociwuroqunxerubyxerqzgggiexuasjdkuirouwefeworiuroweirpmmqweqwxasweqzsqeaqxztiuworpowpoqmrcbxewryqzweppppplmkiuhucgytyfwtdfywelecmqxkefmoqircqupxmrigecnrjkwmdiweiowiofuihfreirfhwoijdowieifehwuifdmcncxzhfhsdjakhuyeraduihjhfjhasdkjfhjksadhfiawehjasdhfjkhavbnabbbaeiopdnjbdahdkljfnc";
    //bjdshklfjskdlnuiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiicsbcjsddljaiosxxaqrqredsgaehuldjfklgotldkvnmxbncbzmxnmnczmjskljdalsjdoepaiodsgjdgiriogdhghjghjgjhgbnhiuuphfjdhfjkhsjkdhsjkhdjhfskjfowiueuyiuyriqowoiuk";
    unsigned char* buffer      = NULL;
    size_t         buffer_size = 0;

    FOPEN (library_file, argv[1], "r");
    FOPEN (result_file, argv[2], "wb");

    read_library_file (library_file, &library);

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

int read_library_file (FILE* library_file, struct Library* library)
{
    CHECK_PTR (library_file);
    CHECK_PTR (library);

    library->capacity  = 30;
    library->size      = 0;
    CALLOC (library->lib_words, struct Library_word, library->capacity);

    size_t file_size = file_size_measure (library_file);            // measures the size of a text
    printf ("size of file: %d\n", file_size);
    char* text_data = NULL;
    CALLOC (text_data, char, file_size + 1);
    size_t factual_size = fread (text_data, sizeof (char), file_size, library_file);

    size_t ptr = 0;
    char* sep = "|\n";

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
        //printf ("word = %s\n", library->lib_words[library->size].name);
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

        char new_library_word[MAX_WORD_SIZE] = {0};
        char word[MAX_WORD_SIZE] = {0};

        new_library_word[j] = string[ptr];

        while (flag == 1 && string[ptr] != '\0')
        {
            word[j++] = string[ptr];
            //printf ("[[%s]]\n", word_1);
            //j++;
            for (int i = 0; i < library->size; i++)
            {
                flag = 0;
                if (!strcmp (word, library->lib_words[i].name))
                {
                    printf ("<<%s>>\n", word);
                    //ptr++;
                    new_library_word[j] = string[++ptr];
                    flag = 1;
                    //if (library->lib_words[i].type == one_word)
                    if (library->lib_words[i].type == one_byte)
                        num = library->lib_words[i].word_num.one_byte_num;
                    else
                        num = library->lib_words[i].word_num.two_byte_num;
                    break;
                }
            }
        }
        if (library->size == library->capacity)         // make library bigger
        {
            library->capacity *= 2;
            REALLOC (library->lib_words, struct Library_word, library->capacity);
        }
        memcpy (library->lib_words[library->size].name, new_library_word, strlen (new_library_word) * sizeof (char));   // add new word in library
        if (library->size < MAX_ONE_BYTE_WORDS_NUM)
        {
            library->lib_words[library->size].word_num.one_byte_num = library->size;
            library->lib_words[library->size].type = one_byte;
        }
        else
        {
            library->lib_words[library->size].word_num.two_byte_num = library->size;
            library->lib_words[library->size].type = two_byte;
        }
        library->size += 1;

        if (num < MAX_ONE_BYTE_WORDS_NUM)   // add one_byte_word in library
        {
            (*(buffer))[*buffer_size] = (unsigned char) num;
            printf ("num = %u (one byte code)\n", (*buffer)[*buffer_size]);
            *buffer_size += 1;
        }
        else if (num < MAX_LIBRARY_SIZE)    // add two_byte_word in library // до 32...
        {
            unsigned short mask = 128;
            unsigned short add = 32896;
            unsigned short zero = 0;
            printf ("num = %u (two byte code)\n",  (num & mask));
            unsigned short res = num & mask;
            if (res == zero)
                num = num | add;
            //num += 128;
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

int compressed_data_output (FILE* result_file, unsigned char* buffer, size_t buffer_size, size_t string_size)
{
    CHECK_PTR (result_file);
    CHECK_PTR (buffer);

    //printf ("size of library = %d\n", library->size);
    printf ("Before %d bytes\n"
            "After  %d bytes\n", string_size, buffer_size);

    fwrite (buffer, sizeof(unsigned char), buffer_size, result_file);
    //fprintf (result_file, "$$");
    return SUCCESS;
}
