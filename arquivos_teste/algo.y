
%{
    #include <stdio.h>
    int yylex(void);
    int yyparse(void);
    void yyerror(const char *);
%}

%token ID
%token INT
%token IF
%token OI

%%

inicio: OI IF '\n' { printf("SIM"); }
    ;

%%

int main(int argc, char **argv) {

	yyparse();
}

void yyerror(const char * s)
{
	/* variáveis definidas no analisador léxico */
	extern int yylineno;    
	extern char * yytext;   

    printf("%d %s\n", yylineno, yytext);
}
 
