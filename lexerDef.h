#ifndef LEXERDEF_H
#define LEXERDEF_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define MAX_LEX_LENGTH 100
#define BUFFER_SIZE 100
#define SYMBOL_TABLE_SIZE 100

typedef enum {
    TK_ASSIGNOP, TK_COMMENT, TK_FIELDID, TK_ID, TK_NUM, TK_RNUM, TK_FUNID, TK_RUID, TK_WITH, TK_END, TK_WHILE, TK_UNION, TK_ENDUNION, TK_DEFINETYPE, TK_AS, TK_TYPE, TK_MAIN, TK_GLOBAL, TK_PARAMETER, TK_LIST, TK_SQL, TK_SQR, TK_INPUT, TK_OUTPUT, TK_INT, TK_REAL, TK_COMMA, TK_SEM, TK_COLON, TK_DOT, TK_ENDWHILE, TK_OP, TK_CL,TK_IF,TK_THEN, TK_ENDIF, TK_READ, TK_WRITE, TK_RETURN, TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, TK_CALL,TK_RECORD, TK_ENDRECORD, TK_ELSE, TK_AND, TK_OR, TK_NOT, TK_LT, TK_LE,  TK_EQ, TK_GT, TK_GE, TK_NE  
} vocab;



typedef enum{
    LEX_UNDEFINED
} lexErrors;


typedef struct{
    vocab tokenName;
    char lexeme[MAX_LEX_LENGTH];
    int lineNum;
} 
token;


typedef struct node{
    char* key;
    token* data;
    struct node* next;
} node;

typedef struct{
    node* table[SYMBOL_TABLE_SIZE];
} symbolTable;

typedef struct{
    int fwdPtr;
    int currPtr;
    int eof;
    int currBuffer;
    char buf1[BUFFER_SIZE];
    char buf2[BUFFER_SIZE];
} buffer;

extern int currLineNum;
extern int dfaState;
extern symbolTable* symbol_table;
extern buffer* twin_buffer;


#endif