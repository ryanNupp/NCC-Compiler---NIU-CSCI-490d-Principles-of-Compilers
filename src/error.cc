#include <iostream>

#include "bufio.h"
#include "error.h"

using std::cout, std::string;

void print_error(const Error &err) {
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
        cout << "Expected " << err.ch << " at " << err.line << ":" << err.col << "\n";
        cout << buf_getline(err.line) << string(err.col-1, '-') << '^' << "\n";
        break;
    case NCC_UNEXPECT_SYM:
        cout << "Unexpected symbol at " << err.line << ":" << err.col << "\n";
        cout << buf_getline(err.line) << string(err.col-1, '-') << '^' << "\n";
        break;
    }
}