#pragma once

#include <string>

////////////////////////////////////////
/////// TOKEN TYPE ////// ID ///////////
////////////////////////////////////////
#define TOKEN_NULL        0
#define TOKEN_EOF         1
// arithmetic operators
#define TOKEN_PLUS        2     //    +
#define TOKEN_MINUS       3     //    -
#define TOKEN_MULT        4     //    *
#define TOKEN_DIV         5     //    /
#define TOKEN_EXP         6     //    ^
// comparison operators
#define TOKEN_LESS        7     //    <
#define TOKEN_LESS_EQ     8     //    <=
#define TOKEN_GREATER     9     //    >
#define TOKEN_GREATER_EQ  10    //    >=
#define TOKEN_EQUAL       11    //    =
#define TOKEN_NOT_EQUAL   12    //    ~=
// logical operators
#define TOKEN_NOT         13    //    !
#define TOKEN_AND         14    //    &
#define TOKEN_OR          15    //    |
// assignment operator
#define TOKEN_ASSIGN      16    //    <-
// other punctuation
#define TOKEN_COLON       17    //    :
#define TOKEN_SEMICOLON   18    //    ;
#define TOKEN_COMMA       19    //    ,
#define TOKEN_DOT         20    //    .
#define TOKEN_AT          21    //    @
#define TOKEN_LPAREN      22    //    (
#define TOKEN_RPAREN      23    //    )
#define TOKEN_LBRACE      24    //    {
#define TOKEN_RBRACE      25    //    }
#define TOKEN_LBRACKET    26    //    [
#define TOKEN_RBRACKET    27    //    ]
// value-carrying tokens
#define TOKEN_IDENT       28
#define TOKEN_FLOAT       29
#define TOKEN_INTEGER     30
#define TOKEN_STRING      31
////////////////////////////////////////

struct Token {
    int id = TOKEN_NULL;
    int line, col;

    double num_val;
    std::string string_val;
};

void print_token(const Token&);