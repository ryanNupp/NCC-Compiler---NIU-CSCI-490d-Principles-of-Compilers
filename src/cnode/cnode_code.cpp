#include <iostream>
#include <array>
#include <vector>
#include <cstdint>

#include "cnode.h"
#include "tables.h"

using std::cout, std::cin, std::array, std::vector;


const array<uint8_t, 4> int_addr_to_val{
    0x5b,        // pop rbx
    0x8b, 0x03,  // mov eax, [rbx]
    0x50         // push rax
};

///////////////////////////////////////////////////////////////////////////////
//                              STATEMENT BLOCK                              //
///////////////////////////////////////////////////////////////////////////////

void StatementBlockNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    for (auto& statement : statements) {
        statement->gen_node_code(prog, p_offset, symtbl);
    }
}

///////////////////////////////////////////////////////////////////////////////
//                                STATEMENTS                                 //
///////////////////////////////////////////////////////////////////////////////

// ============================== //
//         Print Statement        //
// ============================== //

// PRINT HELPER FUNCTIONS ---------------------
void print_int_literal(int32_t v)
{
    cout << v;
}

void print_int_var(int32_t& v)
{
    cout << v;
}

void print_str_literal(char* v)
{
    cout << v;
}

void print_bool(bool b)
{
    cout << (b ? "true" : "false");
}

// --------------------------------------------

void PrintNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    for (auto& expr : expressions) {
        expr->gen_node_code(prog, p_offset, symtbl);

        intptr_t print_helper;
        auto expr_type = expr->get_node_type();

        // String Literal
        if (expr_type == CNODE_STR) {
            print_helper = reinterpret_cast<intptr_t>(print_str_literal);
        }

        // Arith Expression
        else if ((expr_type >= CNODE_ADD) && (expr_type <= CNODE_INT)) {
            print_helper = reinterpret_cast<intptr_t>(print_int_literal);
        }

        // Variable -- (only int4 for now...)
        else if (expr_type == CNODE_VAR) {
            // TODO: figure out var type...
            print_helper = reinterpret_cast<intptr_t>(print_int_var);
        }

        // Bools : Literals, Logical Expressions (or, and, not), Relational Expressions (<, <=, >=, >, =, ~=)
        else if (expr_type == CNODE_BOOL || ((expr_type >= CNODE_OR) && (expr_type <= CNODE_NOT_EQ))) {
            print_helper = reinterpret_cast<intptr_t>(print_bool);
        }

        // ??
        else {
            print_helper = 0;
        }

        prog[p_offset++] = 0x5f; // pop rdi
        
        prog[p_offset++] = 0x48; // mov rsi, (print_helper)
        prog[p_offset++] = 0xbe;
        for (unsigned long int i = 0; i < sizeof(intptr_t); i++) {
            prog[p_offset++] = print_helper & 0xff;
            print_helper >>= 8;
        }

        prog[p_offset++] = 0xff; // call rsi
        prog[p_offset++] = 0xd6;
    }
}

// ============================== //
//         Read Statement         //
// ============================== //

// READ HELPER FUNCTIONS ----------------------

void read_int4_var(int32_t& v)
{
    cin >> v;
}

// --------------------------------------------

void ReadNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    var->gen_node_code(prog, p_offset, symtbl);

    intptr_t int_helper = reinterpret_cast<intptr_t>(read_int4_var);

    prog[p_offset++] = 0x5f;  // pop rdi

    prog[p_offset++] = 0x48;  // mov rsi, (print_helper)
    prog[p_offset++] = 0xbe;
    for (unsigned long int i = 0; i < sizeof(intptr_t); i++) {
        prog[p_offset++] = int_helper & 0xff;
        int_helper >>= 8;
    }
    
    prog[p_offset++] = 0xff;  // call rsi
    prog[p_offset++] = 0xd6;
}

// ============================== //
//          If Statement          //
// ============================== //

void IfNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    logic_expr->gen_node_code(prog, p_offset, symtbl);
    prog[p_offset++] = 0x58;  // pop rax
    prog[p_offset++] = 0xa8;  // test al, 1
    prog[p_offset++] = 0x01;
    prog[p_offset++] = 0x0f;  // jz X   --   X: jump amount to be modified later...
    prog[p_offset++] = 0x84;
    prog[p_offset++] = 0x00;
    prog[p_offset++] = 0x00;
    prog[p_offset++] = 0x00;
    prog[p_offset++] = 0x00;
    int else_jump_loc = p_offset - 4;

    if_body->gen_node_code(prog, p_offset, symtbl);
    prog[p_offset++] = 0xe9;  // jmp Y   --   Y: jump amount to be modified later...
    prog[p_offset++] = 0x00;
    prog[p_offset++] = 0x00;
    prog[p_offset++] = 0x00;
    prog[p_offset++] = 0x00;
    int if_jump_loc = p_offset - 4;

    int else_jump_amt = (p_offset) - (else_jump_loc + 4);
    for (int i = 0; i < 4; i++) {
        prog[else_jump_loc++] = else_jump_amt & 0xff;
        else_jump_amt >>= 8;
    }

    if (else_stmt) {
        else_stmt->gen_node_code(prog, p_offset, symtbl);
    }

    int if_jump_amt = (p_offset) - (if_jump_loc + 4);
    for (int i = 0; i < 4; i++) {
        prog[if_jump_loc++] = if_jump_amt & 0xff;
        if_jump_amt >>= 8;
    }
}

// ============================== //
//         Else Statement         //
// ============================== //

void ElseNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    else_body->gen_node_code(prog, p_offset, symtbl);
}

// ============================== //
//         While Statement        //
// ============================== //

void WhileNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    prog[p_offset++] = 0xe9;  // jmp X  -- X: jump amount to be modified later...
    prog[p_offset++] = 0x00;  // <-- cond_jump_offset (starts) HERE
    prog[p_offset++] = 0x00;
    prog[p_offset++] = 0x00;
    prog[p_offset++] = 0x00;
    int cond_jump_offset = p_offset - 4;
    int while_body_offset = p_offset;

    while_body->gen_node_code(prog, p_offset, symtbl);
    int cond_jump_amt = (p_offset - 1) - (cond_jump_offset + 3);
    //std::cout << "\nCOND_JUMP_AMT: " << std::hex << cond_jump_amt << std::dec << "\n";
    for (unsigned long int i = 0; i < 4; i++) {
        prog[cond_jump_offset++] = cond_jump_amt & 0xff;
        cond_jump_amt >>= 8;
    }

    logic_expr->gen_node_code(prog, p_offset, symtbl);
    prog[p_offset++] = 0x58;  // pop rax
    prog[p_offset++] = 0xa8;  // test al, 1
    prog[p_offset++] = 0x01;
    prog[p_offset++] = 0x0f;  // jnz, Y  -- Y;
    prog[p_offset++] = 0x85;
    int body_jump_amt = while_body_offset - (p_offset + 4);
    for (unsigned long int i = 0; i < 4; i++) {
        prog[p_offset++] = body_jump_amt & 0xff;
        body_jump_amt >>= 8;
    }
}

// ============================== //
// Variable Declaration Statement //
// ============================== //

void VarDeclareNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    // int4
    //symtbl.addSymbol(var_name, var_type);
    auto [var_type, val_loc] = symtbl.getSymbol(var_name);

    prog[p_offset++] = 0x31; // xor ebx, ebx
    prog[p_offset++] = 0xdb;

    prog[p_offset++] = 0x48; // mov rax, (val_loc)
    prog[p_offset++] = 0xb8;
    for (unsigned long int i = 0; i < sizeof(intptr_t); i++) {
        prog[p_offset++] = val_loc & 0xff;
        val_loc >>= 8;
    }

    prog[p_offset++] = 0x89; // mov [rax], ebx
    prog[p_offset++] = 0x18;
}

// ============================== //
//  Variable Assignment Statement //
// ============================== //

void VarAssignNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    expr->gen_node_code(prog, p_offset, symtbl);
    if (expr->get_node_type() == CNODE_VAR) {
        for (auto code : int_addr_to_val) {
            prog[p_offset++] = code;
        }
    }

    auto [var_type, val_loc] = symtbl.getSymbol(var_name);

    // int4  -  pop value from stack, given the location, set value @ location to the value from stack

    prog[p_offset++] = 0x48;  // mov rax, (val_loc)
    prog[p_offset++] = 0xb8;
    for (unsigned long int i = 0; i < sizeof(intptr_t); i++) {
        prog[p_offset++] = val_loc & 0xff;
        val_loc >>= 8;
    }

    prog[p_offset++] = 0x5b;  // pop rbx

    prog[p_offset++] = 0x89;  // mov [rax], ebx
    prog[p_offset++] = 0x18;
}

///////////////////////////////////////////////////////////////////////////////
//                                EXPRESSIONS                                //
///////////////////////////////////////////////////////////////////////////////

// ============================== //
//        Binary Expression       //
// ============================== //

void BinaryExpr::gen_left_right_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    left_expr->gen_node_code(prog, p_offset, symtbl);
    if (left_expr->get_node_type() == CNODE_VAR) {
        for (auto code : int_addr_to_val) {
            prog[p_offset++] = code;
        }
    }
    
    right_expr->gen_node_code(prog, p_offset, symtbl);
    if (right_expr->get_node_type() == CNODE_VAR) {
        for (auto code : int_addr_to_val) {
            prog[p_offset++] = code;
        }
    }
}

// ============================== //
//        Unary Expression        //
// ============================== //

void UnaryExpr::gen_val_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    val_expr->gen_node_code(prog, p_offset, symtbl);
    if (val_expr->get_node_type() == CNODE_VAR) {
        for (auto code : int_addr_to_val) {
            prog[p_offset++] = code;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//                            LOGICAL EXPRESSIONS                            //
///////////////////////////////////////////////////////////////////////////////


// ============================== //
//               Or               //
// ============================== //

void OrNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{    
    // gen left_expr code -- place result in rax
    left_expr->gen_node_code(prog, p_offset, symtbl);
    prog[p_offset++] = 0x58;  // pop rax

    // jump to end if left_expr is true (1)
    prog[p_offset++] = 0xa8;  // test al, 1
    prog[p_offset++] = 0x01;
    prog[p_offset++] = 0x0f;  // jnz X   --   X: jump amount to be modified later...
    prog[p_offset++] = 0x85;
    prog[p_offset++] = 0x00;
    prog[p_offset++] = 0x00;
    prog[p_offset++] = 0x00;
    prog[p_offset++] = 0x00;
    auto jnz_operand_loc = p_offset - 4;

    // gen right_expr code -- place result in rax
    right_expr->gen_node_code(prog, p_offset, symtbl);
    prog[p_offset++] = 0x58;  // pop rax

    // whatever the final result, place into the stack
    prog[p_offset++] = 0x50;  // push rax  <-- This is where JNZ will jump to...
    int jnz_amt = (p_offset - 1) - (jnz_operand_loc + 4);
    for (unsigned long int i = 0; i < 4; i++) {
        prog[jnz_operand_loc++] = jnz_amt & 0xff;
        jnz_amt >>= 8;
    }
}

// ============================== //
//               And              //
// ============================== //

void AndNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    // gen left_expr code -- place result in rax
    left_expr->gen_node_code(prog, p_offset, symtbl);
    prog[p_offset++] = 0x58;  // pop rax

    // jump to end if left_expr is false (0)
    prog[p_offset++] = 0xa8;  // test al, 1
    prog[p_offset++] = 0x01;
    prog[p_offset++] = 0x0f;  // jz X   --   X: jump amount to be modified later...
    prog[p_offset++] = 0x84;
    prog[p_offset++] = 0x00;
    prog[p_offset++] = 0x00;
    prog[p_offset++] = 0x00;
    prog[p_offset++] = 0x00;
    auto jz_operand_loc = p_offset - 4;

    // gen right_expr code -- place result in rax
    right_expr->gen_node_code(prog, p_offset, symtbl);
    prog[p_offset++] = 0x58;  // pop rax

    // whatever the final result, place into the stack
    prog[p_offset++] = 0x50;  // push rax  <-- This is where JZ will jump to...
    int jz_amt = (p_offset - 1) - (jz_operand_loc + 4);
    for (unsigned long int i = 0; i < 4; i++) {
        prog[jz_operand_loc++] = jz_amt & 0xff;
        jz_amt >>= 8;
    }
}

// ============================== //
//               Not              //
// ============================== //

void NotNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    val_expr->gen_node_code(prog, p_offset, symtbl);
    prog[p_offset++] = 0x58;  // push rax
    prog[p_offset++] = 0x34;  // xor al, 1
    prog[p_offset++] = 0x01;
    prog[p_offset++] = 0x50;  // pop rax
}

