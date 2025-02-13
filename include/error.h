#pragma once

#include <string>

////////////////////////////////////////
/////// ERROR TYPE //////////// ID /////
////////////////////////////////////////
#define NCC_OK                   0
#define NCC_FILE_NOT_FOUND      -1
#define NCC_EOF                 -2
#define NCC_UNKNOWN_SYMBOL      -3
#define NCC_UNKNOWN_ESCAPE_SEQ  -4
#define NCC_INVALID_UTF8        -5
////////////////////////////////////////

struct Error {
    int id;
    int line, col;

    char ch;
    int num;
    std::string str;
};

void print_error(const Error&);