#pragma once

#include <string>

enum Token_Type {
    TOKEN_NULL,
    TOKEN_EOF,

    // arithmetic operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULT,
    TOKEN_DIV,
    TOKEN_EXP,

    // comparison operators
    TOKEN_LESS,
    TOKEN_LESS_EQ,
    TOKEN_GREATER,
    TOKEN_GREATER_EQ,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_NOT,
    TOKEN_AND,
    TOKEN_OR,

    // assig
    TOKEN_ASSIGN,

    // misc.
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_AT,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    
    // value tokens
    TOKEN_IDENT,
    TOKEN_REAL,
    TOKEN_INTEGER,
    TOKEN_STRING
};

struct Token {
    Token_Type id = TOKEN_NULL;
    int line, col;

    long long int int_val;
    double real_val;
    std::string string_val;
};

void print_token(const Token&);