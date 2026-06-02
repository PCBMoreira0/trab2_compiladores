%{
#include <stdio.h>
#include <stdlib.h>

/* Declaração de funções externas */
extern int yylex(); 
void yyerror(const char *s);
%}

/* Declaração dos Tokens que o Flex vai nos enviar */
%token NUMERO
%token MAIS MENOS VEZES DIVIDIR
%token FIM_LINHA

/* Regras de precedência matemática (multiplicação antes da soma) */
%left MAIS MENOS
%left VEZES DIVIDIR

%%
/* SEÇÃO DE REGRAS GRAMATICAIS (SINTAXE) */

/* Nosso programa é uma lista de expressões */
programa:
    | programa expressao FIM_LINHA { printf("Resultado: %d\n", $2); }
    ;

/* Definindo o que é uma "expressão" */
expressao:
      NUMERO                  { $$ = $1; } /* Se for só um número, o valor da expressão é o número */
    | expressao MAIS expressao    { $$ = $1 + $3; } /* Soma o lado esquerdo ($1) com o direito ($3) */
    | expressao MENOS expressao   { $$ = $1 - $3; }
    | expressao VEZES expressao   { $$ = $1 * $3; }
    | expressao DIVIDIR expressao { $$ = $1 / $3; }
    ;

%%
/* SEÇÃO DE CÓDIGO DO USUÁRIO */

/* Função chamada quando o Bison encontra um erro gramatical */
void yyerror(const char *s) {
    fprintf(stderr, "Erro de sintaxe: %s\n", s);
}

int main() {
    printf("Digite uma conta matematia:\n");
    /* yyparse() é a função gerada pelo Bison. Ela vai chamar o yylex() do Flex automaticamente! */
    yyparse();
    return 0;
}