#include <endian.h>
#include <sys/mman.h>

#include <iostream>

#include "codegen.h"

using std::cout;


char* prog;
int p_offset;

void codegen_init() {
    prog = (char *) mmap(0, 50000,
        PROT_EXEC | PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS, -1 , 0);
    p_offset = 0;
}

void codegen_cleanup() {
    munmap(0, 50000);
}


void codegen_gen(CNode* node) {
    if (node == nullptr)
        return;

    codegen_gen(node->left);
    codegen_gen(node->right);

    switch (node->type) {
    case CNODE_VAL:
        prog[p_offset++] = 0x48;  // mov rax
        prog[p_offset++] = 0xc7;
        prog[p_offset++] = 0xc0;
        prog[p_offset++] = (node->val & 0xff); node->val >>= 8;
        prog[p_offset++] = (node->val & 0xff); node->val >>= 8;
        prog[p_offset++] = (node->val & 0xff); node->val >>= 8;
        prog[p_offset++] = (node->val & 0xff);
        prog[p_offset++] = 0x50;
        break;

    case CNODE_NEG:
        prog[p_offset++] = 0x58;
        prog[p_offset++] = 0x48;
        prog[p_offset++] = 0xf7;
        prog[p_offset++] = 0xd8;
        prog[p_offset++] = 0x50;
        break;

    case CNODE_ADD:
        prog[p_offset++] = 0x5b;
        prog[p_offset++] = 0x58;
        prog[p_offset++] = 0x48;
        prog[p_offset++] = 0x01;
        prog[p_offset++] = 0xd8;
        prog[p_offset++] = 0x50;
        break;

    case CNODE_SUBT:
        prog[p_offset++] = 0x5b;
        prog[p_offset++] = 0x58;
        prog[p_offset++] = 0x48;
        prog[p_offset++] = 0x29;
        prog[p_offset++] = 0xd8;
        prog[p_offset++] = 0x50;
        break;

    case CNODE_MULT:
        prog[p_offset++] = 0x5b;
        prog[p_offset++] = 0x58;
        prog[p_offset++] = 0x48;
        prog[p_offset++] = 0x0f;
        prog[p_offset++] = 0xaf;
        prog[p_offset++] = 0xc3;
        prog[p_offset++] = 0x50;
        break;

    case CNODE_DIV:
        prog[p_offset++] = 0x5b;
        prog[p_offset++] = 0x58;
        prog[p_offset++] = 0x48;
        prog[p_offset++] = 0x31;
        prog[p_offset++] = 0xd2;
        prog[p_offset++] = 0x48;
        prog[p_offset++] = 0xf7;
        prog[p_offset++] = 0xfb;
        prog[p_offset++] = 0x50;
        break;

    case CNODE_MOD:
        prog[p_offset++] = 0x5b;
        prog[p_offset++] = 0x58;
        prog[p_offset++] = 0x48;
        prog[p_offset++] = 0x31;
        prog[p_offset++] = 0xd2;
        prog[p_offset++] = 0x48;
        prog[p_offset++] = 0xf7;
        prog[p_offset++] = 0xfb;
        prog[p_offset++] = 0x52;
        break;

    case CNODE_EXP:
        prog[p_offset++] = 0x58;
        break;

    default:
        break;
    }
}


void codegen_run() {
    if (top_node != nullptr)
        prog[p_offset++] = 0x58;  // pop rax    
    prog[p_offset++] = 0xc3;  // RET

    cout << "Code size: " << p_offset << " bytes.\n";
    cout << "Code execution:\n";

    long long int output = ((long long int(*)(void)) prog)();
    cout << output << "\n";
}