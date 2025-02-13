#include <iostream>

#include "lex.h"
#include "error.h"

int main(int argc, char **argv) {
    // check if file arg is present
    if (argc != 2) {
        std::cout << "Usage: ncc /path/to/file\n";
        return 1;
    }

    Error err = lex_init(argv[1]);
    if (err.id != 0) {
        print_error(err);
        return 1;
    }

    Token tok;
    while (!lex_eof()) {
        err = get_token(tok);
        if (err.id != NCC_OK)
            print_error(err);
        if (tok.id != TOKEN_NULL)
            print_token(tok);
    }

    return 0;
}