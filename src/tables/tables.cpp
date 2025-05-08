
#include "tables.h"

////////////////////////
//    SYMBOL TABLE    //
////////////////////////

// Add a symbol
void SymbolTable::addSymbol(const string& name, const string& type) {
    if (type == "int4") {
        table[name] = pair{type, reinterpret_cast<intptr_t>(&ints[curr_int])};
    }
    curr_int++;
}

// Retrieve a symbol
pair<string, intptr_t> SymbolTable::getSymbol(const string& name) {
    return table[name];
}

// Check if symbol exists
bool SymbolTable::symbolExists(const string& name)
{
    return table.contains(name);
}


////////////////////////
//    STRING TABLE    //
////////////////////////

char* StringTable::add_string(string str)
{
    char* return_str = &stringtable[curr_tbl_pos];

    for (char ch : str) {
        stringtable[curr_tbl_pos++] = ch;
    }

    stringtable[curr_tbl_pos++] = '\0';

    return return_str;
}