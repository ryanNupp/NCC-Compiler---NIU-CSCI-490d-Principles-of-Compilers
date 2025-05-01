#include <iostream>
#include <memory>

#include "lex.h"
#include "error.h"
#include "parser.h"
#include "cnode.h"
#include "codegen.h"

using std::cout;

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

    SymbolTable symtbl{};
    Parser parser{symtbl};
    Codegen codegen{parser, symtbl};

    auto codetree = parser.parse();

    if (!check_error_occur()) {
        cout << "Code Tree:\n";
        codetree->print(0);
        cout << "\n";

        //codegen.generate(std::move(codetree));
        //codegen.run();
        //cout << "\n";
    }

    lex_cleanup();
    return 0;
}