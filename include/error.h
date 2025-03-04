#pragma once

#include <string>

enum Error_Type {
    NCC_OK,
    NCC_FILE_NOT_FOUND,
    NCC_EOF,
    NCC_UNKNOWN_SYMBOL,
    NCC_UNKNOWN_ESCAPE_SEQ,
    NCC_INVALID_UTF8,
    NCC_MALFORMED_REAL,
    NCC_EXPECT_SYM,
    NCC_UNEXPECT_SYM
};

struct Error {
    Error_Type id = NCC_OK;
    int line, col;

    char ch;
    int num;
    std::string str;
};

void print_error(const Error&);