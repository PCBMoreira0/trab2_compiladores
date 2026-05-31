#ifndef SCHEME_TAB_H
#define SCHEME_TAB_H

/* Fingindo ser o Bison gerando códigos de token */
#define TOKEN_LPAREN 258
#define TOKEN_RPAREN 259
#define TOKEN_DEFINE 260
#define TOKEN_IF 261
#define TOKEN_LAMBDA 262
#define TOKEN_SET 263
#define TOKEN_BEGIN 264
#define TOKEN_COND 265
#define TOKEN_LET 266
#define TOKEN_QUOTE 267
#define TOKEN_AND 268
#define TOKEN_OR 269
#define TOKEN_BOOLEAN 270
#define TOKEN_NUMBER 271
#define TOKEN_STRING 272
#define TOKEN_CHARACTER 273
#define TOKEN_IDENTIFIER 274

/* Definindo a union do yylval */
typedef union {
    char* string_val;
} YYSTYPE;

extern YYSTYPE yylval;

#endif