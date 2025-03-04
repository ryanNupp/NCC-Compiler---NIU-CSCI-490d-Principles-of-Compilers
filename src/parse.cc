#include <iostream>

#include "lex.h"
#include "error.h"
#include "parse.h"

using std::cout;

/*  E ->  E+T  |  E-T  |  T
 *  T ->  T*F  |  T/F  |  TmodF  |  F
 *  F ->  X^F  |  X
 *  X ->  +N   |  -N   |  N
 *  N ->  V    |  (E)  |  ident
 *  V ->  int
 */

void parse_V();
void parse_N();
void parse_X();
void parse_F();
void parse_T();
void parse_E();

CNode* top_node = nullptr;
Token tok;

void parse_V() {
    if (tok.id == TOKEN_INTEGER) {
        top_node = new CNode{CNODE_VAL, tok.int_val, nullptr, nullptr};
        get_token(tok);
    }
    else {
        print_error(Error{NCC_UNEXPECT_SYM, tok.line, tok.col});
        top_node = nullptr;
        get_token(tok);
    }
}

void parse_N() {
    if (tok.id == TOKEN_LPAREN) {
        get_token(tok);
        parse_E();
        if (tok.id != TOKEN_RPAREN) {
            print_error(Error{NCC_EXPECT_SYM, tok.line, tok.col, ')'});
        }
        get_token(tok);
    }
    else {
        parse_V();
    }
}

void parse_X() {
    if (tok.id == TOKEN_PLUS || tok.id == TOKEN_MINUS) {
        CNode* temp_node = new CNode{CNODE_POS, 0, nullptr, nullptr};
        if (tok.id == TOKEN_MINUS)
            temp_node->type = CNODE_NEG;

        get_token(tok);
        parse_N();

        if (top_node == nullptr) {
            delete temp_node;
            return;
        }
        
        temp_node->left = top_node;
        top_node = temp_node;
    }
    else {
        parse_N();
    }
}

void parse_F() {
    parse_X();

    if (tok.id == TOKEN_EXP) {
        CNode* temp_node = new CNode{CNODE_EXP, 0, top_node, nullptr};
        
        get_token(tok);
        parse_F();
        
        temp_node->right = top_node;
        top_node = temp_node;
    }
}

void parse_T() {
    parse_F();

    while (tok.id == TOKEN_MULT || tok.id == TOKEN_DIV || (tok.id == TOKEN_IDENT && tok.string_val == "mod")) {
        CNode* temp_node = new CNode{CNODE_MULT, 0, top_node, nullptr};
        if (tok.id == TOKEN_DIV)
            temp_node->type = CNODE_DIV;
        else if (tok.id == TOKEN_IDENT)
            temp_node->type = CNODE_MOD;
        
        get_token(tok);
        parse_F();
        
        temp_node->right = top_node;
        top_node = temp_node;
    }
}

void parse_E() {
    parse_T();
    if (top_node == nullptr) {
        return;
    }

    while (tok.id == TOKEN_PLUS || tok.id == TOKEN_MINUS) {
        CNode* temp_node = new CNode{CNODE_ADD, 0, top_node, nullptr};
        if (tok.id == TOKEN_MINUS)
            temp_node->type = CNODE_SUBT;
        
        // get next Term
        get_token(tok);
        parse_T();

        if (top_node == nullptr) {
            top_node = temp_node->left;
            delete temp_node;
            return;
        }
        
        // fill out right node to temp_node and set it to be the new top_node
        temp_node->right = top_node;
        top_node = temp_node;
    }
}



// Print parse tree
void print_node(CNode* node, int indent) {
    if (node == nullptr)
        return;

    for (int i=0; i<indent; i++)
        cout << "  ";

    switch (node->type) {
    case CNODE_ADD:        cout << "+ (add)\n";        break;
    case CNODE_SUBT:       cout << "- (subt)\n";       break;
    case CNODE_MULT:       cout << "* (mult)\n";       break;
    case CNODE_DIV:        cout << "/ (div)\n";        break;
    case CNODE_MOD:        cout << "mod\n";            break;
    case CNODE_EXP:        cout << "^ (exp)\n";        break;
    case CNODE_NEG:        cout << "- (neg)\n";        break;
    case CNODE_POS:        cout << "+ (pos)\n";        break;
    case CNODE_VAL:        cout << node->val << "\n";  break;
    case CNODE_IDENT:      cout << "ident\n";          break;
    }

    print_node(node->left, indent+1);
    print_node(node->right, indent+1);
}



// Recursively delete parse tree nodes
void delnode(CNode* node) {
    if (node == nullptr)
        return;
    
    delnode(node->left);
    delnode(node->right);
    delete node;
}




// External access function, handles token input & passoff
//   calls the recursion with functions above,
//   after all the logic is through, extern top_node should have top-most node
void parse(Token& input_token) {
    tok = input_token;
    parse_E();
    input_token = tok;
}

void parse_tree_printout() {
    cout << "Code tree:\n";
    print_node(top_node, 0);
}

void parse_tree_cleanup() {
    delnode(top_node);
    top_node = nullptr;
}