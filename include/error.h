#pragma once

#include <string>

void trip_error();
bool check_error_occur();

enum Error_Type {
    NCC_OK,
    NCC_FILE_NOT_FOUND,
    NCC_EOF,
    NCC_UNKNOWN_SYMBOL,
    NCC_UNKNOWN_ESCAPE_SEQ,
    NCC_INVALID_UTF8,
    NCC_MALFORMED_REAL,
    NCC_EXPECT_SYM,
    NCC_UNEXPECT_SYM,
    NCC_EXPECT_EXPR,
    NCC_UNKNOWN_TYPE,
    NCC_NO_DECLARE,
    NCC_DUPE_DECLARE,
    NCC_REL_EXPR_INT4
    
};

struct Error {
    Error_Type id = NCC_OK;
    int line, col;

    char ch;
    int num;
    std::string str;
};

void print_error(const Error&);