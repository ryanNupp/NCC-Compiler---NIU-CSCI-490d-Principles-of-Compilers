#include <iostream>

#include "bufio.h"
#include "error.h"

using std::cout, std::string;

bool error_occurance = false;

void trip_error()
{
    error_occurance = true;
}

bool check_error_occur()
{
    return error_occurance;
}


void print_underline(const Error &err) {
    string line{buf_getline(err.line)};
    
    cout << line;
    if (line[line.length()-1] != '\n') {
        cout << '\n';
    }
    cout << string(err.col-1, '-') << "^\n";
}

void print_error(const Error &err) {
    trip_error();

    cout << "ERROR: ";

    switch (err.id) {
    case NCC_OK:
        break;
    case NCC_FILE_NOT_FOUND:
        cout << "File not found\n";
        break;
    case NCC_EOF:
        cout << "Unexpected end of file\n";
        break;
    case NCC_UNKNOWN_SYMBOL:
        cout << "Unknown symbol " << err.ch << " at " << err.line << ":" << err.col << "\n";
        break;
    case NCC_UNKNOWN_ESCAPE_SEQ:
        cout << "Unknown escape sequence " << err.str << " at " << err.line << ":" << err.col << "\n";
        break;
    case NCC_INVALID_UTF8:
        cout << "Invalid unicode at " << err.line << ":" << err.col << "\n";
        break;
    case NCC_MALFORMED_REAL:
        cout << "Malformed real at " << err.line << ":" << err.col << "\n";
        break;
    case NCC_EXPECT_SYM:
        cout << "Expected " << err.str << " at " << err.line << ":" << err.col << "\n";
        print_underline(err);
        break;
    case NCC_UNEXPECT_SYM:
        cout << "Unexpected symbol at " << err.line << ":" << err.col << "\n";
        print_underline(err);
        break;
    case NCC_EXPECT_EXPR:
        cout << "Expected expression at " << err.line << ":" << err.col << "\n";
        print_underline(err);
        break;
    case NCC_UNKNOWN_TYPE:
        cout << "Unknown type name '" << err.str << "' at " << err.line << ":" << err.col << "\n";
        print_underline(err);
        break;
    case NCC_NO_DECLARE:
        cout << "Variable '" << err.str << "' at " << err.line << ":" << err.col << " has not yet been declared\n";
        print_underline(err);
        break;
    case NCC_DUPE_DECLARE:
        cout << "Declaration of variable '" << err.str << "' at " << err.line << ":" << err.col << ", but has already been declared\n";
        print_underline(err);
        break;
    case NCC_REL_EXPR_INT4:
        cout << "Comparison at '" << err.line << ':' << err.col << "' must compare values of type int4\n";
        print_underline(err);
        break;
    }

    cout << "\n";
}