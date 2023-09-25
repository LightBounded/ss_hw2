#include <stdio.h>
#include <string.h>

#define MAX_IDENTIFIER_LENGTH 11
#define MAX_NUMBER_LENGTH 5

FILE *input_file;
FILE *output_file;

typedef enum
{
    skipsym = 1,
    identsym,
    numbersym,
    plussym,
    minussym,
    multsym,
    slashsym,
    ifelsym,
    eqsym,
    neqsym,
    lessym,
    leqsym,
    gtrsym,
    geqsym,
    lparentsym,
    rparentsym,
    commasym,
    semicolonsym,
    periodsym,
    becomessym,
    beginsym,
    endsym,
    ifsym,
    thensym,
    whilesym,
    dosym,
    callsym,
    constsym,
    varsym,
    procsym,
    writesym,
    readsym,
    elsesym
} token_type;

typedef struct
{
    char lexeme[MAX_IDENTIFIER_LENGTH];
    token_type token;
} token;

int is_reserved_word(char *buffer)
{
    char reserved_words[14][10] = {
        "const",
        "var",
        "procedure",
        "call",
        "begin",
        "end",
        "if",
        "then",
        "ifel",
        "else"
        "while",
        "do",
        "read",
        "write",
    };

    for (int i = 0; i < 15; i++)
    {
        if (strcmp(reserved_words[i], buffer) == 0)
        {
            return 1;
        }
    }

    return 0;
}

// return token value
int handle_special_symbol(char *buffer)
{
    if (strcmp(buffer, "+"))
        return 4; // plussym
    else if (strcmp(buffer, "-"))
        return 5; // minussym
    else if (strcmp(buffer, "*"))
        return 6; // multsym
    else if (strcmp(buffer, "/"))
        return 7; // slashsym
    else if (strcmp(buffer, "("))
        return 15; // lparentsym
    else if (strcmp(buffer, ")"))
        return 16; // rparentsym
    else if (strcmp(buffer, ","))
        return 17; // commasym
    else if (strcmp(buffer, ";"))
        return 18; // semicolonsym
    else if (strcmp(buffer, "."))
        return 19; // periodsym
    else if (strcmp(buffer, "="))
        return 9; // eqsym
    else if (strcmp(buffer, "<"))
        return 21; // lessym
    else if (strcmp(buffer, ">"))
        return 11; // gtrsym
    else if (strcmp(buffer, ":="))
        return 20; // becomessym
    else if (strcmp(buffer, "<="))
        return 12; // leqsym
    else if (strcmp(buffer, ">="))
        return 14; // geqsym
    return 0;
}

int main(int argc, char *argv[])
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

    fclose(input_file);
    fclose(output_file);

    return 0;
}