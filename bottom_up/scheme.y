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
    %empty                                  { printf("[program] -> %%empty\n"); }
    | list_command_or_definition_plus       { printf("[program] -> list_command_or_definition_plus\n"); }
    ;

command_or_definition:
    command                                 { printf("[command_or_definition] -> command\n"); }
    | definition                            { printf("[command_or_definition] -> definition\n"); }
    ;

command: expression                         { printf("[command] -> expression\n"); }
    ;


list_command_or_definition_plus:
    command_or_definition                                       { printf("[list_command_or_definition_plus] -> command_or_definition\n"); }
    | list_command_or_definition_plus command_or_definition     { printf("[list_command_or_definition_plus] -> list_command_or_definition_plus command_or_definition\n"); }
    ;

definition:
    TOKEN_LPAREN TOKEN_DEFINE variable expression TOKEN_RPAREN                                   { printf("[definition] -> ( define variable expression )\n"); }
    | TOKEN_LPAREN TOKEN_DEFINE TOKEN_LPAREN variable def_formals TOKEN_RPAREN body TOKEN_RPAREN  { printf("[definition] -> ( define ( variable def_formals ) body )\n"); }
    | TOKEN_LPAREN TOKEN_BEGIN list_definition_star TOKEN_RPAREN                                  { printf("[definition] -> ( begin list_definition_star )\n"); }
    ;

def_formals:
    %empty
    | list_variable_plus                              { printf("[def_formals] -> list_variable_star\n"); }
    | list_variable_plus TOKEN_DOT variable         { printf("[def_formals] -> list_variable_plus . variable\n"); }
    ;

list_definition_star:
    %empty                              { printf("[list_definition_star] -> %%empty\n"); }
    | list_definition_star definition   { printf("[list_definition_star] -> list_definition_star definition\n"); }
    ;


expression:
    variable                { printf("[expression] -> variable\n"); }
    | literal               { printf("[expression] -> literal\n"); }
    | procedure_call        { printf("[expression] -> procedure_call\n"); }
    | lambda_expression     { printf("[expression] -> lambda_expression\n"); }
    | conditional           { printf("[expression] -> conditional\n"); }
    | assignment            { printf("[expression] -> assignment\n"); }
    | derived_expression    { printf("[expression] -> derived_expression\n"); }
    ;
variable: TOKEN_IDENTIFIER  { printf("[variable] -> IDENTIFIER (yylval = \"%s\")\n", $1); }
    ;

literal:
    quotation               { printf("[literal] -> quotation\n"); }
    | self_evaluating       { printf("[literal] -> self_evaluating\n"); }
    ;

quotation:
    TOKEN_SINGLE_QUOTE datum                            { printf("[quotation] -> ' datum\n"); }
    | TOKEN_LPAREN TOKEN_QUOTE datum TOKEN_RPAREN        { printf("[quotation] -> ( quote datum )\n"); }
    ;

self_evaluating:
    TOKEN_BOOLEAN           { printf("[self_evaluating] -> BOOLEAN (yylval = \"%s\")\n", $1); }
    | TOKEN_NUMBER          { printf("[self_evaluating] -> NUMBER (yylval = \"%s\")\n", $1); }
    | TOKEN_CHARACTER       { printf("[self_evaluating] -> CHARACTER (yylval = \"%s\")\n", $1); }
    | TOKEN_STRING          { printf("[self_evaluating] -> STRING (yylval = \"%s\")\n", $1); }
    ;

procedure_call:
    TOKEN_LPAREN operator list_operand_star TOKEN_RPAREN    { printf("[procedure_call] -> ( operator list_operand_star )\n"); }
    ;

operator: expression        { printf("[operator] -> expression\n"); }
    ;
list_operand_star:
    %empty                          { printf("[list_operand_star] -> %%empty\n"); }
    | list_operand_star operand     { printf("[list_operand_star] -> list_operand_star operand\n"); }
    ;
operand:
    expression              { printf("[operand] -> expression\n"); }
    ;

lambda_expression:
    TOKEN_LPAREN TOKEN_LAMBDA formals body TOKEN_RPAREN     { printf("[lambda_expression] -> ( lambda formals body )\n"); }
    ;

formals:
    variable
    | TOKEN_LPAREN TOKEN_RPAREN
    | TOKEN_LPAREN list_variable_plus TOKEN_RPAREN                            { printf("[formals] -> ( list_variable_star )\n"); }                                                             { printf("[formals] -> variable\n"); }
    | TOKEN_LPAREN list_variable_plus TOKEN_DOT variable TOKEN_RPAREN       { printf("[formals] -> ( list_variable_plus . variable )\n"); }
    ;

list_variable_star:
    %empty        
    | list_variable_star variable       { printf("[list_variable_star] -> list_variable_star variable\n"); }
    ;

list_variable_plus:
    variable list_variable_star                            { printf("[list_variable_plus] -> variable\n"); }
    ;

body:
    list_command_or_definition_plus       { printf("[body] -> list_definition_star sequence\n"); }
    ;

