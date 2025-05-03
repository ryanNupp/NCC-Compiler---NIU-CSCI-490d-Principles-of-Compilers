#include <memory>

#include "cnode.h"
#include "tables.h"

using std::unique_ptr;

// CNode
//
CNodeType CNode::get_node_type() const
{
    return CNODE;
}


///////////////////////////////////////////////////////////////////////////////
//                              STATEMENT BLOCK                              //
///////////////////////////////////////////////////////////////////////////////

StatementBlockNode::StatementBlockNode(vector<unique_ptr<CNode>> statements)
    : statements(std::move(statements))
{}

CNodeType StatementBlockNode::get_node_type() const
{
    return CNODE_STMT_BLOCK;
}


///////////////////////////////////////////////////////////////////////////////
//                                STATEMENTS                                 //
///////////////////////////////////////////////////////////////////////////////


// ============================== //
//         Print Statement        //
// ============================== //

PrintNode::PrintNode(vector<unique_ptr<CNode>> expressions)
    : expressions(std::move(expressions))
{}

CNodeType PrintNode::get_node_type() const
{
    return CNODE_PRINT;
}


// ============================== //
//         Read Statement         //
// ============================== //

ReadNode::ReadNode(unique_ptr<CNode> var)
    : var{std::move(var)}
{}

CNodeType ReadNode::get_node_type() const
{
    return CNODE_READ;
}

// ============================== //
//          If Statement          //
// ============================== //

IfNode::IfNode(unique_ptr<CNode> logic_expr, unique_ptr<CNode> if_body, unique_ptr<CNode> else_stmt)
    : logic_expr{std::move(logic_expr)}
    , if_body{std::move(if_body)}
    , else_stmt{std::move(else_stmt)}
{}

CNodeType IfNode::get_node_type() const
{
    return CNODE_IF;
}

// ============================== //
//         Else Statement         //
// ============================== //

ElseNode::ElseNode(unique_ptr<CNode> else_body)
    : else_body{std::move(else_body)}
{}

CNodeType ElseNode::get_node_type() const
{
    return CNODE_ELSE;
}

// ============================== //
//         While Statement        //
// ============================== //

WhileNode::WhileNode(unique_ptr<CNode> logic_expr, unique_ptr<CNode> while_body)
    : logic_expr{std::move(logic_expr)}
    , while_body{std::move(while_body)}
{}

CNodeType WhileNode::get_node_type() const
{
    return CNODE_WHILE;
}


// ============================== //
// Variable Declaration Statement //
// ============================== //

VarDeclareNode::VarDeclareNode(string var_name)
    : var_name{var_name}
{}

CNodeType VarDeclareNode::get_node_type() const
{
    return CNODE_VAR_DECL;
}


// ============================== //
//  Variable Assignment Statement //
// ============================== //

VarAssignNode::VarAssignNode(string var_name, unique_ptr<CNode> expr)
    : var_name{var_name}
    , expr{std::move(expr)}
{}

CNodeType VarAssignNode::get_node_type() const
{
    return CNODE_VAR_ASSIG;
}


///////////////////////////////////////////////////////////////////////////////
//                                EXPRESSIONS                                //
///////////////////////////////////////////////////////////////////////////////


// ============================== //
//        Binary Expression       //
// ============================== //

BinaryExpr::BinaryExpr(unique_ptr<CNode> left_expr, unique_ptr<CNode> right_expr)
    : left_expr{std::move(left_expr)}
    , right_expr{std::move(right_expr)}
{}


// ============================== //
//        Unary Expression        //
// ============================== //

UnaryExpr::UnaryExpr(unique_ptr<CNode> val_expr)
    : val_expr{std::move(val_expr)}
{}


///////////////////////////////////////////////////////////////////////////////
//                            LOGICAL EXPRESSIONS                            //
///////////////////////////////////////////////////////////////////////////////


// ============================== //
//               Or               //
// ============================== //

CNodeType OrNode::get_node_type() const
{
    return CNODE_OR;
}


// ============================== //
//               And              //
// ============================== //

CNodeType AndNode::get_node_type() const
{
    return CNODE_AND;
}


// ============================== //
//               Not              //
// ============================== //

CNodeType NotNode::get_node_type() const
{
    return CNODE_NOT;
}


///////////////////////////////////////////////////////////////////////////////
//                           RELATIONAL EXPRESSIONS                          //
///////////////////////////////////////////////////////////////////////////////

RelateExprNode::RelateExprNode(unique_ptr<CNode> left_expr, unique_ptr<CNode> right_expr, RelateExprType type)
    : BinaryExpr(std::move(left_expr), std::move(right_expr))
    , type{type}
{}

CNodeType RelateExprNode::get_node_type() const
{
    return static_cast<CNodeType>(type);
}


///////////////////////////////////////////////////////////////////////////////
//                           ARITHMETIC EXPRESSIONS                          //
///////////////////////////////////////////////////////////////////////////////


// ============================== //
//               Add              //
// ============================== //

CNodeType AddNode::get_node_type() const
{
    return CNODE_ADD;
}


// ============================== //
//            Subtract            //
// ============================== //

CNodeType SubtractNode::get_node_type() const
{
    return CNODE_SUB;
}


// ============================== //
//            Multiply            //
// ============================== //

CNodeType MultiplyNode::get_node_type() const
{
    return CNODE_MULT;
}


// ============================== //
//             Divide             //
// ============================== //

CNodeType DivideNode::get_node_type() const
{
    return CNODE_DIV;
}


// ============================== //
//              Mod               //
// ============================== //

CNodeType ModNode::get_node_type() const
{
    return CNODE_MOD;
}


// ============================== //
//              Power             //
// ============================== //

CNodeType PowerNode::get_node_type() const
{
    return CNODE_POW;
}


// ============================== //
//            Negation            //
// ============================== //

CNodeType NegativeNode::get_node_type() const
{
    return CNODE_NEG;
}


// ============================== //
//            Positive            //
// ============================== //

CNodeType PositiveNode::get_node_type() const
{
    return CNODE_POS;
}


///////////////////////////////////////////////////////////////////////////////
//                                  VALUES                                   //
///////////////////////////////////////////////////////////////////////////////


// ============================== //
//             Integer            //
// ============================== //

IntegerNode::IntegerNode(long long int int_value)
    : int_value{int_value}
{}

CNodeType IntegerNode::get_node_type() const
{
    return CNODE_INT;
}


// ============================== //
//             String             //
// ============================== //

StringNode::StringNode(char* string_val)
    : string_val{string_val}
{}

CNodeType StringNode::get_node_type() const
{
    return CNODE_STR;
}


// ============================== //
//             Boolean            //
// ============================== //

BoolNode::BoolNode(bool bool_val)
    : bool_val{bool_val}
{}

CNodeType BoolNode::get_node_type() const
{
    return CNODE_BOOL;
}


// ============================== //
//            Variable            //
// ============================== //

VariableNode::VariableNode(string var_name)
    : var_name{var_name}
{}

CNodeType VariableNode::get_node_type() const
{
    return CNODE_VAR;
}