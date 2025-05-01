#include <iostream>
#include <memory>

#include "cnode.h"
#include "lex.h"
#include "error.h"
#include "token.h"
#include "parser.h"

using std::cout;

// Public methods

Parser::Parser(SymbolTable& symtbl)
    : symtbl{symtbl}
{
    get_token(tok);
}


unique_ptr<CNode> Parser::parse()
{
    return parse_stmt_block();
}


// Private methods

bool Parser::eat(Token_Type type, string symbol)
{
    if (tok.id == type) {
        get_token(tok);
        return true;
    }
    else {
        auto err = Error{NCC_EXPECT_SYM, tok.line, tok.col};
        err.str = symbol;
        print_error(err);
        return false;
    }
}