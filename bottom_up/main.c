#include <stdio.h>
#include <stdlib.h>
#include "scheme_test.tab.h"


YYSTYPE yylval;

extern int yylex();
extern char* yytext;

int main() {
    int token;
    printf("Digite o código Scheme (Ctrl+D para sair):\n");
    
    
    while ((token = yylex()) != 0) {
        printf("-> Recebi o TOKEN numero: %d | Texto capturado: %s\n", token, yytext);
        
        
        if (token >= TOKEN_BOOLEAN && token <= TOKEN_IDENTIFIER) {
            free(yylval.string_val);
        }
    }
    
    printf("\nFim da leitura!\n");
    return 0;
}