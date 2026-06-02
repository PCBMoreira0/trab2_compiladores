%{
#include <stdio.h>    
#include <stdlib.h> 

extern FILE *yyin;

int yylex(void);
void yyerror(const char *);

%}

%code requires {
    #include "../../libs/ast.h"
    #include "../../libs/translator.h"
}

%union{
    char *texto;
    int inteiro;
    ASTNode *ast_node;
}

// fluxo
%token TOKEN_SE
%token TOKEN_SENAO
%token TOKEN_ENTAO
%token TOKEN_ENQUANTO
%token TOKEN_FIMENQUANTO
%token TOKEN_FIMSE
%token TOKEN_FACA

// operadores
%token TOKEN_IGUAL
%token TOKEN_MENORQUE
%token TOKEN_MAIORQUE
%token TOKEN_ADICAO
%token TOKEN_SUBTRACAO
%token TOKEN_MULTIPLICACAO
%token TOKEN_DIVISAO
%token TOKEN_ATRIBUICAO

// símbolos
%token <texto> TOKEN_ID
%token <inteiro> TOKEN_INT
%token TOKEN_PAR_ESQUERDO
%token TOKEN_PAR_DIREITO
%token TOKEN_PONTO_VIRGULA

%type <ast_node> lista_comandos
%type <ast_node> comando
%type <ast_node> atribuicao
%type <ast_node> enquanto
%type <ast_node> se_entao
%type <ast_node> condicao
%type <ast_node> expressao
%type <ast_node> termo
%type <ast_node> fator

%%

programa
    : lista_comandos { 
        ast_print($1, 0); 
        printf("\n\n"); 
        translate($1, stdout, 0);
        }
    ;

lista_comandos
    : comando { $$ = $1; }
    | lista_comandos comando { $$ = ast_create_block($1, $2); }
    ;

comando
    : atribuicao { $$ = $1; }
    | enquanto { $$ = $1; }
    | se_entao { $$ = $1; }
    ;

atribuicao
    : TOKEN_ID TOKEN_ATRIBUICAO expressao TOKEN_PONTO_VIRGULA {
        $$ = ast_create_assign_stmt($1, $3);}
    ;

enquanto
    : TOKEN_ENQUANTO TOKEN_PAR_ESQUERDO condicao TOKEN_PAR_DIREITO TOKEN_FACA lista_comandos TOKEN_FIMENQUANTO { $$ = ast_create_while_stmt($3, $6);}
    ;

se_entao
    : TOKEN_SE TOKEN_PAR_ESQUERDO condicao TOKEN_PAR_DIREITO TOKEN_ENTAO lista_comandos TOKEN_FIMSE { $$ = ast_create_if_stmt($3, $6, NULL); }
    | TOKEN_SE TOKEN_PAR_ESQUERDO condicao TOKEN_PAR_DIREITO TOKEN_ENTAO lista_comandos TOKEN_SENAO lista_comandos TOKEN_FIMSE { $$ = ast_create_if_stmt($3, $6, $8); }
    ;

condicao
    : expressao TOKEN_IGUAL expressao { $$ = ast_create_op('=', $1, $3); }
    | expressao TOKEN_MAIORQUE expressao { $$ = ast_create_op('>', $1, $3); }
    | expressao TOKEN_MENORQUE expressao { $$ = ast_create_op('<', $1, $3); }
    ;

/* --- INÍCIO DA MATEMÁTICA --- */

expressao
    : expressao TOKEN_ADICAO termo { $$ = ast_create_op('+', $1, $3); }
    | expressao TOKEN_SUBTRACAO termo { $$ = ast_create_op('-', $1, $3); }
    | termo { $$ = $1; }
    ;

termo
    : termo TOKEN_MULTIPLICACAO fator { $$ = ast_create_op('*', $1, $3); }
    | termo TOKEN_DIVISAO fator { $$ = ast_create_op('/', $1, $3); }
    | fator { $$ = $1; }
    ;

fator
    : TOKEN_ID { $$ = ast_create_identifier($1); }
    | TOKEN_INT { $$ = ast_create_literal_int($1); }
    | TOKEN_PAR_ESQUERDO expressao TOKEN_PAR_DIREITO { $$ = $2; }
    ;

%%

void yyerror(const char *s)
{
    printf("Erro: %s\n", s);
}

char *itoa(int t) {
    char *num_str = malloc(20);
    snprintf(num_str, 20, "%d", t);
    return num_str;
}

const char *token_name_int(void *t){
    return (const char *)t;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *arquivo = fopen(argv[1], "r");
        if (!arquivo) {
            perror("Erro ao abrir o arquivo");
            return 1; 
        }
        
        yyin = arquivo; 
    } else {
        printf("Nenhum arquivo passado. Lendo do teclado (Ctrl+D para sair):\n");
    }

    yyparse();

    if (yyin != stdin) {
        fclose(yyin);
    }

    printf("fim\n");

    return 0;
}