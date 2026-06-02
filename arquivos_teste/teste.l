%{
#include <stdio.h>
#include <stdlib.h>

/* IMPORTANTÍSSIMO: Inclui o cabeçalho gerado pelo Bison. 
   É ele quem define o que é NUMERO, MAIS, MENOS, etc. */
#include "calculadora.tab.h"
%}

/* Macro para identificar dígitos */
DIGITO    [0-9]

%%

    /* Quando encontrar um ou mais dígitos */
{DIGITO}+        { 
                    /* Converte o texto capturado (yytext) para inteiro 
                       e envia para a variável de valor do Bison (yylval) */
                    yylval = atoi(yytext); 
                    return NUMERO; 
                 }

    /* Operadores matemáticos */
"+"              { return MAIS; }
"-"              { return MENOS; }
"*"              { return VEZES; }
"/"              { return DIVIDIR; }

    /* Quebra de linha sinaliza o fim de uma expressão para a calculadora */
"\n"             { return FIM_LINHA; }

    /* Ignora espaços em branco e tabulações */
[ \t]            { /* Faz nada, apenas pula */ }

    /* Qualquer outro caractere inesperado (erro léxico) */
.                { printf("Caractere invalido: %s\n", yytext); }

%%

/* Função padrão do Flex para encerramento de leitura */
int yywrap() {
    return 1;
}