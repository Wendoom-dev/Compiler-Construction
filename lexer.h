#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

buffer* makeBuffer(FILE* fp);
void freeBuffer(buffer* twinBuffer);
void resetBuffer(buffer* twinBuffer);

void getStream(buffer* twinBuffer);
token* getNextToken(buffer* twinBuffer); 

symbolTable* makeSymbolTable();
void freeSymbolTable(symbolTable* symt);
void insertIntoSymbolTable(symbolTable* symt, token* key);
char* lookupSymbolTable(symbolTable* symt, token* key);

void retract(buffer* twinBuffer);
void retractK(buffer* twinBuffer, int k); // Could be used for double retract or more in the future
void increaseBuffer(buffer* twinBuffer);

token* createToken(vocab v, char* lexeme, int linenumber);
token* createErrorToken(char* lexeme, int linenumber);

void extractLexeme(buffer* twinBuffer, char* dest);
vocab checkKeyword(char* lexeme);

void removeComments(char *testcaseFile, char *cleanFile);

#endif