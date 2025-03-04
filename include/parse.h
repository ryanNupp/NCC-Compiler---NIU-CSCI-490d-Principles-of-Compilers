#pragma once

#include "token.h"

enum CNode_Type {
    CNODE_ADD,
    CNODE_SUBT,
    CNODE_MULT,
    CNODE_DIV,
    CNODE_MOD,
    CNODE_EXP,
    CNODE_NEG,
    CNODE_POS,
    CNODE_VAL,
    CNODE_IDENT
};

struct CNode {
    CNode_Type type;
    long long int val;

    CNode* left;
    CNode* right;
};

extern CNode* top_node;

void parse(Token&);
void parse_tree_printout();
void parse_tree_cleanup();
