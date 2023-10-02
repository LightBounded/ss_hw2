/* Pharit Smitasin
 * COP 3402 Fall 2023
 * HW2
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

// These defined values are the max lengths for identifiers, numbers, and tokens
#define ID_LEN_MAX 11
#define NUM_LEN_MAX 5
#define TOKEN_LEN_MAX 1000

FILE *inputFile;
FILE *outputFile;

// list of enumerations for token types
// with skipsym starting at 1, the rest of the enumerations will be assigned values incrementing by 1
typedef enum
{
    skipsym = 1,  //  skip symbol
    identsym,     //  identifier symbol
    numbersym,    //  number symbol
    plussym,      //  '+' symbol
    minussym,     //  '-' symbol
    multsym,      //  '*' symbol
    slashsym,     //  '/' symbol
    ifelsym,      //  'if', 'else' keyword
    eqsym,        //  '=' symbol
    neqsym,       //  '<>' symbol
    lessym,       //  '<' symbol
    leqsym,       //  '<=' symbol
    gtrsym,       //  '>' symbol
    geqsym,       //  '>=' symbol
    lparentsym,   //  '(' symbol
    rparentsym,   //  ')' symbol
    commasym,     //  ',' symbol
    semicolonsym, //  ';' symbol
    periodsym,    //  '.' symbol
    becomessym,   //  ':=' symbol
    beginsym,     //  'begin' keyword
    endsym,       //  'end' keyword
    ifsym,        //  'if' keyword
    thensym,      //  'then' keyword
    whilesym,     //  'while' keyword
    dosym,        //  'do' keyword
    callsym,      //  'call' keyword
    constsym,     //  'const' keyword
    varsym,       //  'var' keyword
    procsym,      //  'procedure' keyword
    writesym,     //  'write' keyword
    readsym,      //  'read' keyword
    elsesym       //  'else' keyword
} token_type;

// token struct
typedef struct
{
    char value[TOKEN_LEN_MAX + 1];
    char lexeme[TOKEN_LEN_MAX + 1];
} token;

// list of tokens struct
typedef struct
{
    token *tokens;
    int size;
    int capacity;
} list;

// this is the list of tokens that will be used to store the tokens for the parser
list *token_list;

// reads next character from input file
char peekc()
{
    int nextChar = getc(inputFile);
    ungetc(nextChar, inputFile);

    return (char)nextChar;
}

// prints output to console and output file
void printOutput(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    int result = vprintf(format, args);
    va_end(args);

    va_start(args, format);
    int fileResult = vfprintf(outputFile, format, args);
    va_end(args);

    if (result < 0 || fileResult < 0)
    {
        printf("Error occurred while writing to output file.\n");
    }
}

// this prints the original to the console above the lexeme and in the the output file
void printOriginal()
{
    char buffer[1024];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0)
    {
        fwrite(buffer, 1, bytesRead, stdout);
        fwrite(buffer, 1, bytesRead, outputFile);
    }

    rewind(inputFile); // Reset file pointer to the beginning of the file
}

// sets chars in the specified string to null up to the parameter index using '\0'
void cutString(char *string, int cutoff)
{
    for (int i = 0; i < cutoff; i++)
    {
        string[i] = '\0';
    }
}

// checks if the given string is a reserved word and returns its token value
// in = reserved word, out = token value
int reservedToToken(char *buffer)
{
    char *reserved_words[] = {"const", "var", "procedure", "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write", "ifel"};
    int reserved_word_values[] = {constsym, varsym, procsym, callsym, beginsym, endsym, ifsym, thensym, elsesym, whilesym, dosym, readsym, writesym, ifelsym};
    int num_reserved_words = sizeof(reserved_words) / sizeof(reserved_words[0]);

    // loop through reserved words and check if buffer matches any of them
    for (int i = 0; i < num_reserved_words; i++)
    {
        if (strcmp(buffer, reserved_words[i]) == 0)
        {
            return reserved_word_values[i];
        }
    }

    return 0; // invalid word
}

// checks if the given string matches any special symbo and return its token value
// this is literally the same as the reservedToToken function but with different arrays
int specialToToken(char *buffer)
{
    char *special_symbols[] = {"+", "-", "*", "/", "(", ")", ",", ";", ".", "=", "<", ">", ":=", "<=", ">=", "<>"};
    int special_symbol_values[] = {plussym, minussym, multsym, slashsym, lparentsym, rparentsym, commasym, semicolonsym, periodsym, eqsym, lessym, gtrsym, becomessym, leqsym, geqsym, neqsym};
    int num_special_symbols = sizeof(special_symbols) / sizeof(special_symbols[0]);

    // loop through special symbols and check if buffer matches any of them
    for (int i = 0; i < num_special_symbols; i++)
    {
        if (strcmp(buffer, special_symbols[i]) == 0)
        {
            return special_symbol_values[i];
        }
    }

    return 0; // invalid symbol
}

// checks if a character is a special symbol, returning 1 for true and 0 for false
int specialSymbolCheck(char c)
{
    char special_symbols[] = "+-*/()=,.<>:;&%!?@#$^`~|";
    int num_special_symbols = sizeof(special_symbols) / sizeof(special_symbols[0]);

    for (int i = 0; i < num_special_symbols; i++)
    {
        if (c == special_symbols[i])
        {
            return 1;
        }
    }

    return 0;
}

