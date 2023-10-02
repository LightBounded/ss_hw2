**Pharit Smitasin**
**COP3402**
**Euripides Montagne**
**Fall 2023**

The lex.c program is a lexical analyzer written in C. It reads in a source file and outputs a list of tokens.

The lexical analyzer is responsible for identifying these tokens and categorizing them into different types, such as identifiers, numbers, and special symbols while **ignoring** grammar.

The lexical analyzer works by reading in the source code file one character at a time and grouping them into tokens. It recognizes the following types of tokens:

- **Identifiers**
- **Numbers**
- **Special symbols**: `+`, `-`, `*`, `/`, `(`, `)`, `=`, `,`, `.`, `<`, `>`, `;`, `:`, `[`, `]`, `:=`, `<=`, `>=`, `<>`.
- **Reserved words**: `begin`, `end`, `if`, `then`, `while`, `do`, `call`, `const`, `var`, `procedure`, `out`, `in`, `else`, `odd`.

## How to run

To compile:
`gcc lex.c -o lex`

To run:
`./lex input.txt output.txt`

Remove the compiled file after each run:
`rm lex`
