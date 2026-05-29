#pragma once
#include "ast.h"
#include <stdio.h>

void translate(ASTNode *node, FILE *file, int identation);