#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

buffer* makeBuffer();
void freeBuffer(buffer* twinBuffer);
void resetBuffer(buffer* twinBuffer);

FILE *getStream(FILE *fp, buffer* twinBuffer);
token* getNextToken(buffer* twinBuffer); // getTokenInfo==token* (basically pointer to token)

symbolTable* makeSymbolTable();
void freeSymbolTable(symbolTable* symt);
void insertIntoSymbolTable(symbolTable* symt, token* key);
char* lookupSymbolTable(symbolTable* symt,token* key);

void retract(buffer* twinBuffer);
void increaseBuffer(buffer* twinBuffer);
void doubleRetract(buffer* twinBuffer);

token* createToken(vocab v, char* lexeme, int linenumber);

char* retractReturnLex(char* dst, buffer* twinBuffer, int max_len);
void removeComments(char *testcaseFile, char *cleanFile);


#endif



