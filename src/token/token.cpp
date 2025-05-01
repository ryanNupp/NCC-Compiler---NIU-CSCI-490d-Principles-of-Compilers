#include <iostream>

#include "token.h"

using std::cout;

constexpr string_view Token::get_token_name() {
    switch (id) {
    case TOKEN_NULL:          return "TOKEN_NULL";
    case TOKEN_EOF:           return "TOKEN_EOF";
    case TOKEN_PLUS:          return "TOKEN_PLUS";
    case TOKEN_MINUS:         return "TOKEN_MINUS";
    case TOKEN_MULT:          return "TOKEN_MULT";
    case TOKEN_DIV:           return "TOKEN_DIV";
    case TOKEN_EXP:           return "TOKEN_EXP";
    case TOKEN_LESS:          return "TOKEN_LESS";
    case TOKEN_LESS_EQ:       return "TOKEN_LESS_EQ";
    case TOKEN_GREATER:       return "TOKEN_GREATER";
    case TOKEN_GREATER_EQ:    return "TOKEN_GREATER_EQ";
    case TOKEN_EQUAL:         return "TOKEN_EQUAL";
    case TOKEN_NOT_EQUAL:     return "TOKEN_NOT_EQUAL";
    case TOKEN_NOT:           return "TOKEN_NOT";
    case TOKEN_AND:           return "TOKEN_AND";
    case TOKEN_OR:            return "TOKEN_OR";
    case TOKEN_ASSIGN:        return "TOKEN_ASSIGN";
    case TOKEN_COLON:         return "TOKEN_COLON";
    case TOKEN_SEMICOLON:     return "TOKEN_SEMICOLON";
    case TOKEN_COMMA:         return "TOKEN_COMMA";
    case TOKEN_DOT:           return "TOKEN_DOT";
    case TOKEN_AT:            return "TOKEN_AT";
    case TOKEN_LPAREN:        return "TOKEN_LPAREN";
    case TOKEN_RPAREN:        return "TOKEN_RPAREN";
    case TOKEN_LBRACE:        return "TOKEN_LBRACE";
    case TOKEN_RBRACE:        return "TOKEN_RBRACE";
    case TOKEN_LBRACKET:      return "TOKEN_LBRACKET";
    case TOKEN_RBRACKET:      return "TOKEN_RBRACKET";
    case TOKEN_IDENT:         return "TOKEN_IDENT";
    case TOKEN_REAL:          return "TOKEN_REAL";
    case TOKEN_INTEGER:       return "TOKEN_INTEGER";
    case TOKEN_STRING:        return "TOKEN_STRING";
    default:                  return "unknown token";
    }
}

void Token::print() {
    cout << get_token_name();

    if (id == TOKEN_IDENT || id == TOKEN_STRING)
        cout << ": " << string_val;
    else if (id == TOKEN_INTEGER)
        cout << ": " << int_val;
    else if (id == TOKEN_REAL)
        cout << ": " << real_val;
    cout << " at " << line << ":" << col << "\n";
}