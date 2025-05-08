#include <cmath>
#include <string>
#include <cctype>

#include "lex.h"
#include "bufio.h"
#include "token.h"
#include "error.h"

using std::string;

Error block_comment(Token&);
Error ident_token(Token&);
Error string_token(Token&);
Error int_token(Token&);
Error real_token(Token&);
int get_ucode_num(Token&);

bool eof_flag;

Error lex_init(const char *filepath) {
    eof_flag = false;
    Error err{NCC_OK};
    if (buf_init(filepath) == -1)
        err.id = NCC_FILE_NOT_FOUND;
    return err;
}

void lex_cleanup() {
    buf_cleanup();
}

bool lex_eof() {
    return eof_flag;
}

// Get a token from the buffer reading the input file
//   returns an error, modifies the Token parameter
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
        eof_flag = true;
        tok.id = TOKEN_EOF;
        return err;
    }

    switch (ch) {
    // ignore whitespace
    case ' ':
    case '\t':
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
    case ',':    tok.id = TOKEN_COMMA;        break;
    case '@':    tok.id = TOKEN_AT;           break;
    case '(':    tok.id = TOKEN_LPAREN;       break;
    case ')':    tok.id = TOKEN_RPAREN;       break;
    case '{':    tok.id = TOKEN_LBRACE;       break;
    case '}':    tok.id = TOKEN_RBRACE;       break;
    case '[':    tok.id = TOKEN_LBRACKET;     break;
    case ']':    tok.id = TOKEN_RBRACKET;     break;

    // tokens  .  token_real (floating point value)
    case '.':
        if (buf_get_next_char(ch) == 0 && isdigit(ch)) {
            buf_prev_char();
            tok.int_val = 0;
            return real_token(tok);
        }
        buf_prev_char();
        tok.id = TOKEN_DOT;
        break;

    // tokens  ~  ~=
    case '~':
        if (buf_get_next_char(ch) == 0 && ch == '=') {
            tok.id = TOKEN_NOT_EQUAL;
            break;
        }
        buf_prev_char();
        buf_get_curr_char(err.ch);
        err.id = NCC_UNKNOWN_SYMBOL;
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
        err.id = NCC_EOF;
        break;

    // strings
    case '"':
        return string_token(tok);
    
    default:
        // identifiers
        if (isalpha(ch) || ch == '_')
            return ident_token(tok);

        // integers
        else if (isdigit(ch))
            return int_token(tok);

        err.ch = ch;
        err.id = NCC_UNKNOWN_SYMBOL;
    }

    buf_next_char();
    return err;
}

// Ignores block comment, returns the next token
//   if EOF encountered before the end of the block comment ->>
//   and NCC_EOF error is returned (unexpected end of file)
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

// gathers the string name of an identifier token
Error ident_token(Token &tok) {
    Error err;
    err.id = NCC_OK;

    char ch;
    buf_get_curr_char(ch);
    string ident_str (1, ch);
    
    while (buf_get_next_char(ch) == 0 && 
          (isalpha(ch) || isdigit(ch) || ch == '_')) {
        ident_str += ch;
    }

    tok.id = TOKEN_IDENT;
    tok.string_val = ident_str;
    return err;
}

// adds token integer value as integer characters are parsed for an integer token
Error int_token(Token &tok) {
    Error err;
    err.id = NCC_OK;
    tok.id = TOKEN_INTEGER;

    char ch;
    buf_get_curr_char(ch);
    tok.int_val = ch - 48;

    while (buf_get_next_char(ch) == 0 && isdigit(ch)) {
        tok.int_val *= 10;
        tok.int_val += (ch - 48);
    }

    if (ch == '.' || ch == 'e') {
        return real_token(tok);
    }

    return err;
}

// TODO
Error real_token(Token &tok) {
    Error err;
    err.id = NCC_OK;
    tok.id = TOKEN_REAL;

    tok.real_val = tok.int_val;

    char ch;
    buf_get_curr_char(ch);
    int state = (ch == '.') ? 1 : 3;
    double decimal_place = 1;
    bool e_pos = true;

    while (state > 0) {
        switch (state) {
        case 1:  // .
            if (buf_get_next_char(ch) == -2 || !isdigit(ch))
                state = 0;
            else
                state = 2;

            break;

        case 2:  // decimal digit (after .)
            decimal_place *= 0.1;
            tok.real_val += (ch - 48) * decimal_place;

            if (buf_get_next_char(ch) == -2) {
                state = 0;
                break;
            }

            if (isdigit(ch))
                state = 2;
            else if (ch == 'e' || ch == 'E')
                state = 3; 
            else
                state = 0;

            break;

        case 3:  // e
            if (buf_get_next_char(ch) == -2) {
                state = -1;
                break;
            }
            
            if (ch == '+' || ch == '-') {
                e_pos = (ch == '+');
                if (buf_get_next_char(ch) == -2) {
                    state = -1;
                    break;
                }
            }

            if (isdigit(ch)) {
                decimal_place = 0;
                state = 4;
            }
            else {
                state = -1;
            }
            
            break;

        case 4:  // decimal digit (after e)
            
            decimal_place *= 10;
            decimal_place += ch - 48;

            if (buf_get_next_char(ch) == -2) {
                state = 5;
                break;
            }

            if (isdigit(ch))
                state = 4;
            else
                state = 5;

            break;
        
        case 5:  // process sci notation (e)
            tok.real_val *= pow((e_pos) ? 10 : 0.1, decimal_place);
            state = 0;
            break;
        }
    }

    if (state == -1) {  // malformed real
        buf_prev_char();
        tok.id = TOKEN_NULL;
        err.id = NCC_MALFORMED_REAL;
        err.line = buf_line_pos;
        err.col = buf_col_pos;
        buf_next_char();
    }

    return err;
}

// Process a string token
//   if EOF before closing quote " - return error NCC_EOF
//   if unknown escape sequence    - return error NCC_UNKNOWN_ESC_SEQ
//   if invalid UTF-8 after \u     - return error NCC_INVALID_UTF8
//      -- valid UTF-8 must be: \u followed by 6 hex digits
//         * the hex value must not be greater than 10FFFF
//         * any extra digits will just become another character in the string
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

// adds unicode character to the string in the given Token& tok
//   returns NCC_INVALID_UTF8 if invalid unicode
int get_ucode_num(Token &tok) {
    long ucode_num = 0;
    char ch;

    for (int i=0; i<6; i++) {
        if (buf_get_next_char(ch) == -2 || !isxdigit(ch)) {
            buf_prev_char();
            return NCC_INVALID_UTF8;
        }

        ucode_num *= 0x10;
        
        // 0 - 9
        if (isdigit(ch))
            ucode_num += ch - 48;
        // A - F
        else if (isalpha(ch) && ch <= 70)
            ucode_num += ch - 65 + 10;
        // a - f
        else if (isalpha(ch) && ch >= 97)
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