///////////////////////////////////////////////////////////////////////////////
//                           RELATIONAL EXPRESSIONS                          //
///////////////////////////////////////////////////////////////////////////////


void RelateExprNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    BinaryExpr::gen_left_right_code(prog, p_offset, symtbl);

    array<uint8_t, 12> relate_code{
        0x5b,                    // pop rbx
        0x58,                    // pop rax
        0x39, 0xd8,              // cmp eax, ebx
        0x0f, 0x90, 0xc0,        // setcc al   --   modified as necessary below
        0x48, 0x0f, 0xbe, 0xc0,  // movsx rax, al
        0x50                     // push rax
    };

    switch (type) {
    case RelateExprType::LESS:            relate_code[5] = 0x9c;      break;
    case RelateExprType::LESS_EQ:         relate_code[5] = 0x9e;      break;
    case RelateExprType::GREATER:         relate_code[5] = 0x9f;      break;
    case RelateExprType::GREATER_EQ:      relate_code[5] = 0x9d;      break;
    case RelateExprType::EQUAL:           relate_code[5] = 0x94;      break;
    case RelateExprType::NOT_EQ:          relate_code[5] = 0x95;      break;
    }

    for (auto code : relate_code) {
        prog[p_offset++] = code;
    }
}


///////////////////////////////////////////////////////////////////////////////
//                           ARITHMETIC EXPRESSIONS                          //
///////////////////////////////////////////////////////////////////////////////


// ============================== //
//               Add              //
// ============================== //

void AddNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    BinaryExpr::gen_left_right_code(prog, p_offset, symtbl);

    array<uint8_t, 6> add_code{
        0x5b,             // pop rbx
        0x58,             // pop rax
        0x48, 0x01, 0xd8, // add rax, rbx
        0x50,             // push rax
    };

    for (auto code : add_code) {
        prog[p_offset++] = code;
    }
}


// ============================== //
//            Subtract            //
// ============================== //

void SubtractNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    BinaryExpr::gen_left_right_code(prog, p_offset, symtbl);

    array<uint8_t, 6> sub_code{
        0x5b,             // pop rbx
        0x58,             // pop rax
        0x48, 0x29, 0xd8, // sub rax, rbx
        0x50,             // push rax
    };

    for (auto code : sub_code) {
        prog[p_offset++] = code;
    }
}


// ============================== //
//            Multiply            //
// ============================== //

void MultiplyNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    BinaryExpr::gen_left_right_code(prog, p_offset, symtbl);

    array<uint8_t, 7> mult_code{
        0x5b,                    // pop rbx
        0x58,                    // pop rax
        0x48, 0x0f, 0xaf, 0xc3,  // mul rax, rbx
        0x50                     // push rax
    };

    for (auto code : mult_code) {
        prog[p_offset++] = code;
    }
}


// ============================== //
//             Divide             //
// ============================== //

void DivideNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    BinaryExpr::gen_left_right_code(prog, p_offset, symtbl);

    array<uint8_t, 9> div_code{
        0x5b,               // pop rbx
        0x58,               // pop rax
        0x48, 0x31, 0xd2,   // ixor rdx, rdx
        0x48, 0xf7, 0xfb,   // idiv rbx
        0x50                // push rax
    };

    for (auto code : div_code) {
        prog[p_offset++] = code;
    }
}


// ============================== //
//              Mod               //
// ============================== //

void ModNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    BinaryExpr::gen_left_right_code(prog, p_offset, symtbl);

    array<uint8_t, 9> mod_code{
        0x5b,               // pop rbx
        0x58,               // pop rax
        0x48, 0x31, 0xd2,   // ixor rdx, rdx
        0x48, 0xf7, 0xfb,   // idiv rbx
        0x52                // push rdx
    };

    for (auto code : mod_code) {
        prog[p_offset++] = code;
    }
}


