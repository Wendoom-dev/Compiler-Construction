#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

buffer* makeBuffer();
void freeBuffer(buffer* twinBuffer);
void resetBuffer(buffer* twinBuffer);

FILE *getStream(FILE *fp);
token* getNextToken(buffer* twinBuffer); // getTokenInfo==token* (basically pointer to token)

SymbolTable* makeSymbolTable();
void freeSymbolTable(SymbolTable* symt);
void insertIntoSymbolTable(SymbolTable* symt, token* key);
char* lookupSymbolTable(SymbolTable* symt,token* key);

void retract(buffer* twinBuffer);
void increaseBuffer(buffer* twinBuffer);
void doubleRetract(buffer* twinBuffer);

token* createToken(vocabulary v, char* lexeme, int linenumber);

char* retractReturnLex(char* dst, buffer* twinBuffer, int max_len);
void removeComments(char *testcaseFile, char *cleanFile);


#endif



