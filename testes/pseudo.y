%{
#include <stdio.h>    
#include <stdlib.h>    
#include "tree.h"
#include "stack.h"

extern FILE *yyin;

int yylex(void);
void yyerror(const char *);

const char *token_name_int(int t);

int valores[26];

Stack *stack = NULL;

%}

%code requires {                                                                                                                                                                            
    #include "tree.h"                                                                                                                                                                       
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

// simbolos
%token <texto>TOKEN_ID
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
    TreeNode *atribuicao = create_node(456);
    add_child(atribuicao, $1);
    add_child(atribuicao, $2);
    $$ = atribuicao;
}
    | declaracao {
        printf("DECLARACAO\n");
        TreeNode *atribuicao = create_node(100);
        add_child(atribuicao, $1);
        $$ = atribuicao;
    }
    ;

declaracao: atribuicao TOKEN_PONTO_VIRGULA { $$ = $1; }
    | enquanto { $$ = $1; }
    /*| se  */
    ;

atribuicao: valor TOKEN_ATRIBUICAO valor {
    printf("ATRIBUICAO\n");
    TreeNode *atribuicao = create_node(TOKEN_ATRIBUICAO);
    add_child(atribuicao, $1);
    add_child(atribuicao, $3);
    $$ = atribuicao;
}
    ;

enquanto: TOKEN_ENQUANTO condicao TOKEN_FACA sequencia TOKEN_FIMENQUANTO {
    printf("ENQUANTO\n");
    TreeNode *atribuicao = create_node(22);
    add_child(atribuicao, $2);
    add_child(atribuicao, $4);
    $$ = atribuicao;
    }
    ;
/*
se: TOKEN_SE condicao TOKEN_ENTAO sequencia TOKEN_FIMSE {
        printf("(se) condicao deu %s\n", $2 ? "VERDADEIRO" : "FALSO");
    }
    | TOKEN_SE condicao TOKEN_ENTAO sequencia TOKEN_SENAO sequencia TOKEN_FIMSE {printf("(senao) condicao deu %s\n", $2 ? "VERDADEIRO" : "FALSO");}
    ;*/

condicao: TOKEN_PAR_ESQUERDO valor TOKEN_IGUAL valor TOKEN_PAR_DIREITO { $$ = create_node(31); }
    | TOKEN_PAR_ESQUERDO valor TOKEN_MAIORQUE valor TOKEN_PAR_DIREITO { $$ = create_node(32); }
    | TOKEN_PAR_ESQUERDO valor TOKEN_MENORQUE valor TOKEN_PAR_DIREITO { $$ = create_node(33); }
    ; 

valor: TOKEN_ID { printf("ID = %c\n", (char)$1[0]); $$ = create_node((int)$1[0]); }
    | TOKEN_INT { printf("INT\n"); $$ = create_node($1); }
    /*    | valor TOKEN_ADICAO valor {$$ = $1 + $3;} 
    | valor TOKEN_SUBTRACAO valor {$$ = $1 - $3;} 
    | valor TOKEN_MULTIPLICACAO valor {$$ = $1 * $3;} 
    | valor TOKEN_DIVISAO valor {$$ = $1 / $3;}  */
    ;

%%


void yyerror(const char *s)
{
    printf("Erro: %s\n", s);
}

const char *token_name_int(int t){
    char *buffer = malloc(sizeof(int) * 20); // Certifique-se de que o array é grande o suficiente

    snprintf(buffer, 20, "%d", t); 
    return buffer;
}

int main(int argc, char **argv) {
    stack = stack_create();


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