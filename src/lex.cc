#include <string>

#include <iostream>
#include <bitset>

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
int get_ucode_num(Token&);
bool is_digit(char&);
bool is_hex(char&);
bool is_alpha(char&);
bool is_underscore(char&);

bool end_of_file;

Error lex_init(const char *filepath) {
    end_of_file = false;
    Error err;
    err.id = buf_init(filepath);
    return err;
}

void lex_cleanup() {
    buf_cleanup();
}

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
    case '"':
        return string_token(tok);
    }
    // identifiers
    if (is_alpha(ch) || is_underscore(ch))
        return ident_token(tok);

    // integers
    if (is_digit(ch))
        return int_token(tok);

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

// returns NCC_INVALID_UTF8
// otherwise adds unicode character to the string
int get_ucode_num(Token &tok) {
    long ucode_num = 0;
    char ch;

    for (int i=0; i<6; i++) {
        if (buf_get_next_char(ch) != 0 || !is_hex(ch)) {
            buf_prev_char();
            return NCC_INVALID_UTF8;
        }

        ucode_num *= 0x10;
        
        // 0 - 9
        if (is_digit(ch))
            ucode_num += ch - 48;
        // A - F
        else if (is_alpha(ch) && ch <= 70)
            ucode_num += ch - 65 + 10;
        // a - f
        else if (is_alpha(ch) && ch >= 97)
            ucode_num += ch - 97 + 10;
    }

    // encode unicode to char or whatever, after getting all int values
    if (ucode_num >= 0x0 && ucode_num <= 0x7F) {
        // 0yyy zzzz
        ucode_num = ucode_num & 0x7F;
        tok.string_val += ((ucode_num & 0x000000FF));
    }
    else if (ucode_num >= 0x80 && ucode_num <= 0x7FF) {
        // 110xx xxyy 10yy zzzz
        ucode_num = 0xC080
                  + (ucode_num & 0x03F)
                  + ((ucode_num & 0x7C0) << 2);
        tok.string_val += ((ucode_num & 0x0000FF00) >> 8);
        tok.string_val += ((ucode_num & 0x000000FF));
    }
    else if (ucode_num >= 0x800 && ucode_num <= 0xFFFF) {
        // 1110 wwww 10xx xxyy 10yy zzzz
        ucode_num = 0xE08080
                  + (ucode_num & 0x03F)
                  + ((ucode_num & 0x0FC0) << 2)
                  + ((ucode_num & 0xF000) << 4);
        tok.string_val += ((ucode_num & 0x00FF0000) >> 16);
        tok.string_val += ((ucode_num & 0x0000FF00) >> 8);
        tok.string_val += ((ucode_num & 0x000000FF));
    }
    else if (ucode_num >= 0x10000 && ucode_num <= 0x10FFFF) {
        // 1111 0uvv 10vv wwww 10xx xxyy 10yy zzzz
        ucode_num = 0xF0808080
                  + (ucode_num & 0x00003F)
                  + ((ucode_num & 0x000FC0) << 2)
                  + ((ucode_num & 0x03F000) << 4)
                  + ((ucode_num & 0x1C0000) << 6);
        tok.string_val += ((ucode_num & 0xFF000000) >> 24);
        tok.string_val += ((ucode_num & 0x00FF0000) >> 16);
        tok.string_val += ((ucode_num & 0x0000FF00) >> 8);
        tok.string_val += ((ucode_num & 0x000000FF));
    }
    else {
        return NCC_INVALID_UTF8;
    }
    
    return NCC_OK;
}

Error string_token(Token &tok) {
    Error err;
    err.id = NCC_OK;
    tok.id = TOKEN_STRING;

    tok.string_val = "";
    char ch;
    
    while (buf_get_next_char(ch) == 0) {
        switch (ch) {
        case '"':
            buf_next_char();
            return err;
        case '\\':
            if (buf_get_next_char(ch) != 0) {
                err.id = NCC_EOF;
                return err;
            }
            switch (ch) {
            // line continuation, skip
            case '\n':    continue;
            // add character as-is
            case '"':
            case '\\':    break;
            // convert to escaped version
            case 'r':     ch = '\r';    break;
            case 'n':     ch = '\n';    break;
            case 't':     ch = '\t';    break;
            case 'a':     ch = '\a';    break;
            case 'b':     ch = '\b';    break;
            // unicode encoding
            case 'u':
                if (get_ucode_num(tok) == NCC_INVALID_UTF8) {
                    err.id = NCC_INVALID_UTF8;
                    err.line = buf_line_pos;
                    err.col  = buf_col_pos;
                }
                continue;
            // unknown escape sequence
            default:
                err.id = NCC_UNKNOWN_ESCAPE_SEQ;
                err.line = buf_line_pos;
                err.col  = buf_col_pos;
                err.str  = {'\\', ch};
                continue;
            }
        }
        tok.string_val += ch;
    }
    err.id = NCC_EOF;
    return err;
}

Error ident_token(Token &tok) {
    Error err;
    err.id = NCC_OK;

    char ch;
    buf_get_curr_char(ch);
    string ident_str (1, ch);
    
    while (buf_get_next_char(ch) == 0 && 
          (is_alpha(ch) || is_digit(ch) || is_underscore(ch))) {
        ident_str += ch;
    }

    tok.id = TOKEN_IDENT;
    tok.string_val = ident_str;
    return err;
}

Error int_token(Token &tok) {
    Error err;
    err.id = NCC_OK;
    tok.id = TOKEN_INTEGER;

    char ch;
    buf_get_curr_char(ch);
    long val = ch - 48;

    while (buf_get_next_char(ch) == 0 && is_digit(ch)) {
        val *= 10;
        val += (ch - 48);
    }

    tok.int_val = val;
    return err;
}

Error float_token(Token &tok) {
    Error err;
    return err;
}

// 0 - 9
bool is_digit(char &ch) {
    return (ch >= '0' && ch <= '9');
}

// 0 - 9, a - f, A - F
bool is_hex(char &ch) {
    return is_digit(ch)
        || (ch >= 'a' && ch <= 'f')
        || (ch >= 'A' && ch <= 'F');
}

// a - z   or A - Z
bool is_alpha(char &ch) {
    return (ch >= 'a' && ch <= 'z')
        || (ch >= 'A' && ch <= 'Z');
}

// _
bool is_underscore(char &ch) {
    return (ch == '_');
}