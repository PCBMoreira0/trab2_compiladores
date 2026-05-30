%{
#include <stdio.h>    
#include <stdlib.h>    
%}

%option noyywrap

LETTER [a-z]
DIGIT  [0-9]

SPECIAL_INITIAL [!\$\%&*/:<=>?~_^]

INITIAL {LETTER}|{SPECIAL_INITIAL}

SPECIAL_SUBSEQUENT [.+-]
SUBSEQUENT {INITIAL}|{DIGIT}|{SPECIAL_SUBSEQUENT}
PECULIAR_IDENTIFIER "+"|"-"|"..."

IDENTIFIER {INITIAL}{SUBSEQUENT}*|{PECULIAR_IDENTIFIER}

WHITESPACE [ \n]

DELIMITER {WHITESPACE}|[()";]
COMMENT ;.+\n

ATMOSPHERE {WHITESPACE}|{COMMENT}
INTERTOKEN_SPACE ATMOSPHERE*

EXPRESSION_KEYWORD (quote|lambda|if|set!|begin|cond|and|or|case|let|let\*|letrec|do|delay|quasiquote)
SYNTATIC_KEYWORD ({EXPRESSION_KEYWORD}|else|=>|define|unquote|unquote-splicing)


BOOLEAN (#t|#f)
CHARACTER_NAME (space|newline)
CHARACTER (#\\{CHARACTER_NAME}|#\\.)

STRING_ELEMENT    ([^"\\]|\\\"|\\\\)
STRING \"{STRING_ELEMENT}*\"

RADIX_2 (#b)
RADIX_8 (#o)
RADIX_10 (\#d)?
RADIX_16 (#x)

DIGIT_2 [01]
DIGIT_8 [01234567]
DIGIT_10 {DIGIT}
DIGIT_16 {DIGIT_10}|[abcdef]

EXACTNESS (#i|#e)?

PREFIX_2 ({RADIX_2}{EXACTNESS})|{EXACTNESS}{RADIX_2}
PREFIX_8 ({RADIX_8}{EXACTNESS})|{EXACTNESS}{RADIX_8}
PREFIX_10 ({RADIX_10}{EXACTNESS})|{EXACTNESS}{RADIX_10}
PREFIX_16 ({RADIX_16}{EXACTNESS})|{EXACTNESS}{RADIX_16}

SIGN [+-]?
UINTEGER_2 {DIGIT_2}+(#)*
UINTEGER_8 {DIGIT_8}+(#)*
UINTEGER_10 {DIGIT_10}+(#)*
UINTEGER_16 {DIGIT_16}+(#)*

EXPOENT_MARKER [esfdl]
SUFFIX ({EXPOENT_MARKER}{SIGN}{DIGIT_10}+)?

DECIMAL_10 ({UINTEGER_10}{SUFFIX})|("."{DIGIT_10}+(#)*{SUFFIX})({DIGIT_10}+"."{DIGIT_10}*(#)*{SUFFIX})
|({DIGIT_10}+(#)+"."(#)*{SUFFIX})
UREAL_2 ({UINTEGER_2})|({UINTEGER_2}"/"{UINTEGER_2})
UREAL_8 ({UINTEGER_8})|({UINTEGER_8}"/"{UINTEGER_8})
UREAL_10 ({UINTEGER_10})|({UINTEGER_10}"/"{UINTEGER_10})|({DECIMAL_10})
UREAL_16 ({UINTEGER_16})|({UINTEGER_16}"/"{UINTEGER_16})
REAL_2 {SIGN}{UREAL_2}
REAL_8 {SIGN}{UREAL_8}
REAL_10 {SIGN}{UREAL_10}
REAL_16 {SIGN}{UREAL_16}
COMPLEX_2 ({REAL_2})|({REAL_2}"@"{REAL_2})|({REAL_2}"+"{UREAL_2}i)({REAL_2}"-"{UREAL_2}i)|({REAL_2}"+"i)|({REAL_2}"-"i)|("+"{UREAL_2}i)|("-"{UREAL_2}i)|("+"i)|("-"i)
COMPLEX_8 ({REAL_8})|({REAL_8}"@"{REAL_8})|({REAL_8}"+"{UREAL_8}i)({REAL_8}"-"{UREAL_8}i)|({REAL_8}"+"i)|({REAL_8}"-"i)|("+"{UREAL_8}i)|("-"{UREAL_8}i)|("+"i)|("-"i)
COMPLEX_10 ({REAL_10})|({REAL_10}"@"{REAL_10})|({REAL_10}"+{UREAL_10}i)|({REAL_10}"-"{UREAL_10}i)|({RE
AL_10}"+"i)|({REAL_10}"-"i)|("+"{UREAL_10}i)|("-"{UREAL_10}i)|("+"i)("-"i)
COMPLEX_16 ({REAL_16})|({REAL_16}"@"{REAL_16})|({REAL_16}"+{UREAL_16}i)|({REAL_16}"-"{UREAL_16}i)|({RE
AL_16}"+"i)|({REAL_16}"-"i)|("+"{UREAL_16}i)|("-"{UREAL_16}i)|("+"i)("-"i)
NUM_2 {PREFIX_2}{COMPLEX_2}
NUM_8 {PREFIX_8}{COMPLEX_8}
NUM_10 {PREFIX_10}{COMPLEX_10}
NUM_16 {PREFIX_16}{COMPLEX_16}

NUMBER {NUM_2}|{NUM_8}|{NUM_10}|{NUM_16}



%%
{STRING} { printf("STRING: %s\n", yytext); }
{CHARACTER} { printf("CHARACTER: %s\n", yytext); }
{COMMENT} { printf("COMMENT: %s\n", yytext); }
{DELIMITER} { printf("DELIMITER: %s\n", yytext); }
{SYNTATIC_KEYWORD} { printf("SYNTATIC_KEYWORD: %s\n", yytext); }
{IDENTIFIER} { printf("IDENTIFICADOR: %s\n", yytext); }
. printf("token não reconhecido: %s\n", yytext);

%%


int main(int argc, char **argv) {
    printf("Digite seu codigo (Ctrl+D para encerrar):\n");
    
    yylex(); 
    
    return 0;
} 