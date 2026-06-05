%{
#include <stdio.h>
#include <stdlib.h>
#include "libs/symbol_tab.h"
#include "libs/scheme_ast.h"
#include "libs/translator.h"
#include "libs/merge_file.h"

int yylex(void);
void yyerror(const char *);
extern FILE *yyin;

/* raiz da AST construida durante o parsing */
ASTNode *ast_root = NULL;
%}

%code requires {
    #include "libs/scheme_ast.h"
    #include "libs/symbol_tab.h"
}

%union {
    char *string_val;
    ASTNode *node;
    /* formals carregam a lista de parametros + o parametro variadico (resto) */
    struct { ASTNode *params; ASTNode *rest; } formals_val;
    /* corpos de cond/case carregam a lista de clausulas + a clausula else */
    struct { ASTNode *clauses; ASTNode *else_clause; } clause_body;
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

/* nao-terminais que produzem um no' da AST */
%type <node> program command_or_definition command
%type <node> list_command_or_definition_plus list_command_or_definition_star
%type <node> definition expression variable literal quotation self_evaluating
%type <node> procedure_call operator list_operand_star operand
%type <node> lambda_expression list_variable_star list_variable_plus
%type <node> body sequence list_command_star
%type <node> conditional test consequent alternate assignment derived_expression
%type <node> cond_clause list_cond_clause_plus cond_else_clause recipient
%type <node> case_clause list_case_clause_plus case_else_clause
%type <node> list_test_star bindind_spec list_bind_spec_star
%type <node> iteration_spec init step list_iteration_spec_star
%type <node> datum simple_datum symbol compound_datum list abbreviation
%type <node> vector list_datum_star list_datum_plus

/* nao-terminais com valor semantico composto */
%type <formals_val> formals def_formals
%type <clause_body> cond_body case_body

%%
program:
    %empty                              { ast_root = NULL; }
    | list_command_or_definition_plus   { ast_root = $1; }
    ;

command_or_definition:
    command         { $$ = $1; }
    | definition    { $$ = $1; }
    ;

command: expression { $$ = $1; }
    ;


list_command_or_definition_plus:
    command_or_definition                                   { $$ = ast_create_list($1, NULL); }
    | list_command_or_definition_plus command_or_definition { $$ = ast_list_append($1, $2); }
    ;

list_command_or_definition_star:
      %empty                            { $$ = NULL; }
    | list_command_or_definition_plus   { $$ = $1; }
    ;

definition:
    TOKEN_LPAREN TOKEN_DEFINE variable expression TOKEN_RPAREN
        { $$ = ast_create_define_var($3, $4); }
    | TOKEN_LPAREN TOKEN_DEFINE TOKEN_LPAREN variable def_formals TOKEN_RPAREN body TOKEN_RPAREN
        { $$ = ast_create_define_func($4, $5.params, $5.rest, $7); }
    ;

def_formals:
    list_variable_star                          { $$.params = $1;   $$.rest = NULL; }
    | list_variable_plus TOKEN_DOT variable     { $$.params = $1;   $$.rest = $3; }
    ;

expression:
    variable            { $$ = $1; }
    | literal           { $$ = $1; }
    | procedure_call    { $$ = $1; }
    | lambda_expression { $$ = $1; }
    | conditional       { $$ = $1; }
    | assignment        { $$ = $1; }
    | derived_expression{ $$ = $1; }
    ;

variable: TOKEN_IDENTIFIER { $$ = ast_create_variable($1); }
    ;

literal:
    quotation           { $$ = $1; }
    | self_evaluating   { $$ = $1; }
    ;

quotation:
    TOKEN_SINGLE_QUOTE datum                        { $$ = ast_create_quote($2); }
    | TOKEN_LPAREN TOKEN_QUOTE datum TOKEN_RPAREN   { $$ = ast_create_quote($3); }
    ;

self_evaluating:
    TOKEN_BOOLEAN       { $$ = ast_create_boolean($1); }
    | TOKEN_NUMBER      { $$ = ast_create_number($1); }
    | TOKEN_CHARACTER   { $$ = ast_create_character($1); }
    | TOKEN_STRING      { $$ = ast_create_string($1); }
    ;

procedure_call:
    TOKEN_LPAREN operator list_operand_star TOKEN_RPAREN { $$ = ast_create_call($2, $3); }
    ;

operator: expression { $$ = $1; }
    ;

list_operand_star:
    %empty                          { $$ = NULL; }
    | list_operand_star operand     { $$ = ast_list_append($1, $2); }
    ;
operand:
    expression { $$ = $1; }
    ;

lambda_expression:
    TOKEN_LPAREN TOKEN_LAMBDA formals body TOKEN_RPAREN
        { $$ = ast_create_lambda($3.params, $3.rest, $4); }
    ;

formals:
    TOKEN_LPAREN list_variable_star TOKEN_RPAREN
        { $$.params = $2;   $$.rest = NULL; }
    | variable
        { $$.params = NULL; $$.rest = $1; }
    | TOKEN_LPAREN list_variable_plus TOKEN_DOT variable TOKEN_RPAREN
        { $$.params = $2;   $$.rest = $4; }
    ;

list_variable_star:
    %empty                  { $$ = NULL; }
    | list_variable_plus    { $$ = $1; }
    ;

list_variable_plus:
    variable                        { $$ = ast_create_list($1, NULL); }
    | list_variable_plus variable   { $$ = ast_list_append($1, $2); }
    ;

body:
    list_command_or_definition_plus { $$ = $1; }
    ;

sequence:
    list_command_star expression { $$ = ast_list_append($1, $2); }
    ;


list_command_star:
    %empty                      { $$ = NULL; }
    | list_command_star command { $$ = ast_list_append($1, $2); }
    ;

conditional:
    TOKEN_LPAREN TOKEN_IF test consequent alternate TOKEN_RPAREN
        { $$ = ast_create_if($3, $4, $5); }
    ;

test: expression        { $$ = $1; }
    ;
consequent: expression  { $$ = $1; }
    ;
alternate:
    expression  { $$ = $1; }
    | %empty    { $$ = NULL; }
    ;

assignment:
    TOKEN_LPAREN TOKEN_SET variable expression TOKEN_RPAREN
        { $$ = ast_create_set($3, $4); }
    ;

derived_expression:
    TOKEN_LPAREN TOKEN_COND cond_body TOKEN_RPAREN
        { $$ = ast_create_cond($3.clauses, $3.else_clause); }
    | TOKEN_LPAREN TOKEN_CASE expression case_body TOKEN_RPAREN
        { $$ = ast_create_case($3, $4.clauses, $4.else_clause); }
    | TOKEN_LPAREN TOKEN_AND list_test_star TOKEN_RPAREN
        { $$ = ast_create_and($3); }
    | TOKEN_LPAREN TOKEN_OR list_test_star TOKEN_RPAREN
        { $$ = ast_create_or($3); }
    | TOKEN_LPAREN TOKEN_LET TOKEN_LPAREN list_bind_spec_star TOKEN_RPAREN body TOKEN_RPAREN
        { $$ = ast_create_let($4, $6); }
    | TOKEN_LPAREN TOKEN_LET variable TOKEN_LPAREN list_bind_spec_star TOKEN_RPAREN body TOKEN_RPAREN
        { $$ = ast_create_named_let($3, $5, $7); }
    | TOKEN_LPAREN TOKEN_LET_STAR TOKEN_LPAREN list_bind_spec_star TOKEN_RPAREN body TOKEN_RPAREN
        { $$ = ast_create_let_star($4, $6); }
    | TOKEN_LPAREN TOKEN_LET_REC TOKEN_LPAREN list_bind_spec_star TOKEN_RPAREN body TOKEN_RPAREN
        { $$ = ast_create_letrec($4, $6); }
    | TOKEN_LPAREN TOKEN_BEGIN list_command_or_definition_star TOKEN_RPAREN
        { $$ = ast_create_begin($3); }
    | TOKEN_LPAREN TOKEN_DO TOKEN_LPAREN list_iteration_spec_star TOKEN_RPAREN TOKEN_LPAREN test sequence TOKEN_RPAREN list_command_star TOKEN_RPAREN
        { $$ = ast_create_do($4, $7, $8, $10); }
    | TOKEN_LPAREN TOKEN_DELAY expression TOKEN_RPAREN
        { $$ = ast_create_delay($3); }
    ;

cond_clause:
    TOKEN_LPAREN test sequence TOKEN_RPAREN
        { $$ = ast_create_cond_clause($2, $3, NULL); }
    | TOKEN_LPAREN test TOKEN_RPAREN
        { $$ = ast_create_cond_clause($2, NULL, NULL); }
    | TOKEN_LPAREN test TOKEN_ARROW recipient TOKEN_RPAREN
        { $$ = ast_create_cond_clause($2, NULL, $4); }
    ;

list_cond_clause_plus:
    cond_clause                             { $$ = ast_create_list($1, NULL); }
    | list_cond_clause_plus cond_clause     { $$ = ast_list_append($1, $2); }
    ;

cond_body:
    list_cond_clause_plus                       { $$.clauses = $1;   $$.else_clause = NULL; }
    | list_cond_clause_plus cond_else_clause    { $$.clauses = $1;   $$.else_clause = $2; }
    | cond_else_clause                          { $$.clauses = NULL; $$.else_clause = $1; }
    ;

cond_else_clause:
      TOKEN_LPAREN TOKEN_ELSE sequence TOKEN_RPAREN { $$ = $3; }
    ;

recipient: expression { $$ = $1; }
    ;

case_clause:
    TOKEN_LPAREN TOKEN_LPAREN list_datum_star TOKEN_RPAREN sequence TOKEN_RPAREN
        { $$ = ast_create_case_clause($3, $5); }
    ;

list_case_clause_plus:
    case_clause                             { $$ = ast_create_list($1, NULL); }
    | list_case_clause_plus case_clause     { $$ = ast_list_append($1, $2); }
    ;

case_body:
      list_case_clause_plus                     { $$.clauses = $1;   $$.else_clause = NULL; }
    | list_case_clause_plus case_else_clause    { $$.clauses = $1;   $$.else_clause = $2; }
    | case_else_clause                          { $$.clauses = NULL; $$.else_clause = $1; }
    ;

case_else_clause:
      TOKEN_LPAREN TOKEN_ELSE sequence TOKEN_RPAREN { $$ = $3; }
    ;

list_test_star:
    %empty                  { $$ = NULL; }
    | list_test_star test   { $$ = ast_list_append($1, $2); }
    ;

bindind_spec:
    TOKEN_LPAREN variable expression TOKEN_RPAREN
        { $$ = ast_create_binding($2, $3); }
    ;

list_bind_spec_star:
    %empty                              { $$ = NULL; }
    | list_bind_spec_star bindind_spec  { $$ = ast_list_append($1, $2); }
    ;

iteration_spec:
    TOKEN_LPAREN variable init step TOKEN_RPAREN
        { $$ = ast_create_iter_spec($2, $3, $4); }
    | TOKEN_LPAREN variable init TOKEN_RPAREN
        { $$ = ast_create_iter_spec($2, $3, NULL); }
    ;

init: expression { $$ = $1; }
    ;

step: expression { $$ = $1; }
    ;
list_iteration_spec_star:
    %empty                                      { $$ = NULL; }
    | list_iteration_spec_star iteration_spec   { $$ = ast_list_append($1, $2); }
    ;

datum:
    simple_datum        { $$ = $1; }
    | compound_datum    { $$ = $1; }
    ;

simple_datum:
    TOKEN_BOOLEAN       { $$ = ast_create_boolean($1); }
    | TOKEN_NUMBER      { $$ = ast_create_number($1); }
    | TOKEN_CHARACTER   { $$ = ast_create_character($1); }
    | TOKEN_STRING      { $$ = ast_create_string($1); }
    | symbol            { $$ = $1; }
    ;

symbol: TOKEN_IDENTIFIER { $$ = ast_create_variable($1); }
    ;

compound_datum:
    list        { $$ = $1; }
    | vector    { $$ = $1; }
    ;

list:
    TOKEN_LPAREN list_datum_star TOKEN_RPAREN
        { $$ = $2; }
    | TOKEN_LPAREN list_datum_plus TOKEN_DOT datum TOKEN_RPAREN
        { $$ = ast_list_append($2, $4); }
    | abbreviation
        { $$ = $1; }
    ;

abbreviation: abbrev_prefix datum { $$ = ast_create_quote($2); }
    ;

abbrev_prefix:
    TOKEN_SINGLE_QUOTE
    | TOKEN_BACKQUOTE
    | TOKEN_COMMA
    | TOKEN_COMMA_AT
    ;

vector: TOKEN_VECTOR_OPEN list_datum_star TOKEN_RPAREN { $$ = ast_create_vector($2); }
    ;

list_datum_star:
    %empty              { $$ = NULL; }
    | list_datum_plus   { $$ = $1; }
    ;

list_datum_plus:
    datum                       { $$ = ast_create_list($1, NULL); }
    | list_datum_plus datum     { $$ = ast_list_append($1, $2); }
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

    printf("\n===== AST =====\n");
    printf("===============\n");
    ast_print(ast_root, 0);

    printf("\n===== TABELA DE SIMBOLOS =====\n"); 
    SymbolTable *table = symtab_create();
    ast_dfs(ast_root, table);

    printf("\n===== VERIFICAÇÃO =====\n");
    printf("===============\n");
    ast_dfs_second(ast_root, table); 


    FILE *py = fopen("py_code.py", "w+");
    if (!py) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    FILE *py_defs = fopen("py_defs.py", "w+");
    if (!py) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }
    translate(ast_root, py, py_defs, 0);

    fclose(py);
    fclose(py_defs);
    if (merge_files("py_defs.py", "py_code.py", "python.py")) {
        printf("Arquivos mesclados com sucesso em 'python.py'!\n");
    } else {
        printf("Falha na mesclagem.\n");
    }

    /* remove("py_defs.py");
    remove("py_code.py"); */

    if (yyin != stdin) {
        fclose(yyin);
    }

    printf("\nfim\n");

    return 0;
}
