%{
#include <stdio.h>    

extern FILE *yyin;

int yylex(void);
void yyerror(const char *);

int valores[26];

%}

%union{
    char *texto;
    int inteiro;
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

%type <inteiro> valor
%type <inteiro> condicao

%%
inicio: 
    | sequencia 
    ;

sequencia: sequencia declaracao
    | declaracao
    ;

declaracao: atribuicao TOKEN_PONTO_VIRGULA
    | enquanto 
    | se 
    ;

atribuicao: TOKEN_ID TOKEN_ATRIBUICAO valor {
    valores[(int)$1[0]] = $3; 
    printf("ID: %s = %d\n", $1, $3);
}
    ;

enquanto: TOKEN_ENQUANTO condicao TOKEN_FACA sequencia TOKEN_FIMENQUANTO {printf("while\n");}
    ;

se: TOKEN_SE condicao TOKEN_ENTAO sequencia TOKEN_FIMSE {
        printf("(se) condicao deu %s\n", $2 ? "VERDADEIRO" : "FALSO");
    }
    | TOKEN_SE condicao TOKEN_ENTAO sequencia TOKEN_SENAO sequencia TOKEN_FIMSE {printf("(senao) condicao deu %s\n", $2 ? "VERDADEIRO" : "FALSO");}
    ;

condicao: TOKEN_PAR_ESQUERDO valor TOKEN_IGUAL valor TOKEN_PAR_DIREITO { $$ = ($2 == $4); }
    | TOKEN_PAR_ESQUERDO valor TOKEN_MAIORQUE valor TOKEN_PAR_DIREITO { $$ = ($2 > $4); }
    | TOKEN_PAR_ESQUERDO valor TOKEN_MENORQUE valor TOKEN_PAR_DIREITO { $$ = ($2 < $4); }
    ;

valor: valor TOKEN_ADICAO valor {$$ = $1 + $3;} 
    | valor TOKEN_SUBTRACAO valor {$$ = $1 - $3;} 
    | valor TOKEN_MULTIPLICACAO valor {$$ = $1 * $3;} 
    | valor TOKEN_DIVISAO valor {$$ = $1 / $3;} 
    | TOKEN_ID { $$ = valores[(int)$1[0]]; }
    | TOKEN_INT
    ;

%%


void yyerror(const char *s)
{
    printf("Erro: %s\n", s);
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

    return 0;
}