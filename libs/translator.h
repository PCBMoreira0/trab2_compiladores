#pragma once
#include "scheme_ast.h"
#include <stdio.h>

void translate(ASTNode *node, FILE *file, FILE *file_defs, int identation);