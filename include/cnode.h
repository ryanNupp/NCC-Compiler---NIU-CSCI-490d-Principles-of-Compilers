#ifndef CNODE_H
#define CNODE_H

#include <memory>
#include <string>
#include <vector>
#include <variant>

#include "tables.h"

using std::string, std::vector, std::unique_ptr;

//using ValueType = std::variant<int32_t, string>;

enum CNodeType {
    CNODE,

    // statement block
    CNODE_STMT_BLOCK,

    // statements
    CNODE_PRINT,
    CNODE_READ,
    CNODE_IF,
    CNODE_ELSE,
    CNODE_WHILE,
    CNODE_VAR_DECL,
    CNODE_VAR_ASSIG,

    // logical expressions
    CNODE_OR,
    CNODE_AND,
    CNODE_NOT,

    // relational expressions
    CNODE_LESS,
    CNODE_LESS_EQ,
    CNODE_GREATER,
    CNODE_GREATER_EQ,
    CNODE_EQUAL,
    CNODE_NOT_EQ,

    // arithmetic expressions
    CNODE_ADD,
    CNODE_SUB,
    CNODE_MULT,
    CNODE_DIV,
    CNODE_MOD,
    CNODE_POW,
    CNODE_NEG,
    CNODE_POS,

    // values
    CNODE_INT,
    CNODE_STR,
    CNODE_BOOL,
    CNODE_VAR
};

// CNode
//
class CNode {
public:
    virtual ~CNode() = default;
    virtual void print(int) const;
    virtual void gen_node_code(char*&, int&, SymbolTable&) = 0;
    virtual CNodeType get_node_type() const = 0;
};

///////////////////////////////////////////////////////////////////////////////
//                              STATEMENT BLOCK                              //
///////////////////////////////////////////////////////////////////////////////

// A statement block can contain multiple statements (or just one)
//
class StatementBlockNode : public CNode {
private:
    vector<unique_ptr<CNode>> statements;

public:
    StatementBlockNode(vector<unique_ptr<CNode>>);
    void print(int) const override;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};

///////////////////////////////////////////////////////////////////////////////
//                                STATEMENTS                                 //
///////////////////////////////////////////////////////////////////////////////

// Print statement can have any (nonzero positive) number of expressions:
//
// print(expression);
// print(expression, expression, expression...);
//
class PrintNode : public CNode {
private:
    vector<unique_ptr<CNode>> expressions;

public:
    PrintNode(vector<unique_ptr<CNode>>);

    void print(int) const override;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};

// Read statement reads input and stores in a declared variable:
//
// read(variable);
//
class ReadNode : public CNode {
private:
    unique_ptr<CNode> var;

public:
    ReadNode(unique_ptr<CNode>);
    void print(int) const override;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};

// If statement
//
class IfNode : public CNode {
private:
    unique_ptr<CNode> logic_expr, if_body, else_stmt;

public:
    IfNode(unique_ptr<CNode>, unique_ptr<CNode>, unique_ptr<CNode>);
    void print(int) const override;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};

// Else statement
//
class ElseNode : public CNode {
private:
    unique_ptr<CNode> else_body;

public:
    ElseNode(unique_ptr<CNode>);
    void print(int) const override;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};

// While statement
//
class WhileNode : public CNode {
private:
    unique_ptr<CNode> logic_expr, while_body;

public:
    WhileNode(unique_ptr<CNode>, unique_ptr<CNode>);
    void print(int) const override;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};

// Declare a variable given a type & the name:
//
// (only int4 for now)
// int4 a;
//
class VarDeclareNode : public CNode {
private:
    string var_name;

public:
    VarDeclareNode(string);
    void print(int) const override;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};

// Assign a value to a variable:
//
// a <- expr;
//
class VarAssignNode : public CNode {
private:
    string var_name;
    unique_ptr<CNode> expr;

public:
    VarAssignNode(string, unique_ptr<CNode>);
    void print(int) const override;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};

