#include <iostream>
#include <memory>

#include "cnode.h"
#include "lex.h"
#include "error.h"
#include "token.h"
#include "parser.h"


///////////////////////////////////////////////////////////////////////////////
//                        PARSE ARITHMETIC EXPRESSIONS                       //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  expr  ->  expr + term    |  expr - term    |  term                       //
//  term  ->  term * factor  |  term / factor  |  term mod factor |  factor  //
//  fact  ->  exp ^ factor   |  exp                                          //
//  exp   ->  +neg           |  -neg           |  neg                        //
//  neg   ->  val            |  (expr)         |  ident                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


unique_ptr<CNode> Parser::parse_arith_expr()
{
    auto left_node = parse_term();
    if (left_node == nullptr) return nullptr;

    while (tok.id == TOKEN_PLUS || tok.id == TOKEN_MINUS) {
        auto op_id = tok.id;
        get_token(tok);
        auto right_node = parse_term();
        if (right_node == nullptr) return nullptr;

        if (op_id == TOKEN_PLUS) {
            left_node = make_unique<AddNode>(std::move(left_node), std::move(right_node));
        }
        else { // minus
            left_node = make_unique<SubtractNode>(std::move(left_node), std::move(right_node));
        }
    }
    
    return left_node;
}


unique_ptr<CNode> Parser::parse_term() 
{
    auto left_node = parse_fact();
    if (left_node == nullptr) return nullptr;

    while (tok.id == TOKEN_MULT || tok.id == TOKEN_DIV || (tok.id == TOKEN_IDENT && tok.string_val == "mod")) {
        auto op_id = tok.id;
        get_token(tok);
        auto right_node = parse_fact();
        if (right_node == nullptr) return nullptr;

        if (op_id == TOKEN_MULT) {
            left_node = make_unique<MultiplyNode>(std::move(left_node), std::move(right_node));
        }
        else if (op_id == TOKEN_DIV) {
            left_node = make_unique<DivideNode>(std::move(left_node), std::move(right_node));
        }
        else { // mod
            left_node = make_unique<ModNode>(std::move(left_node), std::move(right_node));
        }
    }

    return left_node;
}


unique_ptr<CNode> Parser::parse_fact()
{
    auto fact_node = parse_pow();
    if (fact_node == nullptr) return nullptr;

    if (tok.id == TOKEN_EXP) {
        get_token(tok);
        auto pow_node = parse_fact();
        if (pow_node == nullptr) return nullptr;

        return make_unique<PowerNode>(std::move(fact_node), std::move(pow_node));
    }

    return fact_node;
}


unique_ptr<CNode> Parser::parse_pow()
{ 
    if (tok.id == TOKEN_PLUS || tok.id == TOKEN_MINUS) {
        auto op_id = tok.id;
        get_token(tok);
        auto pow_node = parse_neg();
        if (pow_node == nullptr) return nullptr;
        
        if (op_id == TOKEN_MINUS) {
            return make_unique<NegativeNode>(std::move(pow_node));
        }
        else { // positive
            return make_unique<PositiveNode>(std::move(pow_node));
        }
    }
    
    return parse_neg();
}


unique_ptr<CNode> Parser::parse_neg()
{
    unique_ptr<CNode> neg_node = nullptr;

    if (tok.id == TOKEN_LPAREN) {
        get_token(tok);
        neg_node = parse_expr();
        if (!eat(TOKEN_RPAREN, ")")) return nullptr;
    }
    else {
        neg_node = parse_val();
    }

    return neg_node;
}