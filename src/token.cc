#include <iostream>

#include "token.h"

using std::cout;

const char token_names[32][20] = {
    "TOKEN_NULL", "TOKEN_EOF", "TOKEN_PLUS", "TOKEN_MINUS", "TOKEN_MULT",
    "TOKEN_DIV", "TOKEN_EXP", "TOKEN_LESS", "TOKEN_LESS_EQ", "TOKEN_GREATER",
    "TOKEN_GREATER_EQ", "TOKEN_EQUAL", "TOKEN_NOT_EQUAL", "TOKEN_NOT", "TOKEN_AND",
    "TOKEN_OR", "TOKEN_ASSIGN", "TOKEN_COLON", "TOKEN_SEMICOLON", "TOKEN_COMMA",
    "TOKEN_DOT", "TOKEN_AT", "TOKEN_LPAREN", "TOKEN_RPAREN", "TOKEN_LBRACE",
    "TOKEN_RBRACE","TOKEN_LBRACKET", "TOKEN_RBRACKET", "TOKEN_IDENT", "TOKEN_REAL",
    "TOKEN_INTEGER", "TOKEN_STRING"
};

void print_token(const Token& t) {
    cout << token_names[t.id];
    /*if (t.id == TOKEN_NULL)
        std::cout << " " << 3 << " ";*/
    // print longer things like str or int here
    if (t.id == TOKEN_IDENT || t.id == TOKEN_STRING)
        cout << ": " << t.string_val;
    else if (t.id == TOKEN_INTEGER)
        cout << ": " << t.int_val;
    else if (t.id == TOKEN_REAL)
        cout << ": " << t.real_val;
    cout << " at " << t.line << ":" << t.col << "\n";
}