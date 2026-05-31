%{
#include <stdio.h>    
#include <stdlib.h> 
int yylex(void);
void yyerror(const char *);
extern FILE *yyin;
%}

%code requires {
    #include "libs/ast.h"
    #include "libs/translator.h"
}

%union {
    char *string_val;
}

%define parse.error verbose

%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_SINGLE_QUOTE
%token TOKEN_BACKQUOTE
%token TOKEN_COMMA
%token TOKEN_COMMA_AT
%token TOKEN_VECTOR_OPEN
%token TOKEN_DOT
%token TOKEN_ARROW

%token TOKEN_DELAY
%token TOKEN_DEFINE     
%token TOKEN_DO    
%token TOKEN_IF     
%token TOKEN_ELSE        
%token TOKEN_LAMBDA         
%token TOKEN_SET            
%token TOKEN_BEGIN          
%token TOKEN_COND    
%token TOKEN_CASE       
%token TOKEN_LET  
%token TOKEN_LET_STAR
%token TOKEN_LET_REC          
%token TOKEN_QUOTE          
%token TOKEN_AND            
%token TOKEN_OR             


%token <string_val> TOKEN_BOOLEAN
%token <string_val> TOKEN_NUMBER
%token <string_val> TOKEN_STRING
%token <string_val> TOKEN_CHARACTER
%token <string_val> TOKEN_IDENTIFIER

%%
program:
    %empty
    | list_command_or_definition_plus
    ;

command_or_definition:
    command
    | definition
    ;

command: expression;


list_command_or_definition_plus:
    command_or_definition
    | list_command_or_definition_plus command_or_definition
    ;

definition: 
    TOKEN_LPAREN TOKEN_DEFINE variable expression TOKEN_RPAREN
    | TOKEN_LPAREN TOKEN_DEFINE TOKEN_LPAREN variable def_formals TOKEN_RPAREN body TOKEN_RPAREN
    | TOKEN_LPAREN TOKEN_BEGIN list_definition_star TOKEN_RPAREN
    ;

def_formals:
    list_variable_star
    | list_variable_plus TOKEN_DOT variable
    ;

list_definition_star:
    %empty
    | list_definition_star definition
    ;


expression: 
    variable 
    | literal 
    | procedure_call
    | lambda_expression
    | conditional
    | assignment
    | derived_expression
    ;
variable: TOKEN_IDENTIFIER;

literal: 
    quotation
    | self_evaluating
    ;

quotation: 
    TOKEN_SINGLE_QUOTE datum
    | TOKEN_LPAREN TOKEN_QUOTE datum TOKEN_RPAREN
    ;

self_evaluating:
    TOKEN_BOOLEAN
    | TOKEN_NUMBER
    | TOKEN_CHARACTER
    | TOKEN_STRING
    ;

procedure_call: 
    TOKEN_LPAREN operator list_operand_star TOKEN_RPAREN;

operator: expression;
list_operand_star:
    %empty
    | list_operand_star operand
    ;
operand:
    expression
    ;

lambda_expression:
    TOKEN_LPAREN TOKEN_LAMBDA formals body TOKEN_RPAREN
    ;

formals:
    TOKEN_LPAREN list_variable_star TOKEN_RPAREN
    | variable
    | TOKEN_LPAREN list_variable_plus TOKEN_DOT variable TOKEN_RPAREN
    ;

list_variable_star:
    %empty
    | list_variable_star variable
    ;

list_variable_plus:
    variable
    | list_variable_plus variable
    ;

body:
    list_definition_star sequence
    ;

sequence:
    list_command_star expression;
    

list_command_star:
    %empty
    | list_command_star command
    ;

conditional:
    TOKEN_LPAREN TOKEN_IF test consequent alternate TOKEN_RPAREN;

test: expression;
consequent: expression;
alternate: 
    expression
    | %empty
    ;
    
assignment: TOKEN_LPAREN TOKEN_SET variable expression TOKEN_RPAREN;

