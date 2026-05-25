%{
#include <stdio.h>    
#include <stdlib.h>    
#include "pseudo.tab.h"
%}

%option noyywrap

ID [a-zA-Z]+
INT [0-9]+

%%
se return TOKEN_SE;
entao return TOKEN_ENTAO;
senao return TOKEN_SENAO;
fimse return TOKEN_FIMSE;
enquanto return TOKEN_ENQUANTO;
faca return TOKEN_FACA;
fimenquanto return TOKEN_FIMENQUANTO;


"<" return TOKEN_MENORQUE;
">" return TOKEN_MAIORQUE;
"==" return TOKEN_IGUAL;
"+" return TOKEN_ADICAO;
"-" return TOKEN_SUBTRACAO;
"*" return TOKEN_MULTIPLICACAO;
"/" return TOKEN_DIVISAO;

"=" return TOKEN_ATRIBUICAO;


")" return TOKEN_PAR_DIREITO;
"(" return TOKEN_PAR_ESQUERDO;

";" return TOKEN_PONTO_VIRGULA;

{ID} { yylval.texto = strdup(yytext); return TOKEN_ID; }
{INT} { yylval.inteiro = atoi(yytext); return TOKEN_INT; }

[ \t\n]+		;

. printf("token não reconhecido: %s\n", yytext);

%%

/* int main(int argc, char **argv) {
    printf("Digite seu codigo (Ctrl+D para encerrar):\n");
    
    // yylex() é a função gerada pelo Flex que inicia a leitura dos tokens 
    yylex(); 
    
    return 0;
} 
*/

/* Função necessária pelo Flex para saber o que fazer ao fim do arquivo */
/* int yywrap() {
    return 1;
} */