sequence:
    list_command_star expression        { printf("[sequence] -> list_command_star expression\n"); }
    ;


list_command_star:
    %empty                          { printf("[list_command_star] -> %%empty\n"); }
    | list_command_star command     { printf("[list_command_star] -> list_command_star command\n"); }
    ;

conditional:
    TOKEN_LPAREN TOKEN_IF test consequent alternate TOKEN_RPAREN     { printf("[conditional] -> ( if test consequent alternate )\n"); }
    ;

test: expression            { printf("[test] -> expression\n"); }
    ;
consequent: expression      { printf("[consequent] -> expression\n"); }
    ;
alternate:
    expression              { printf("[alternate] -> expression\n"); }
    | %empty                { printf("[alternate] -> %%empty\n"); }
    ;

assignment: TOKEN_LPAREN TOKEN_SET variable expression TOKEN_RPAREN  { printf("[assignment] -> ( set! variable expression )\n"); }
    ;

derived_expression:
    TOKEN_LPAREN TOKEN_COND list_cond_clause_plus TOKEN_RPAREN                                                       { printf("[derived_expression] -> ( cond list_cond_clause_plus )\n"); }
    | TOKEN_LPAREN TOKEN_COND list_cond_clause_star TOKEN_LPAREN TOKEN_ELSE sequence TOKEN_RPAREN TOKEN_RPAREN       { printf("[derived_expression] -> ( cond list_cond_clause_star ( else sequence ) )\n"); }
    | TOKEN_LPAREN TOKEN_CASE expression list_case_clause_plus TOKEN_RPAREN                                          { printf("[derived_expression] -> ( case expression list_case_clause_plus )\n"); }
    | TOKEN_LPAREN TOKEN_CASE expression list_case_clause_star TOKEN_LPAREN TOKEN_ELSE sequence TOKEN_RPAREN TOKEN_RPAREN  { printf("[derived_expression] -> ( case expression list_case_clause_star ( else sequence ) )\n"); }
    | TOKEN_LPAREN TOKEN_AND list_test_star TOKEN_RPAREN                                                             { printf("[derived_expression] -> ( and list_test_star )\n"); }
    | TOKEN_LPAREN TOKEN_OR list_test_star TOKEN_RPAREN                                                              { printf("[derived_expression] -> ( or list_test_star )\n"); }
    | TOKEN_LPAREN TOKEN_LET TOKEN_LPAREN list_bind_spec_star TOKEN_RPAREN body TOKEN_RPAREN                         { printf("[derived_expression] -> ( let ( list_bind_spec_star ) body )\n"); }
    | TOKEN_LPAREN TOKEN_LET variable TOKEN_LPAREN list_bind_spec_star TOKEN_RPAREN body TOKEN_RPAREN                { printf("[derived_expression] -> ( let variable ( list_bind_spec_star ) body )\n"); }
    | TOKEN_LPAREN TOKEN_LET_STAR TOKEN_LPAREN list_bind_spec_star TOKEN_RPAREN body TOKEN_RPAREN                    { printf("[derived_expression] -> ( let* ( list_bind_spec_star ) body )\n"); }
    | TOKEN_LPAREN TOKEN_LET_REC TOKEN_LPAREN list_bind_spec_star TOKEN_RPAREN body TOKEN_RPAREN                     { printf("[derived_expression] -> ( letrec ( list_bind_spec_star ) body )\n"); }
    | TOKEN_LPAREN TOKEN_BEGIN sequence TOKEN_RPAREN                                                                 { printf("[derived_expression] -> ( begin sequence )\n"); }
    | TOKEN_LPAREN TOKEN_DO TOKEN_LPAREN list_iteration_spec_star TOKEN_RPAREN TOKEN_LPAREN test sequence TOKEN_RPAREN list_command_star TOKEN_RPAREN  { printf("[derived_expression] -> ( do ( list_iteration_spec_star ) ( test sequence ) list_command_star )\n"); }
    | TOKEN_LPAREN TOKEN_DELAY expression TOKEN_RPAREN                                                               { printf("[derived_expression] -> ( delay expression )\n"); }
    ;

cond_clause:
    TOKEN_LPAREN test sequence TOKEN_RPAREN                  { printf("[cond_clause] -> ( test sequence )\n"); }
    | TOKEN_LPAREN test TOKEN_RPAREN                         { printf("[cond_clause] -> ( test )\n"); }
    | TOKEN_LPAREN test TOKEN_ARROW recipient TOKEN_RPAREN   { printf("[cond_clause] -> ( test => recipient )\n"); }
    ;

list_cond_clause_plus:
    cond_clause                             { printf("[list_cond_clause_plus] -> cond_clause\n"); }
    | list_cond_clause_plus cond_clause     { printf("[list_cond_clause_plus] -> list_cond_clause_plus cond_clause\n"); }
    ;

