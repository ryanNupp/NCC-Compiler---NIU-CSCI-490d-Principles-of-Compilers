
#include "cnode.h"
#include "parser.h"
#include "token.h"


///////////////////////////////////////////////////////////////////////////////
//                                PARSE VALUES                               //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// val  ->   int literal   |   string literal   |   bool literal   |  var    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

unique_ptr<CNode> Parser::parse_val()
{
    unique_ptr<CNode> val = nullptr;

    // integer literal
    if (tok.id == TOKEN_INTEGER) {
        val = std::make_unique<IntegerNode>(tok.int_val);
    }

    // string literal
    else if (tok.id == TOKEN_STRING) {
        val = std::make_unique<StringNode>(strtbl.add_string(tok.string_val));
    }

    // identifier -- can be: bool literak, variable
    else if (tok.id == TOKEN_IDENT) {

        // bool literal - true
        if (tok.string_val == "true") {
            val = std::make_unique<BoolNode>(true);
        }

        // bool literal - false
        else if (tok.string_val == "false") {
            val = std::make_unique<BoolNode>(false);
        }

        // variable
        else if (symtbl.symbolExists(tok.string_val)) {
            val = std::make_unique<VariableNode>(tok.string_val);
        }

        // ERROR - undeclared identifier
        else {
            auto err = Error{NCC_NO_DECLARE, tok.line, tok.col};
            err.str = tok.string_val;
            print_error(err);
        }
    }

    // ERROR - unexpected symbol
    else {
        print_error(Error{NCC_UNEXPECT_SYM, tok.line, tok.col});
    }

    get_token(tok);
    return val;
}