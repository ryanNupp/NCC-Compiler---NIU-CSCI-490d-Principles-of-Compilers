#include <iostream>
#include <memory>

#include "cnode.h"
#include "lex.h"
#include "error.h"
#include "token.h"
#include "parser.h"


unique_ptr<CNode> Parser::parse_expr()
{
    return parse_log_expr();
}

///////////////////////////////////////////////////////////////////////////////
//                         PARSE LOGICAL EXPRESSIONS                         //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  log_expr    ->    log_val     |    log_expr                              //
//  log_val     ->    log_neg     &    log_val                               //
//  log_neg     ->    !rel_expr                                              //
//              ->    rel_expr                                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

unique_ptr<CNode> Parser::parse_log_expr()
{
    auto left_node = parse_log_term();
    if (left_node == nullptr) return nullptr;

    while (tok.id == TOKEN_OR) {
        get_token(tok);
        auto right_node = parse_log_term();
        if (right_node == nullptr) return nullptr;

        left_node = make_unique<OrNode>(std::move(left_node), std::move(right_node));
    }

    return left_node;
}


unique_ptr<CNode> Parser::parse_log_term()
{
    auto left_node = parse_log_neg();
    if (left_node == nullptr) return nullptr;

    while (tok.id == TOKEN_AND) {
        get_token(tok);
        auto right_node = parse_log_neg();
        if (right_node == nullptr) return nullptr;

        left_node = make_unique<AndNode>(std::move(left_node), std::move(right_node));
    }

    return left_node;
}


unique_ptr<CNode> Parser::parse_log_neg()
{
    if (tok.id == TOKEN_NOT) {
        get_token(tok);
        auto rel_expr = parse_rel_expr();
        if (rel_expr == nullptr) return nullptr;
        
        return make_unique<NotNode>(std::move(rel_expr));
    }
    else {
        return parse_rel_expr();
    }
}


///////////////////////////////////////////////////////////////////////////////
//                         PARSE RELATIVE EXPRESSIONS                        //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  rel_expr  ->  rel_expr = arith_expr   |   rel_expr < arith_expr          //
//            ->  rel_expr > arith_expr   |   rel_expr <= arith expr         //
//            ->  rel-expr >= arith_expr  |   rel_expr ~= arith expr         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

unique_ptr<CNode> Parser::parse_rel_expr()
{
    auto left_expr = parse_arith_expr();
    if (left_expr == nullptr) return nullptr;

    if (tok.id >= TOKEN_LESS && tok.id <= TOKEN_NOT_EQUAL) {
        int cmp_line = tok.line;
        int cmp_col = tok.col;

        // SECOND CHECK NEEDS REFINEMENT - As of rn, Variables can only be int4... upon expansion 
        //                                 make sure this checks a variable CNode is int4 type
        if (!(left_expr->get_node_type() == CNODE_INT || left_expr->get_node_type() == CNODE_VAR)) {
            print_error(Error{NCC_REL_EXPR_INT4, cmp_line, cmp_col});
            get_token(tok);
            return nullptr;
        }

        auto rel_id = tok.id;
        get_token(tok);
        auto right_expr = parse_arith_expr();
        if (right_expr == nullptr) {
            return nullptr;
        }

        // SECOND CHECK NEEDS REFINEMENT - As of rn, Variables can only be int4... upon expansion 
        //                                 make sure this checks a variable CNode is int4 type
        else if (!(right_expr->get_node_type() == CNODE_INT || right_expr->get_node_type() == CNODE_VAR)) {
            print_error(Error{NCC_REL_EXPR_INT4, cmp_line, cmp_col});
            get_token(tok);
            return nullptr;
        }

        switch (rel_id) {
        case TOKEN_LESS:          return make_unique<LessNode>         (std::move(left_expr), std::move(right_expr));
        case TOKEN_LESS_EQ:       return make_unique<LessEqualNode>    (std::move(left_expr), std::move(right_expr));
        case TOKEN_GREATER:       return make_unique<GreaterNode>      (std::move(left_expr), std::move(right_expr));
        case TOKEN_GREATER_EQ:    return make_unique<GreaterEqualNode> (std::move(left_expr), std::move(right_expr));
        case TOKEN_EQUAL:         return make_unique<EqualNode>        (std::move(left_expr), std::move(right_expr));
        case TOKEN_NOT_EQUAL:     return make_unique<NotEqualNode>     (std::move(left_expr), std::move(right_expr));
        default:                  break;
        }
    }

    return left_expr;
}