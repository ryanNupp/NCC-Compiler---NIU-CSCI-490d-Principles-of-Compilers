#include <iostream>

#include "cnode.h"

using std::cout, std::cin;

// CNode
//
void CNode::print(int indent) const
{
    cout << string(indent*2, ' ');
}

///////////////////////////////////////////////////////////////////////////////
//                              STATEMENT BLOCK                              //
///////////////////////////////////////////////////////////////////////////////

void StatementBlockNode::print(int indent) const
{
    CNode::print(indent);
    cout << "statement block\n";

    for (auto& statement : statements) {
        statement->print(indent+1);
    }
}

///////////////////////////////////////////////////////////////////////////////
//                                STATEMENTS                                 //
///////////////////////////////////////////////////////////////////////////////

// Print
//
void PrintNode::print(int indent) const
{
    CNode::print(indent);
    cout << "print\n";

    for (auto& expression : expressions) {
        expression->print(indent+1);
    }
}

// Read
//
void ReadNode::print(int indent) const
{
    CNode::print(indent);
    cout << "read\n";

    var->print(indent+1);
}

// If
//
void IfNode::print(int indent) const
{
    CNode::print(indent);
    cout << "if\n";

    CNode::print(indent);
    cout << "(LOGICAL EXPR)\n";
    logic_expr->print(indent+1);

    CNode::print(indent);
    cout << "(BODY)\n";
    if_body->print(indent+1);

    if (else_stmt != nullptr) {
        CNode::print(indent);
        cout << "(ELSE STMT)\n";
        else_stmt->print(indent+1);
    }
}

// Else
//
void ElseNode::print(int indent) const
{
    CNode::print(indent);
    cout << "else\n";

    else_body->print(indent+1);
}

// While
//
void WhileNode::print(int indent) const
{
    CNode::print(indent);
    cout << "while\n";

    while_body->print(indent+1);
}

// Variable declaration
//
void VarDeclareNode::print(int indent) const
{
    CNode::print(indent);
    cout << "variable: " << var_name << "\n";
}

// Variable assignment
//
void VarAssignNode::print(int indent) const
{
    CNode::print(indent);
    cout << "<-\n";
    CNode::print(indent);
    cout << "  variable: " << var_name << '\n';
    expr->print(indent+1);
}

///////////////////////////////////////////////////////////////////////////////
//                                EXPRESSIONS                                //
///////////////////////////////////////////////////////////////////////////////

void BinaryExpr::print(int indent) const
{
    CNode::print(indent);

    switch (get_node_type()) {
    case CNODE_OR:               cout << "|  (or)";                     break;
    case CNODE_AND:              cout << "&  (and)";                    break;
    case CNODE_EQUAL:            cout << "=  (equal)";                  break;
    case CNODE_NOT_EQUAL:        cout << "~= (not equal)";              break;
    case CNODE_LESS:             cout << "<  (less)";                   break;
    case CNODE_GREATER:          cout << ">  (greater)";                break;
    case CNODE_LESS_EQ:          cout << "<=  (less or equal)";         break;
    case CNODE_GREATER_EQ:       cout << ">=  (greater or equal)";      break;
    case CNODE_ADD:              cout << "+  (add)";                    break;
    case CNODE_SUB:              cout << "-  (subt)";                   break;
    case CNODE_MULT:             cout << "*  (mult)";                   break;
    case CNODE_DIV:              cout << "/  (div)";                    break;
    case CNODE_MOD:              cout << "mod";                         break;
    case CNODE_POW:              cout << "^  (pow)";                    break;
    default:                     cout << "?  UNKNOWN BINARY OPERATOR";  break;
    }

    cout << '\n';
    
    left_expr->print(indent+1);
    right_expr->print(indent+1);
}

void UnaryExpr::print(int indent) const
{
    CNode::print(indent);

    switch (get_node_type()) {
    case CNODE_NOT:    cout << "!  (not)";                     break;
    case CNODE_NEG:    cout << "-  (negative)";                break;
    case CNODE_POS:    cout << "+  (positive)";                break;
    default:           cout << "?  UNKNOWN UNARY OPERATOR";    break;
    }

    cout << '\n';

    val_expr->print(indent+1);
}


///////////////////////////////////////////////////////////////////////////////
//                                  VALUES                                   //
///////////////////////////////////////////////////////////////////////////////

// integer literal
//
void IntegerNode::print(int indent) const
{
    CNode::print(indent);
    cout << int_value << "\n";
}

// string literal
//
void StringNode::print(int indent) const
{
    CNode::print(indent);
    cout << string_val << "\n";
}

// boolean literal
//
void BoolNode::print(int indent) const
{
    CNode::print(indent);
    cout << (bool_val ? "true\n" : "false\n");
}

// variable
//
void VariableNode::print(int indent) const
{
    CNode::print(indent);
    cout << "variable: " << var_name << "\n";
}