#include <stdio.h>

FILE *input_file;
FILE *output_file;

// remove all whitespace, comments, and other unnecessary characters from the source code file using regular expressions
void clean_source_code()
{
    char c;
    while ((c = fgetc(input_file)) != EOF)
    {
        if (c == ' ' || c == '\t' || c == '\n') // whitespace
            continue;
        else if (c == '/') // comment
        {
            c = fgetc(input_file); // read next character
            if (c == '/')          // single line comment
            {
                while ((c = fgetc(input_file)) != '\n') // loop until end of line
                    continue;
            }
            else if (c == '*') // multi line comment
            {
                while (1) // loop until end of comment
                {
                    c = fgetc(input_file); // read next character
                    if (c == '*')          // check for end of comment
                    {
                        c = fgetc(input_file); // read next character
                        if (c == '/')          // check for end of comment
                            break;
                    }
                }
            }
            else
            {
                fprintf(output_file, "/");     // not a comment, so print the character
                fprintf(output_file, "%c", c); // print the character
            }
        }
        else
            fprintf(output_file, "%c", c); 
    }
}

int main(int argc, int *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }

    input_file = fopen(argv[1], "r");
    output_file = fopen(argv[2], "w");

    if (input_file == NULL)
    {
        printf("Error: Could not open input file %s\n", argv[1]);
        return 1;
    }

    if (output_file == NULL)
    {
        printf("Error: Could not open output file %s\n", argv[2]);
        return 1;
    }

    clean_source_code();

    fclose(input_file);
    fclose(output_file);

    return 0;
}
