#include <iostream>

#include "lex.h"
#include "error.h"
#include "parse.h"
#include "codegen.h"

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

        // generate machine code
        codegen_init();
        codegen_gen(top_node);
        
        // execute program (prints output)
        codegen_run();
        std::cout << "\n";

        parse_tree_cleanup();
        codegen_cleanup();
    }
    lex_cleanup();

    return 0;
}