// ============================== //
//              Power             //
// ============================== //

void PowerNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    BinaryExpr::gen_left_right_code(prog, p_offset, symtbl);

    array<uint8_t, 46> exp_code{
        0x41, 0x5a,             // pop r10
        0x41, 0x59,             // pop r9
        0x4d, 0x31, 0xc0,       // xor r8, r8
        0x4d, 0x85, 0xd2,       // test r10, r10
        0x7c, 0x1e,             // jl 1EH
        0x41, 0xff, 0xc0,       // inc r8
        0x45, 0x85, 0xd2,       // test r10, r10
        0x74, 0x16,             // je 0x16
        0x41, 0xf7, 0xc2,       // test r10, 0x00000001
        0x01, 0x00, 0x00, 0x00, 
        0x74, 0x04,             // je 4H
        0x45, 0x0f, 0xaf, 0xc1, // imul r8, r9
        0x45, 0x0f, 0xaf, 0xc9, // imul r9, r9
        0x41, 0xd1, 0xfa,       // sar r10, 1
        0xeb, 0xe5,             // jmp -27H
        0x44, 0x89, 0xc0,       // mov rax, r8
        0x50                    // push rax
    };

    for (auto code : exp_code) {
        prog[p_offset++] = code;
    }
}


// ============================== //
//            Negation            //
// ============================== //

void NegativeNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    UnaryExpr::gen_val_code(prog, p_offset, symtbl);

    array<uint8_t, 5> negate_code{
        0x58,              // pop rax
        0x48, 0xf7, 0xd8,  // neg rax
        0x50               // push rax
    };

    for (auto code : negate_code) {
        prog[p_offset++] = code;
    }
}


// ============================== //
//            Positive            //
// ============================== //

void PositiveNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    UnaryExpr::gen_val_code(prog, p_offset, symtbl);
}


///////////////////////////////////////////////////////////////////////////////
//                                  VALUES                                   //
///////////////////////////////////////////////////////////////////////////////


// ============================== //
//             Integer            //
// ============================== //
void IntegerNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    long long int val = int_value;

    prog[p_offset++] = 0x48; // mov rax, (int val) 
    prog[p_offset++] = 0xc7;
    prog[p_offset++] = 0xc0;
    for (int i = 0; i < 4; i++) {
        prog[p_offset++] = val & 0xff;
        val >>= 8;
    }

    prog[p_offset++] = 0x50; // push rax  -  (push value onto the stack)
}


// ============================== //
//             String             //
// ============================== //

void StringNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    intptr_t str_addr = reinterpret_cast<intptr_t>(string_val);

    prog[p_offset++] = 0x48; // mov rax, (address of string)
    prog[p_offset++] = 0xB8;
    for (unsigned long int i = 0; i < sizeof(intptr_t); i++) {
        prog[p_offset++] = str_addr & 0xFF;
        str_addr >>= 8;
    }

    prog[p_offset++] = 0x50; // push rax  -  (push address onto the stack)
}


// ============================== //
//             Boolean            //
// ============================== //

void BoolNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    array<uint8_t, 7> bool_code {
        0xb0, bool_val,          // mov al, (bool_val)
        0x48, 0x0f, 0xbe, 0xc0,  // movsx rax, al
        0x50                     // push rax
    };

    for (auto code : bool_code) {
        prog[p_offset++] = code;
    }
}


// ============================== //
//            Variable            //
// ============================== //

void VariableNode::gen_node_code(char*& prog, int& p_offset, SymbolTable& symtbl)
{
    auto [var_type, val_loc] = symtbl.getSymbol(var_name);

    // if (var_type == "int4")
    prog[p_offset++] = 0x48; // mov rbx, (val_loc)
    prog[p_offset++] = 0xbb;
    for (unsigned long int i = 0; i < sizeof(intptr_t); i++) {
        prog[p_offset++] = val_loc & 0xff;
        val_loc >>= 8;
    }

    prog[p_offset++] = 0x53; // push rbx  -  (push address onto the stack)
}