///////////////////////////////////////////////////////////////////////////////
//                                EXPRESSIONS                                //
///////////////////////////////////////////////////////////////////////////////

// Binary Expressions
//
class BinaryExpr : public CNode {
public:
    BinaryExpr(unique_ptr<CNode>, unique_ptr<CNode>);
    void print(int) const override;

protected:
    unique_ptr<CNode> left_expr, right_expr;
    void gen_left_right_code(char*&, int&, SymbolTable&);
};

// Unary Expressions
//
class UnaryExpr : public CNode {
public:
    UnaryExpr(unique_ptr<CNode>);
    void print(int) const override;

protected:
    unique_ptr<CNode> val_expr;
    void gen_val_code(char*&, int&, SymbolTable&);
};

///////////////////////////////////////////////////////////////////////////////
//                            LOGICAL EXPRESSIONS                            //
///////////////////////////////////////////////////////////////////////////////

// Or
//
class OrNode : public BinaryExpr {
public:
    using BinaryExpr::BinaryExpr;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};

// And
//
class AndNode: public BinaryExpr {
public:
    using BinaryExpr::BinaryExpr;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};

// Not
//
class NotNode : public UnaryExpr {
public:
    using UnaryExpr::UnaryExpr;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};

///////////////////////////////////////////////////////////////////////////////
//                           RELATIONAL EXPRESSIONS                          //
///////////////////////////////////////////////////////////////////////////////

enum class RelateExprType {
    LESS = CNODE_LESS,
    LESS_EQ = CNODE_LESS_EQ,
    GREATER = CNODE_GREATER,
    GREATER_EQ = CNODE_GREATER_EQ,
    EQUAL = CNODE_EQUAL,
    NOT_EQ = CNODE_NOT_EQ
};

class RelateExprNode : public BinaryExpr {
private:
    RelateExprType type;

public:
    RelateExprNode(unique_ptr<CNode>, unique_ptr<CNode>, RelateExprType);
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};

///////////////////////////////////////////////////////////////////////////////
//                           ARITHMETIC EXPRESSIONS                          //
///////////////////////////////////////////////////////////////////////////////

// Add
//
class AddNode : public BinaryExpr {
public:
    using BinaryExpr::BinaryExpr;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};


// Subtract
//
class SubtractNode : public BinaryExpr {
public:
    using BinaryExpr::BinaryExpr;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};


// Multiply
//
class MultiplyNode : public BinaryExpr {
public:
    using BinaryExpr::BinaryExpr;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};


// Divide
//
class DivideNode : public BinaryExpr {
public:
    using BinaryExpr::BinaryExpr;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};


// Mod - Modulo
//
class ModNode : public BinaryExpr {
public:
    using BinaryExpr::BinaryExpr;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};


// Powers - Exponentiation
//
class PowerNode : public BinaryExpr {
public:
    using BinaryExpr::BinaryExpr;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};


// Unary minus - negative
//
class NegativeNode : public UnaryExpr {
public:
    using UnaryExpr::UnaryExpr;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};


// Unary plus - positive
//
class PositiveNode : public UnaryExpr {
public:
    using UnaryExpr::UnaryExpr;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};

///////////////////////////////////////////////////////////////////////////////
//                                  VALUES                                   //
///////////////////////////////////////////////////////////////////////////////

// Integer
//
class IntegerNode : public CNode {
private:
    long long int int_value;

public:
    IntegerNode(long long int);
    void print(int) const override;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};


// String
//
class StringNode : public CNode {
private:
    char* string_val; // pointer to string value in string table

public:
    StringNode(char*);
    void print(int) const override;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};


// Bool
//
class BoolNode : public CNode {
private:
    bool bool_val;

public:
    BoolNode(bool);
    void print(int) const override;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};


// Variables
//
class VariableNode : public CNode {
private:
    string var_name;

public:
    VariableNode(string);
    void print(int) const override;
    void gen_node_code(char*&, int&, SymbolTable&) override;
    CNodeType get_node_type() const override;
};

#endif