#include <string>

#include "lex.h"
#include "bufio.h"
#include "token.h"
#include "error.h"

using std::string;

Error block_comment(Token&);
Error ident_token(Token&);
Error string_token(Token&);
Error int_token(Token&);
Error float_token(Token&);

bool end_of_file;

Error lex_init(const char *filepath) {
    end_of_file = false;
    Error err;
    err.id = buf_init(filepath);
    return err;
}

void lex_cleanup() {}

bool lex_eof() {
    return end_of_file;
}

Error get_token(Token &tok) {
    tok.id   = TOKEN_NULL;
    tok.line = buf_line_pos;
    tok.col  = buf_col_pos;
    
    char ch;

    Error err;
    err.id   = NCC_OK;
    err.line = buf_line_pos;
    err.col  = buf_col_pos;
    if (buf_get_curr_char(ch) == -2) {
        end_of_file = true;
        tok.id = TOKEN_EOF;
        return err;
    }

    switch (ch) {
    // ignore whitespace
    case ' ':
    case '\n':
        buf_next_char();
        return get_token(tok);

    // single-char tokens    
    case '+':    tok.id = TOKEN_PLUS;         break;
    case '-':    tok.id = TOKEN_MINUS;        break;
    case '*':    tok.id = TOKEN_MULT;         break;
    case '/':    tok.id = TOKEN_DIV;          break;
    case '^':    tok.id = TOKEN_EXP;          break;
    case '=':    tok.id = TOKEN_EQUAL;        break;
    case '!':    tok.id = TOKEN_NOT;          break;
    case '&':    tok.id = TOKEN_AND;          break;
    case '|':    tok.id = TOKEN_OR;           break;
    case ';':    tok.id = TOKEN_SEMICOLON;    break;
    case ':':    tok.id = TOKEN_COLON;        break;
    case '.':    tok.id = TOKEN_DOT;          break;
    case ',':    tok.id = TOKEN_COMMA;        break;
    case '@':    tok.id = TOKEN_AT;           break;
    case '(':    tok.id = TOKEN_LPAREN;       break;
    case ')':    tok.id = TOKEN_RPAREN;       break;
    case '{':    tok.id = TOKEN_LBRACE;       break;
    case '}':    tok.id = TOKEN_RBRACE;       break;
    case '[':    tok.id = TOKEN_LBRACKET;     break;
    case ']':    tok.id = TOKEN_RBRACKET;     break;

    // tokens  ~  ~=
    case '~':
        if (buf_get_next_char(ch) == 0 && ch == '=') {
            tok.id = TOKEN_NOT_EQUAL;
            break;
        }
        buf_prev_char();
        tok.id = TOKEN_NULL;
        err.id = NCC_UNKNOWN_SYMBOL;
        err.ch = ch;
        break;

    // tokens  >  >=
    case '>':
        if (buf_get_next_char(ch) == 0 && ch == '=') {
            tok.id = TOKEN_GREATER_EQ;
            break;
        }
        buf_prev_char();
        tok.id = TOKEN_GREATER;
        break;
    
    // tokens  <  <=  <-  <<-
    case '<':
        if (buf_get_next_char(ch) == 0) {
            if (ch == '=') { 
                tok.id = TOKEN_LESS_EQ;
                break;
            }
            if (ch == '-') {
                tok.id = TOKEN_ASSIGN;
                break;
            }
            if (ch == '<') {
                if (buf_get_next_char(ch) == 0 && ch == '-') {
                    return block_comment(tok);   
                }
                buf_prev_char();
            }
        }
        buf_prev_char();
        tok.id = TOKEN_LESS;
        break;

    // single-line comments
    case '#':
        while (buf_get_next_char(ch) == 0) {
            if (ch == '\n') {
                buf_next_char();
                return get_token(tok);
            }
        }
        tok.id = TOKEN_NULL;
        err.id = NCC_EOF;
        break;

    // strings
    case '"':    return string_token(tok);
    }
    // identifiers
    if ()
        return ident_token(tok);

    buf_next_char();
    return err;
}

Error block_comment(Token &tok) {
    char ch;
    Error err;
    while (buf_get_next_char(ch) == 0) {
        if (ch == '-' && buf_get_next_char(ch) == 0 &&
            ch == '>' && buf_get_next_char(ch) == 0 &&
            ch == '>')
        {
            buf_next_char();
            return get_token(tok);
        }
    }
    tok.id = TOKEN_NULL;
    err.id = NCC_EOF;
    return err;
}

Error string_token(Token &tok) {
    string str = "";
    Error err;
    return err;
}

Error ident_token(Token &tok) {
    Error err;
    return err;
}

Error int_token(Token &tok) {
    char ch;
    Error err;
    return err;
}

Error float_token(Token &tok) {
    Error err;
    return err;
}