#pragma once

#include <variant>

#include "parser.h"
#include "cnode.h"
#include "tables.h"

using std::variant;

class Codegen {
public:
    using ValueType = variant<string, uint32_t>;

    Codegen(Parser&, SymbolTable&);
    ~Codegen();

    void generate(unique_ptr<CNode>);
    void run();

private:
    Parser& parser;
    SymbolTable& symtbl;
    char* prog;
    int p_offset = 0;
};