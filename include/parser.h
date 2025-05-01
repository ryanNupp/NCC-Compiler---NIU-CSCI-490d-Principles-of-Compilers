#ifndef NCC_PARSER_H
#define NCC_PARSER_H

#include <unordered_map>

#include "tables.h"
#include "lex.h"
#include "cnode.h"

using std::unordered_map;

class Parser {
public:
    Parser(SymbolTable&);
    
    unique_ptr<CNode> parse();

private:
    // Lexer lexer;
    Token tok;
    StringTable strtbl{};
    SymbolTable& symtbl;

    bool eat(Token_Type, string);

    // parse 

    // parse statement blocks ---
    // --------------------------
    // stmt_block  ->  stmt (followed by) stmt_block  |  stmt
    // statement    :  print()  |  read()  |  declaration  |  assignment
    //
    // print statement formatting:
    //   print(expr);
    //   print(expr, expr, expr, ... );
    //
    // read statement formatting:
    //   read(var);
    //
    // variable declarations:
    //   int4 var_name;
    //
    // variable assignment formatting:
    //   var_name <- expr;
    //
    unique_ptr<CNode> parse_stmt_block();
    unique_ptr<CNode> parse_stmt();
    unique_ptr<CNode> parse_print_stmt();
    unique_ptr<CNode> parse_read_stmt();
    unique_ptr<CNode> parse_vardecl_stmt(string, int, int);
    unique_ptr<CNode> parse_varassig_stmt(string, int, int);
    unique_ptr<CNode> parse_if_stmt();
    unique_ptr<CNode> parse_else_stmt();
    unique_ptr<CNode> parse_while_stmt();

    // parse expressions ---
    // --------------------- 
    // expr  -> log_expr
    //
    unique_ptr<CNode> parse_expr();

    // parse logical expressions ---
    // -----------------------------
    // log_expr  ->  log_val   |  log_expr
    // log_val   ->  log_neg   |  log_val
    // log_neg   ->  rel_expr  |  log_neg
    //
    unique_ptr<CNode> parse_log_expr();
    unique_ptr<CNode> parse_log_term();
    unique_ptr<CNode> parse_log_neg();

    // parse relative expressions ---
    // ------------------------------
    //  rel_expr  ->  rel_expr = arith_expr  |  rel_expr < arith_expr  |  rel_expr > arith_expr
    //            ->  rel_expr <= arith expr |  rel-expr >= arith_expr |  rel_expr ~= arith expr
    //
    unique_ptr<CNode> parse_rel_expr();
    
    // parse arithmetic expressions ---
    // --------------------------------
    // arith_expr  ->  arith expr + term  |  arith expr - term    |  term
    // term        ->  term * factor      |  term / factor        |  term mod factor  |  factor
    // fact        ->  exp ^ factor       |  exp
    // exp         ->  +neg               |  -neg                 |  neg
    // neg         ->  val                |  (expr)               |  ident
    // val         ->  int const          |  string const
    //
    unique_ptr<CNode> parse_arith_expr();
    unique_ptr<CNode> parse_term();
    unique_ptr<CNode> parse_fact();
    unique_ptr<CNode> parse_pow();
    unique_ptr<CNode> parse_neg();
    


    // parse values ---
    // ----------------
    // val   ->   int literal   |   string literal   |   boolean literal   |   var
    unique_ptr<CNode> parse_val();
};

#endif