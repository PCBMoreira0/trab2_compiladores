%{
#include <stdio.h>    
#include <stdlib.h> 

extern FILE *yyin;

int yylex(void);
void yyerror(const char *);

const char *token_name_int(void *t);
char *itoa(int t);
%}

%code requires {
    #include "libs/tree.h"
}

%union{
    char *texto;
    int inteiro;
    TreeNode* tree_node;
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

%left TOKEN_ADICAO TOKEN_SUBTRACAO
%left TOKEN_MULTIPLICACAO TOKEN_DIVISAO

%type <tree_node> valor
%type <tree_node> atribuicao
%type <tree_node> enquanto
%type <tree_node> sequencia
%type <tree_node> declaracao
%type <tree_node> condicao

%%
inicio: 
    | sequencia { print_tree($1, token_name_int); }
    ;

sequencia: sequencia declaracao { 
    printf("SEQUENCIA DECLARACAO\n");
    TreeNode *seq = create_node("sequencia-declaracao");
    add_child(seq, $1);
    add_child(seq, $2);
    $$ = seq;
}
    | declaracao {
        printf("DECLARACAO\n");
        TreeNode *declaracao = create_node("declaracao");
        add_child(declaracao, $1);
        $$ = declaracao;
    }
    ;

declaracao: atribuicao TOKEN_PONTO_VIRGULA { $$ = $1; }
    | enquanto { $$ = $1; }
    ;

atribuicao: valor TOKEN_ATRIBUICAO valor {
    printf("ATRIBUICAO %s = %s\n", token_name_int($1->data), token_name_int($3->data));
    TreeNode *atribuicao = create_node("=");
    add_child(atribuicao, $1);
    add_child(atribuicao, $3);
    $$ = atribuicao;
}
    ;

enquanto: TOKEN_ENQUANTO condicao TOKEN_FACA sequencia TOKEN_FIMENQUANTO {
    printf("ENQUANTO\n");
    TreeNode *enquanto = create_node("while");
    add_child(enquanto, $2);
    add_child(enquanto, $4);
    $$ = enquanto;
    }
    ;

condicao: TOKEN_PAR_ESQUERDO valor TOKEN_IGUAL valor TOKEN_PAR_DIREITO { $$ = create_node("=="); }
    | TOKEN_PAR_ESQUERDO valor TOKEN_MAIORQUE valor TOKEN_PAR_DIREITO { $$ = create_node(">"); }
    | TOKEN_PAR_ESQUERDO valor TOKEN_MENORQUE valor TOKEN_PAR_DIREITO { $$ = create_node("<"); }
    ; 

valor: TOKEN_ID { 
        printf("ID = %s\n", $1); 
        $$ = create_node($1); 
    }
    | TOKEN_INT { 
        printf("INT = %d\n", $1);        
        $$ = create_node(itoa($1)); 
    }
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