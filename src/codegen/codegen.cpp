#include <iostream>
#include <iomanip>

#include <vector>
#include <sys/mman.h>

#include "parser.h"
#include "codegen.h"

#include "disasm.h"

using std::cout, std::endl, std::vector;

Codegen::Codegen(Parser& parser, SymbolTable& symtbl)
    : parser{parser}
    , symtbl{symtbl}
{
    prog = (char *) mmap(0, 50000,
        PROT_EXEC | PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS, -1 , 0);
}

Codegen::~Codegen()
{
    munmap(0, 50000);
}

void Codegen::generate(unique_ptr<CNode> code_tree)
{
    code_tree->gen_node_code(prog, p_offset, symtbl);

    prog[p_offset++] = 0xC3; // RET
}

void Codegen::run()
{
    cout << "Code size: " << p_offset << " bytes.\n";

    // DEBUGGING PURPOSES REMOVE THIS 
    // cout << "\n\nGENERATED CODE:\n";
    // for (int i = 0; i < p_offset; i++) {
    //     cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(prog[i] & 0xFF) << "\n";
    // }
    // cout << "END GEN CODE ----\n\n" << std::dec;
    // ------------------------------

    // disassemble((unsigned char *)prog, p_offset);  cout << "\n";

    cout << "Code execution:\n";

    reinterpret_cast<void(*)()>(prog)();

    cout << '\n' << endl;
}