
%{
    #include <stdio.h>
    #include "algo.tab.h"
%}

%option noyywrap

DIGITO [0-9]
LETRA [a-zA-Z]


%%

[ \t]  { }
if { return IF; }
oi { return OI; }
\n return '\n';

{DIGITO}+ { printf("É UM INT: %s\n", yytext); }
{LETRA}+ { printf("É UM ID: %s\n", yytext); }

. { printf("Nao identificado"); }

%%
/* 
int main(int argc, char **argv) {
    printf("Digite seu codigo (Ctrl+D para encerrar):\n");
    
    // yylex() é a função gerada pelo Flex que inicia a leitura dos tokens 
    yylex(); 
    
    return 0;
}  */
