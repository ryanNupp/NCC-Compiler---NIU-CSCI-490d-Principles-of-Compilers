#include <iostream>
#include <memory>

#include "cnode.h"
#include "lex.h"
#include "error.h"
#include "token.h"
#include "parser.h"


///////////////////////////////////////////////////////////////////////////////
//                             PARSE STATEMENTS                              //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  stmt_block  ->  stmt (followed by) stmt_block  |  nothing                //
//  statement    :  print  |  read  |  declaration  |  assignment            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


unique_ptr<CNode> Parser::parse_stmt_block() {
    vector<unique_ptr<CNode>> stmts;

    while (!lex_eof() && tok.id != TOKEN_RBRACE) {
        unique_ptr<CNode> stmt_node = parse_stmt();
        if (stmt_node == nullptr) {
            return nullptr;
        }
        else {
            stmts.push_back(std::move(stmt_node));
        }
    }

    return make_unique<StatementBlockNode>(std::move(stmts));
}


unique_ptr<CNode> Parser::parse_stmt() {
    if (tok.id != TOKEN_IDENT) {
        print_error(Error{NCC_UNEXPECT_SYM, tok.line, tok.col});
        get_token(tok);
        return nullptr;
    }
    else if (tok.string_val == "print")    {    get_token(tok);    return parse_print_stmt();    }
    else if (tok.string_val == "read")     {    get_token(tok);    return parse_read_stmt();     }
    else if (tok.string_val == "if")       {    get_token(tok);    return parse_if_stmt();       }
    else if (tok.string_val == "while")    {    get_token(tok);    return parse_while_stmt();    }

    // if falls thru to here, either var assignment OR var declaration
    //   - first identifier does not yet determine the statement type
    string ident = tok.string_val;
    int line = tok.line;
    int col  = tok.col;
    get_token(tok);

    if (tok.id == TOKEN_ASSIGN) {
        get_token(tok);
        return parse_varassig_stmt(ident, line, col);
    }
    else {
        return parse_vardecl_stmt(ident, line, col);
    }
}


unique_ptr<CNode> Parser::parse_print_stmt()
{
    if (!eat(TOKEN_LPAREN, "(")) return nullptr;

    vector<unique_ptr<CNode>> exprs;
    unique_ptr<CNode> expr_node = parse_expr();
    if (expr_node == nullptr) {
        print_error(Error{NCC_EXPECT_EXPR, tok.line, tok.col});
        return nullptr; 
    }
    else {
        exprs.push_back(std::move(expr_node));
    }

    // , expr, expr, expr... (if exists)
    while(tok.id == TOKEN_COMMA) {
        eat(TOKEN_COMMA, ",");
        expr_node = parse_expr();
        if (expr_node == nullptr) {
            print_error(Error{NCC_EXPECT_EXPR, tok.line, tok.col});
            return nullptr; 
        }
        else {
            exprs.push_back(std::move(expr_node));
        }
    }

    if (!eat(TOKEN_RPAREN, ")")) return nullptr;
    if (!eat(TOKEN_SEMICOLON, ";")) return nullptr;

    return make_unique<PrintNode>(std::move(exprs));
}


unique_ptr<CNode> Parser::parse_read_stmt()
{
    if (!eat(TOKEN_LPAREN, "(")) return nullptr;

    unique_ptr<CNode> expr_node = parse_expr();
    if (expr_node == nullptr) {
        print_error(Error{NCC_EXPECT_EXPR, tok.line, tok.col});
        return nullptr; 
    }
    
    if (!eat(TOKEN_RPAREN, ")")) return nullptr;
    if (!eat(TOKEN_SEMICOLON, ";")) return nullptr;

    return make_unique<ReadNode>(std::move(expr_node));
}


unique_ptr<CNode> Parser::parse_if_stmt()
{
    if (!eat(TOKEN_LPAREN, "(")) return nullptr;

    unique_ptr<CNode> expr_node = parse_expr();   // MIGHT HAVE TO: ensure it is a logical expression??
    if (expr_node == nullptr) {
        print_error(Error{NCC_EXPECT_EXPR, tok.line, tok.col});
        return nullptr;
    }

    if (!eat(TOKEN_RPAREN, ")")) return nullptr;

    unique_ptr<CNode> if_body = nullptr;
    // statement block vs. singular statement
    if (tok.id == TOKEN_LBRACE) {
        get_token(tok);
        if_body = parse_stmt_block();
        if (!eat(TOKEN_RBRACE, "}")) return nullptr;
    }
    else {
        //get_token(tok);
        if_body = parse_stmt();
    }
    if (if_body == nullptr) return nullptr;

    unique_ptr<CNode> else_stmt = nullptr;
    if (tok.id == TOKEN_IDENT && tok.string_val == "else") {
        else_stmt = parse_else_stmt();
    }

    return make_unique<IfNode>(std::move(expr_node), std::move(if_body), std::move(else_stmt));
}


unique_ptr<CNode> Parser::parse_else_stmt()
{
    // statement block vs. singular statement
    if (tok.id == TOKEN_LBRACE) {

    }
    else {

    }

    return nullptr;
}


unique_ptr<CNode> Parser::parse_while_stmt()
{
    return nullptr;    
}







unique_ptr<CNode> Parser::parse_vardecl_stmt(string var_type, int line, int col)
{
    if (var_type == "int4") {
        if (tok.id != TOKEN_IDENT) {
            print_error(Error{NCC_UNEXPECT_SYM, tok.line, tok.col});
            return nullptr;
        }
        string var_name = tok.string_val;
        
        if (symtbl.symbolExists(var_name)) {
            auto err = Error{NCC_DUPE_DECLARE, tok.line, tok.col};
            err.str = var_name;
            print_error(err);
            get_token(tok);
            return nullptr;
        }
        symtbl.addSymbol(var_name, var_type);
        
        get_token(tok);
        if (!eat(TOKEN_SEMICOLON, ";")) return nullptr;

        return make_unique<VarDeclareNode>(var_name);
    }

    auto err = Error{NCC_UNKNOWN_TYPE, line, col};
    err.str = var_type;
    print_error(err);
    return nullptr;
}


unique_ptr<CNode> Parser::parse_varassig_stmt(string var_name, int line, int col)
{
    auto expr_node = parse_expr();
    if (expr_node == nullptr) {
        print_error(Error{NCC_EXPECT_EXPR, tok.line, tok.col});
        return nullptr;
    }

    if (!symtbl.symbolExists(var_name)) {
        auto err = Error{NCC_NO_DECLARE, line, col};
        err.str = tok.string_val;
        print_error(err);
        return nullptr;
    }

    if (!eat(TOKEN_SEMICOLON, ";")) return nullptr;
    
    return make_unique<VarAssignNode>(var_name, std::move(expr_node));
}