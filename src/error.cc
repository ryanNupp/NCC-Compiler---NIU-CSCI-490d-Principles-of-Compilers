#include <iostream>

#include "error.h"

using std::cout;

void print_error(const Error &err) {
    cout << "ERROR: ";

    switch (err.id) {
    case NCC_FILE_NOT_FOUND:
        cout << "File not found\n";
        break;
    case NCC_EOF:
        cout << "Unexpected end of file\n";
        break;
    case NCC_UNKNOWN_SYMBOL:
        cout << "Unknown symbol " << err.ch << " at "
             << err.line << ":" << err.col << "\n";
        break;
    case NCC_UNKNOWN_ESCAPE_SEQ:
        cout << "Unknown escape sequence " << err.str << " at "
             << err.line << ":" << err.col << "\n";
        break;
    case NCC_INVALID_UNICODE:
        cout << "Invalid unicode " << " at "
             << err.line << ":" << err.col << "\n";
    }
}