#include <iostream>

#include "lex.h"
#include "error.h"
#include "parse.h"

int main(int argc, char **argv) {
    // check if file arg is present
    if (argc != 2) {
        std::cout << "Usage: ncc /path/to/file\n";
        return 1;
    }

    // initialize lex & buffer
    Error err = lex_init(argv[1]);
    if (err.id == NCC_FILE_NOT_FOUND) {
        print_error(err);
        return 1;
    }

    Token tok;
    get_token(tok);

    while (!lex_eof()) {
        parse(tok);

        parse_tree_printout();

        std::cout << "\n";

        // generate machine code
        
        // execute program

        parse_tree_cleanup();
    }

    lex_cleanup();

    return 0;
}