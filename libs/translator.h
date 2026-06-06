#pragma once
#include "scheme_ast.h"
#include <stdio.h>

void translate(ASTNode *node, FILE *file, FILE *file_defs, int identation);

const char **translator_get_errors(void);
int translator_get_error_count(void);