// Create and initialize new list for storing tokens
list *create_list()
{
    list *l = malloc(sizeof(list));
    l->size = 0;
    l->capacity = 10;
    l->tokens = malloc(sizeof(token) * l->capacity);
    return l;
}

// Free the memory used by a list and its tokens
list *destroy_list(list *l)
{
    free(l->tokens);
    free(l);
    return NULL;
}

// Append a token to a list, resizing the list if necessary
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

// Add a token to a list, resizing the list if necessary
void add_token(list *l, token t)
{
    if (l->size == l->capacity)
    {
        l->capacity *= 2;
        l->tokens = realloc(l->tokens, sizeof(token) * l->capacity);
    }
    l->tokens[l->size++] = t;
}

// Print the lexeme table to both the console and output file
void print_lexeme_table(list *l)
{
    for (int i = 0; i < l->size; i++)
        printOutput("%10s %20s\n", l->tokens[i].lexeme, l->tokens[i].value);
}

// Print the tokens to both the console and output file
void print_tokens(list *l)
{
    int counter; // Counter to keep track of the number of tokens printed for sake of ommitting extra new line character at end of file

    for (int i = 0; i < l->size; i++)
    {
        printOutput("%s ", l->tokens[i].value);
        char identifier_value[3] = {0}, number_value[3] = {0};
        sprintf(identifier_value, "%d", identsym);
        sprintf(number_value, "%d", numbersym);

        // Check if the token value matches identifier or number and print its lexeme
        if (strcmp(l->tokens[i].value, identifier_value) == 0 || strcmp(l->tokens[i].value, number_value) == 0)
            printOutput("%s ", l->tokens[i].lexeme);
        counter++;
    }

    // Print a newline if we haven't reached the last token
    if (counter < l->size - 1)
    {
        printOutput("\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printOutput("Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }

    inputFile = fopen(argv[1], "r");
    outputFile = fopen(argv[2], "w");

    if (inputFile == NULL)
    {
        printOutput("Error: Could not open input file %s\n", argv[1]);
        return 1;
    }

    if (outputFile == NULL)
    {
        printOutput("Error: Could not open output file %s\n", argv[2]);
        return 1;
    }

    printOutput("Source Program:\n");
    printOriginal();
    printOutput("\n");
    printOutput("Lexeme Table:\n");
    printOutput("\n");
    printOutput("%10s %20s\n", "lexeme", "token type");

    token_list = create_list();

    char c;
    char buffer[TOKEN_LEN_MAX + 1] = {0};
    int buffer_index = 0;

    while ((c = fgetc(inputFile)) != EOF)
    {
        if (iscntrl(c) || isspace(c)) // Skip control characters and whitespace
        {
            c = fgetc(inputFile);
        }
        if (isdigit(c)) // Handle numbers
        {
            buffer[buffer_index++] = c;
            while (1)
            {
                char nextc = peekc();
                if (isspace(nextc) || specialSymbolCheck(nextc)) // If next character is a space or special symbol, we've reached the end of the number
                {
                    token t;
                    if (buffer_index > NUM_LEN_MAX)
                    {
                        // Number is too long
                        printOutput("%10s %20s\n", buffer, "ERROR: NUMBER TOO LONG");
                    }
                    else
                    {
                        // Number is valid
                        printOutput("%10s %20d\n", buffer, numbersym);
                        sprintf(t.value, "%d", numbersym);
                        strcpy(t.lexeme, buffer);
                        append_token(token_list, t);
                    }

                    // Clear buffer and break out of loop
                    cutString(buffer, buffer_index);
                    buffer_index = 0;
                    break;
                }
                else if (isdigit(nextc))
                {
                    // If next character is a digit, add it to the buffer
                    c = getc(inputFile);
                    buffer[buffer_index++] = c;
                }
                else if (nextc == EOF) // This is the last character in the file
                    break;
                else if (isalpha(nextc))
                {
                    // Invalid number
                    token t;
                    printOutput("%10s %20d\n", buffer, numbersym);
                    sprintf(t.value, "%d", numbersym);
                    strcpy(t.lexeme, buffer);
                    append_token(token_list, t);
                    cutString(buffer, buffer_index);
                    buffer_index = 0;
                    break;
                }
            }
        }
        else if (isalpha(c)) // Handle identifiers and reserved words
        {
            buffer[buffer_index++] = c;
            while (1)
            {
                char nextc = peekc();
                if (isspace(nextc) || specialSymbolCheck(nextc) || nextc == EOF) // If next character is a space or special symbol, we've reached the end of the identifier
                {
                    // Check reserved words
                    int token_value = reservedToToken(buffer);
                    if (token_value)
                    {
                        token t;
                        printOutput("%10s %20d\n", buffer, token_value);
                        sprintf(t.value, "%d", token_value);
                        strcpy(t.lexeme, buffer);
                        append_token(token_list, t);
                        cutString(buffer, buffer_index);
                        buffer_index = 0;
                        break;
                    }
                    else
                    {
                        // Identifier
                        token t;
                        if (buffer_index > ID_LEN_MAX) // Check if identifier is too long
                        {
                            printOutput("%10s %20s\n", buffer, "ERROR: IDENTIFIER TOO LONG");
                        }
                        else
                        {
                            // Valid identifier
                            printOutput("%10s %20d\n", buffer, identsym);
                            sprintf(t.value, "%d", identsym);
                            strcpy(t.lexeme, buffer);
                            append_token(token_list, t);
                        }

                        cutString(buffer, buffer_index);
                        buffer_index = 0;
                        break;
                    }
                    break;
                }
                else if (isalnum(nextc)) // If next character is a letter or digit, add it to the buffer
                {
                    c = getc(inputFile);
                    buffer[buffer_index++] = c;
                }
            }
        }
        else if (specialSymbolCheck(c)) // Handle special symbols
        {
            buffer[buffer_index++] = c;
            char nextc = peekc();

            if (specialSymbolCheck(nextc)) // Current character is special and so is the next
            {
                // Handle block comments
                if (c == '/' && nextc == '*')
                {
                    // Clear buffer
                    cutString(buffer, buffer_index);
                    buffer_index = 0;
                    while (1) // Consume characters until we reach the end of the block comment
                    {
                        c = getc(inputFile);
                        nextc = peekc();
                        if (c == '*' && nextc == '/')
                        {
                            c = getc(inputFile);
                            break;
                        }
                    }
                    continue;
                }

                // Handle single line comments
                if (c == '/' && nextc == '/')
                {
                    // Clear buffer
                    cutString(buffer, buffer_index);
                    buffer_index = 0;
                    while (1) // Consume characters until we reach the end of the line
                    {
                        c = getc(inputFile);
                        nextc = peekc();
                        if (c == '\n')
                        {
                            break;
                        }
                    }
                    continue;
                }

                // We have two pontentially valid symbols, so we need to check if they make a valid symbol

                c = getc(inputFile);
                buffer[buffer_index++] = c;
                token t;
                int token_value = specialToToken(buffer);
                if (!token_value)
                    // All symbols are invalid
                    for (int i = 0; i < buffer_index; i++)
                        printOutput("%10c %20s\n", buffer[i], "ERROR: INVALID SYMBOL");
                else
                {
                    // Both symbols make a valid symbol
                    printOutput("%10s %20d\n", buffer, token_value);
                    sprintf(t.value, "%d", token_value);
                    strcpy(t.lexeme, buffer);
                    append_token(token_list, t);
                }

                cutString(buffer, buffer_index);
                buffer_index = 0;
            }
            else
            {
                // Handle single special symbol
                token t;
                int token_value = specialToToken(buffer);
                if (!token_value)
                    printOutput("%10c %20s\n", c, "ERROR: INVALID SYMBOL");
                else
                {
                    printOutput("%10s %20d\n", buffer, token_value);
                    sprintf(t.value, "%d", token_value);
                    strcpy(t.lexeme, buffer);
                    append_token(token_list, t);
                }

                cutString(buffer, buffer_index);
                buffer_index = 0;
            }
        }
    }

    printOutput("\n");
    printOutput("Token List:\n");
    print_tokens(token_list); // Print tokens to console and output file
    printf("\n");
    destroy_list(token_list); // Free memory used by token list

    return 0;
}