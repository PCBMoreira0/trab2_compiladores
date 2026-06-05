#pragma once
#include "scheme_ast.h"
#include <stdio.h>

void translate(ASTNode *node, FILE *file, int identation);