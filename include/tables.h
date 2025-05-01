#ifndef NCC_TABLES_H
#define NCC_TABLES_H

#include <string>
#include <map>
#include <utility>
#include <variant>
#include <cstdint>
#include <array>

using std::array;
using std::string, std::map, std::pair, std::variant;


////////////////////////
//    SYMBOL TABLE    //
////////////////////////

class SymbolTable {
public:
    using ValueType = variant<int32_t, intptr_t>;

    void addSymbol(const string&, const string&);
    pair<string, intptr_t> getSymbol(const string&);
    bool symbolExists(const string&);

private:
    // KEY: symbol name    VALUE: pair: type, value (pointer to value location in mem)
    map<string, pair<string, intptr_t>> table;

    //int* ints;
    array<int, 1000> ints;
    int curr_int = 0;
};


////////////////////////
//    STRING TABLE    //
////////////////////////

class StringTable {
public:
    char* add_string(string);
    StringTable() { stringtable = new char[5000]; }
    ~StringTable() { delete[] stringtable; }

private:
    //char stringtable[50000];
    char* stringtable;
    int curr_tbl_pos = 0;
};

#endif