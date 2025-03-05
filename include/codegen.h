#pragma once

#include "parse.h"

void codegen_init();

void codegen_cleanup();

void codegen_gen(CNode*);

void codegen_run();