/*
    COP 3401 Systems Software
    Lexical Analyzer
    Authored by Caleb Rivera and Matthew Labrada
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_IDENTIFIER_LENGTH 11
#define MAX_NUMBER_LENGTH 5
#define MAX_BUFFER_LENGTH 1000

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
    char lexeme[MAX_BUFFER_LENGTH + 1];
    char value[MAX_BUFFER_LENGTH + 1];
} token;

typedef struct
{
    token *tokens;
    int size;
    int capacity;
} list;

list *token_list;

char peekc()
{
    int c = getc(input_file);
    ungetc(c, input_file);
    return (char)c;
}

void print_source_code()
{
    char c;
    while ((c = fgetc(input_file)) != EOF)
        printf("%c", c);
    rewind(input_file);
}

void clear_to_index(char *str, int index)
{
    for (int i = 0; i < index; i++)
        str[i] = '\0';
}

// handle reserved words
int handle_reserved_word(char *buffer)
{
    if (strcmp(buffer, "const") == 0)
        return constsym;
    else if (strcmp(buffer, "var") == 0)
        return varsym;
    else if (strcmp(buffer, "procedure") == 0)
        return procsym;
    else if (strcmp(buffer, "call") == 0)
        return callsym;
    else if (strcmp(buffer, "begin") == 0)
        return beginsym;
    else if (strcmp(buffer, "end") == 0)
        return endsym;
    else if (strcmp(buffer, "if") == 0)
        return ifsym;
    else if (strcmp(buffer, "then") == 0)
        return thensym;
    else if (strcmp(buffer, "else") == 0)
        return elsesym;
    else if (strcmp(buffer, "while") == 0)
        return whilesym;
    else if (strcmp(buffer, "do") == 0)
        return dosym;
    else if (strcmp(buffer, "read") == 0)
        return readsym;
    else if (strcmp(buffer, "write") == 0)
        return writesym;
    return 0; // invalid reserved word
}

// handle special symbols
int handle_special_symbol(char *buffer)
{
    if (strcmp(buffer, "+") == 0)
        return plussym;
    else if (strcmp(buffer, "-") == 0)
        return minussym;
    else if (strcmp(buffer, "*") == 0)
        return multsym;
    else if (strcmp(buffer, "/") == 0)
        return slashsym;
    else if (strcmp(buffer, "(") == 0)
        return lparentsym;
    else if (strcmp(buffer, ")") == 0)
        return rparentsym;
    else if (strcmp(buffer, ",") == 0)
        return commasym;
    else if (strcmp(buffer, ";") == 0)
        return semicolonsym;
    else if (strcmp(buffer, ".") == 0)
        return periodsym;
    else if (strcmp(buffer, "=") == 0)
        return eqsym;
    else if (strcmp(buffer, "<") == 0)
        return lessym;
    else if (strcmp(buffer, ">") == 0)
        return gtrsym;
    else if (strcmp(buffer, ":=") == 0)
        return becomessym;
    else if (strcmp(buffer, "<=") == 0)
        return leqsym;
    else if (strcmp(buffer, ">=") == 0)
        return geqsym;
    else if (strcmp(buffer, "<>") == 0)
        return neqsym;
    return 0; // invalid special symbol
}

// check if character is a special symbol
int is_special_symbol(char c)
{
    return (c == '+' ||
            c == '-' ||
            c == '*' ||
            c == '/' ||
            c == '(' ||
            c == ')' ||
            c == '=' ||
            c == ',' ||
            c == '.' ||
            c == '<' ||
            c == '>' ||
            c == ':' ||
            c == ';' ||
            c == '&');
}

// create a list of tokens
list *create_list()
{
    list *l = malloc(sizeof(list));
    l->size = 0;
    l->capacity = 10;
    l->tokens = malloc(sizeof(token) * l->capacity);
    return l;
}

// destroy a list of tokens
list *destroy_list(list *l)
{
    free(l->tokens);
    free(l);
    return NULL;
}

// append a token to a list
list *append_token(list *l, token t)
{
    if (l->size == l->capacity)
    {
        l->capacity *= 2;
        l->tokens = realloc(l->tokens, sizeof(token) * l->capacity);
    }
    l->tokens[l->size++] = t;
    return l;
}

// add a token to a list
void add_token(list *l, token t)
{
    if (l->size == l->capacity)
    {
        l->capacity *= 2;
        l->tokens = realloc(l->tokens, sizeof(token) * l->capacity);
    }
    l->tokens[l->size++] = t;
}

// print a lexeme table
void print_lexeme_table(list *l)
{

    for (int i = 0; i < l->size; i++)
        printf("%10s %20s\n", l->tokens[i].lexeme, l->tokens[i].value);
}

// print a list of tokens
void print_tokens(list *l)
{
    for (int i = 0; i < l->size; i++)
    {
        printf("%s ", l->tokens[i].value);
        char identifier_value[3] = {0}, number_value[3] = {0};
        sprintf(identifier_value, "%d", identsym);
        sprintf(number_value, "%d", numbersym);
        if (strcmp(l->tokens[i].value, identifier_value) == 0 || strcmp(l->tokens[i].value, number_value) == 0)
            printf("%s ", l->tokens[i].lexeme);
    }
    printf("\n");
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

    printf("Source Program:\n");
    print_source_code();
    printf("\n");
    printf("Lexeme Table:\n");

    token_list = create_list();

    char c;
    char buffer[MAX_BUFFER_LENGTH + 1] = {0};
    int buffer_index = 0;
    while ((c = fgetc(input_file)) != EOF)
    {
        if (iscntrl(c) || isspace(c)) // skip control characters and whitespace
        {
            c = fgetc(input_file);
        }
        if (isdigit(c)) // handle numbers
        {
            buffer[buffer_index++] = c;
            while (1)
            {

                char nextc = peekc();
                if (isspace(nextc) || is_special_symbol(nextc))
                {
                    token t;
                    if (buffer_index > MAX_NUMBER_LENGTH)
                    {
                        printf("%10s %20s", buffer, "ERROR: NUMBER TOO LONG");
                    }
                    else
                    {
                        printf("%10s %20d\n", buffer, numbersym);
                        sprintf(t.value, "%d", numbersym);
                        strcpy(t.lexeme, buffer);
                        append_token(token_list, t);
                    }

                    clear_to_index(buffer, buffer_index);
                    buffer_index = 0;
                    break;
                }
                else if (isdigit(nextc))
                {
                    c = getc(input_file);
                    buffer[buffer_index++] = c;
                }
                else if (nextc == EOF)
                    break;
                else if (isalpha(nextc))
                {

                    printf("%10s %20d\n", buffer, numbersym);
                    clear_to_index(buffer, buffer_index);
                    buffer_index = 0;
                    break;
                }
            }
        }
        else if (isalpha(c)) // handle identifiers and reserved words
        {

            buffer[buffer_index++] = c;
            while (1)
            {
                char nextc = peekc();
                if (isspace(nextc) || is_special_symbol(nextc) || nextc == EOF)
                {
                    // check if reserved word
                    int token_value = handle_reserved_word(buffer);
                    if (token_value)
                    {
                        token t;
                        printf("%10s %20d\n", buffer, token_value);
                        sprintf(t.value, "%d", token_value);
                        strcpy(t.lexeme, buffer);
                        append_token(token_list, t);
                        clear_to_index(buffer, buffer_index);
                        buffer_index = 0;
                        break;
                    }
                    else // identifier
                    {
                        token t;
                        if (buffer_index > MAX_IDENTIFIER_LENGTH)
                        {
                            printf("%10s %20s\n", buffer, "ERROR: IDENTIFIER TOO LONG");
                        }
                        else
                        {
                            printf("%10s %20d\n", buffer, identsym);
                            sprintf(t.value, "%d", identsym);
                            strcpy(t.lexeme, buffer);
                            append_token(token_list, t);
                        }

                        clear_to_index(buffer, buffer_index);
                        buffer_index = 0;
                        break;
                    }
                    break;
                }
                else if (isalnum(nextc))
                {
                    c = getc(input_file);
                    buffer[buffer_index++] = c;
                }
            }
        }
        else if (is_special_symbol(c)) // handle special symbols
        {

            buffer[buffer_index++] = c;
            char nextc = peekc();

            if (is_special_symbol(nextc))
            {
                // handle block comments
                if (c == '/' && nextc == '*')
                {
                    clear_to_index(buffer, buffer_index);
                    buffer_index = 0;
                    while (1)
                    {
                        c = getc(input_file);
                        nextc = peekc();
                        if (c == '*' && nextc == '/')
                        {
                            c = getc(input_file);
                            break;
                        }
                    }
                    continue;
                }

                // handle single line comments
                if (c == '/' && nextc == '/')
                {
                    clear_to_index(buffer, buffer_index);
                    buffer_index = 0;
                    while (1)
                    {
                        c = getc(input_file);
                        nextc = peekc();
                        if (c == '\n')
                        {
                            break;
                        }
                    }
                    continue;
                }

                c = getc(input_file);
                buffer[buffer_index++] = c;
                token t;
                int token_value = handle_special_symbol(buffer);
                if (!token_value)
                    printf("%10s %20s\n", buffer, "ERROR: INVALID SYMBOL");
                else
                    printf("%10s %20d\n", buffer, token_value);
                sprintf(t.value, "%d", token_value);
                strcpy(t.lexeme, buffer);
                append_token(token_list, t);
                clear_to_index(buffer, buffer_index);
                buffer_index = 0;
            }
            else
            {
                token t;
                int token_value = handle_special_symbol(buffer);
                if (!token_value)
                    printf("%10s %20s\n", buffer, "ERROR: INVALID SYMBOL");
                else
                    printf("%10s %20d\n", buffer, token_value);
                sprintf(t.value, "%d", token_value);
                strcpy(t.lexeme, buffer);
                append_token(token_list, t);
                clear_to_index(buffer, buffer_index);
                buffer_index = 0;
            }
        }
    }

    printf("\n\n");
    printf("Token List:\n");
    print_tokens(token_list);
    destroy_list(token_list);
    return 0;
}