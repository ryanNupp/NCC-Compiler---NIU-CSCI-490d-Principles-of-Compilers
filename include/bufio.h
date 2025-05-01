#pragma once

#include <string>

using std::string;

// externally accessible variables
extern size_t buf_line_pos;
extern size_t buf_col_pos;

// init & cleanup
int buf_init(const char*);
void buf_cleanup();

// move to next char / prev char
int buf_next_char();
int buf_prev_char();

// get curr / next char
int buf_get_curr_char(char&);
int buf_get_next_char(char&);

// check end of file
bool buf_eof();

// get line
string buf_getline(int);