#pragma once

#include <string>

#include "error.h"
#include "token.h"

Error lex_init(const char*);
void lex_cleanup();
bool lex_eof();
Error get_token(Token&);
