#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

void loadGrammarFromFile(Grammar* g, const char* filename);
void computeFirst(Grammar* g);
void computeFollow(Grammar* g, int startSymbol);
void buildParseTable(ParseTable* pt, Grammar* g);

void initProduction(Production* p);
void addSymbol(Production* p,int sym);

void initGrammar(Grammar* g);
void addProductionToGrammar(Grammar* g,int nt,Production p);

void setAdd(Set* s,int t);
bool setContains(Set s,int t);

void initParseTable(ParseTable* pt);
void setRule(ParseTable* pt,int nt,int t,int rule);
void setSynch(ParseTable* pt,int nt,int t);

TreeNode* createNode(int symbol);
void addChild(TreeNode* p,TreeNode* c);
void printTree(TreeNode* n, int depth, FILE* fp);

TreeNode* parse(ParseTable* pt,
                Grammar* g,
                token** tokens,
                int tokenCount,
                int startSymbol);

#endif