derived_expression:
    TOKEN_LPAREN TOKEN_COND list_cond_clause_plus TOKEN_RPAREN
    | TOKEN_LPAREN TOKEN_COND list_cond_clause_star TOKEN_LPAREN TOKEN_ELSE sequence TOKEN_RPAREN TOKEN_RPAREN
    | TOKEN_LPAREN TOKEN_CASE expression list_case_clause_plus TOKEN_RPAREN
    | TOKEN_LPAREN TOKEN_CASE expression list_case_clause_star TOKEN_LPAREN TOKEN_ELSE sequence TOKEN_RPAREN TOKEN_RPAREN
    | TOKEN_LPAREN TOKEN_AND list_test_star TOKEN_RPAREN
    | TOKEN_LPAREN TOKEN_OR list_test_star TOKEN_RPAREN
    | TOKEN_LPAREN TOKEN_LET TOKEN_LPAREN list_bind_spec_star TOKEN_RPAREN body TOKEN_RPAREN
    | TOKEN_LPAREN TOKEN_LET variable TOKEN_LPAREN list_bind_spec_star TOKEN_RPAREN body TOKEN_RPAREN
    | TOKEN_LPAREN TOKEN_LET_STAR TOKEN_LPAREN list_bind_spec_star TOKEN_RPAREN body TOKEN_RPAREN
    | TOKEN_LPAREN TOKEN_LET_REC TOKEN_LPAREN list_bind_spec_star TOKEN_RPAREN body TOKEN_RPAREN
    | TOKEN_LPAREN TOKEN_BEGIN sequence TOKEN_RPAREN
    | TOKEN_LPAREN TOKEN_DO TOKEN_LPAREN list_iteration_spec_star TOKEN_RPAREN TOKEN_LPAREN test sequence TOKEN_RPAREN list_command_star TOKEN_RPAREN
    | TOKEN_LPAREN TOKEN_DELAY expression TOKEN_RPAREN
    ;

cond_clause:
    TOKEN_LPAREN test sequence TOKEN_RPAREN
    | TOKEN_LPAREN test TOKEN_RPAREN
    | TOKEN_LPAREN test TOKEN_ARROW recipient TOKEN_RPAREN
    ;

list_cond_clause_plus:
    cond_clause
    | list_cond_clause_plus cond_clause
    ;

list_cond_clause_star:
    %empty
    | list_cond_clause_star cond_clause
    ;

recipient: expression;

case_clause:
    TOKEN_LPAREN TOKEN_LPAREN list_datum_star TOKEN_RPAREN sequence TOKEN_RPAREN
    ;

list_case_clause_plus:
    case_clause
    | list_case_clause_plus case_clause
    ;
list_case_clause_star:
    %empty
    | list_case_clause_star case_clause
    ;

list_test_star:
    %empty
    | list_test_star test
    ;

bindind_spec: TOKEN_LPAREN variable expression TOKEN_RPAREN;

list_bind_spec_star:
    %empty
    | list_bind_spec_star bindind_spec
    ;

iteration_spec: 
    TOKEN_LPAREN variable init step TOKEN_RPAREN
    | TOKEN_LPAREN variable init TOKEN_RPAREN
    ;

init: expression;

step: expression;
list_iteration_spec_star:
    %empty
    | list_iteration_spec_star iteration_spec
    ;

datum:
    simple_datum
    | compound_datum
    ;

simple_datum:
    TOKEN_BOOLEAN
    | TOKEN_NUMBER
    | TOKEN_CHARACTER
    | TOKEN_STRING
    | symbol
    ;

symbol: TOKEN_IDENTIFIER;

compound_datum:
    list
    | vector
    ;

list:
    TOKEN_LPAREN list_datum_star TOKEN_RPAREN
    | TOKEN_LPAREN list_datum_plus TOKEN_DOT datum TOKEN_RPAREN
    | abbreviation
    ;

abbreviation: abbrev_prefix datum;

abbrev_prefix:
    TOKEN_SINGLE_QUOTE
    | TOKEN_BACKQUOTE
    | TOKEN_COMMA
    | TOKEN_COMMA_AT
    ;

vector: TOKEN_VECTOR_OPEN list_datum_star TOKEN_RPAREN;

list_datum_star:
    %empty
    | list_datum_star datum
    ;

list_datum_plus:
    datum
    | list_datum_plus datum
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