list_cond_clause_star:
    %empty                                  { printf("[list_cond_clause_star] -> %%empty\n"); }
    | list_cond_clause_star cond_clause     { printf("[list_cond_clause_star] -> list_cond_clause_star cond_clause\n"); }
    ;

recipient: expression       { printf("[recipient] -> expression\n"); }
    ;

case_clause:
    TOKEN_LPAREN TOKEN_LPAREN list_datum_star TOKEN_RPAREN sequence TOKEN_RPAREN     { printf("[case_clause] -> ( ( list_datum_star ) sequence )\n"); }
    ;

list_case_clause_plus:
    case_clause                             { printf("[list_case_clause_plus] -> case_clause\n"); }
    | list_case_clause_plus case_clause     { printf("[list_case_clause_plus] -> list_case_clause_plus case_clause\n"); }
    ;
list_case_clause_star:
    %empty                                  { printf("[list_case_clause_star] -> %%empty\n"); }
    | list_case_clause_star case_clause     { printf("[list_case_clause_star] -> list_case_clause_star case_clause\n"); }
    ;

list_test_star:
    %empty                      { printf("[list_test_star] -> %%empty\n"); }
    | list_test_star test       { printf("[list_test_star] -> list_test_star test\n"); }
    ;

bindind_spec: TOKEN_LPAREN variable expression TOKEN_RPAREN  { printf("[bindind_spec] -> ( variable expression )\n"); }
    ;

list_bind_spec_star:
    %empty                                  { printf("[list_bind_spec_star] -> %%empty\n"); }
    | list_bind_spec_star bindind_spec      { printf("[list_bind_spec_star] -> list_bind_spec_star bindind_spec\n"); }
    ;

iteration_spec:
    TOKEN_LPAREN variable init step TOKEN_RPAREN    { printf("[iteration_spec] -> ( variable init step )\n"); }
    | TOKEN_LPAREN variable init TOKEN_RPAREN       { printf("[iteration_spec] -> ( variable init )\n"); }
    ;

init: expression        { printf("[init] -> expression\n"); }
    ;

step: expression        { printf("[step] -> expression\n"); }
    ;
list_iteration_spec_star:
    %empty                                          { printf("[list_iteration_spec_star] -> %%empty\n"); }
    | list_iteration_spec_star iteration_spec       { printf("[list_iteration_spec_star] -> list_iteration_spec_star iteration_spec\n"); }
    ;

datum:
    simple_datum            { printf("[datum] -> simple_datum\n"); }
    | compound_datum        { printf("[datum] -> compound_datum\n"); }
    ;

simple_datum:
    TOKEN_BOOLEAN           { printf("[simple_datum] -> BOOLEAN (yylval = \"%s\")\n", $1); }
    | TOKEN_NUMBER          { printf("[simple_datum] -> NUMBER (yylval = \"%s\")\n", $1); }
    | TOKEN_CHARACTER       { printf("[simple_datum] -> CHARACTER (yylval = \"%s\")\n", $1); }
    | TOKEN_STRING          { printf("[simple_datum] -> STRING (yylval = \"%s\")\n", $1); }
    | symbol                { printf("[simple_datum] -> symbol\n"); }
    ;

symbol: TOKEN_IDENTIFIER    { printf("[symbol] -> IDENTIFIER (yylval = \"%s\")\n", $1); }
    ;

compound_datum:
    list                    { printf("[compound_datum] -> list\n"); }
    | vector                { printf("[compound_datum] -> vector\n"); }
    ;

list:
    TOKEN_LPAREN list_datum_star TOKEN_RPAREN                        { printf("[list] -> ( list_datum_star )\n"); }
    | TOKEN_LPAREN list_datum_plus TOKEN_DOT datum TOKEN_RPAREN      { printf("[list] -> ( list_datum_plus . datum )\n"); }
    | abbreviation                                                  { printf("[list] -> abbreviation\n"); }
    ;

abbreviation: abbrev_prefix datum   { printf("[abbreviation] -> abbrev_prefix datum\n"); }
    ;

abbrev_prefix:
    TOKEN_SINGLE_QUOTE      { printf("[abbrev_prefix] -> '\n"); }
    | TOKEN_BACKQUOTE       { printf("[abbrev_prefix] -> `\n"); }
    | TOKEN_COMMA           { printf("[abbrev_prefix] -> ,\n"); }
    | TOKEN_COMMA_AT        { printf("[abbrev_prefix] -> ,@\n"); }
    ;

vector: TOKEN_VECTOR_OPEN list_datum_star TOKEN_RPAREN   { printf("[vector] -> #( list_datum_star )\n"); }
    ;

list_datum_star:
    %empty                          { printf("[list_datum_star] -> %%empty\n"); }
    | list_datum_star datum         { printf("[list_datum_star] -> list_datum_star datum\n"); }
    ;

list_datum_plus:
    datum                           { printf("[list_datum_plus] -> datum\n"); }
    | list_datum_plus datum         { printf("[list_datum_plus] -> list_datum_plus datum\n"); }
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
    printf("\n");
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