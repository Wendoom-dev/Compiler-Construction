#include "parser.h"

const char* TerminalNames[] = {
    "TK_ASSIGNOP", "TK_COMMENT", "TK_FIELDID", "TK_ID", "TK_NUM", "TK_RNUM",
    "TK_FUNID", "TK_RUID", "TK_WITH", "TK_END", "TK_WHILE", "TK_UNION",
    "TK_ENDUNION", "TK_DEFINETYPE", "TK_AS", "TK_TYPE", "TK_MAIN",
    "TK_GLOBAL", "TK_PARAMETER", "TK_PARAMETERS", "TK_LIST", "TK_SQL", "TK_SQR",
    "TK_INPUT", "TK_OUTPUT", "TK_INT", "TK_REAL", "TK_COMMA",
    "TK_SEM", "TK_COLON", "TK_DOT", "TK_ENDWHILE", "TK_OP",
    "TK_CL", "TK_IF", "TK_THEN", "TK_ENDIF", "TK_READ",
    "TK_WRITE", "TK_RETURN", "TK_PLUS", "TK_MINUS", "TK_MUL",
    "TK_DIV", "TK_CALL", "TK_RECORD", "TK_ENDRECORD",
    "TK_ELSE", "TK_AND", "TK_OR", "TK_NOT", "TK_LT",
    "TK_LE", "TK_EQ", "TK_GT", "TK_GE", "TK_NE",
    "TK_DOLLAR", "TK_ERROR"
};

const char* NonTerminalNames[] = {
    "program", "mainFunction", "otherFunctions", "function", "input_par",
    "output_par", "parameter_list", "remaining_list", "dataType",
    "primitiveDatatype", "constructedDatatype", "stmts", "typeDefinitions",
    "actualOrRedefined", "typeDefinition", "fieldDefinitions", "fieldDefinition",
    "fieldType", "moreFields", "declarations", "declaration", "global_or_not",
    "otherStmts", "stmt", "assignmentStmt", "singleOrRecId",
    "option_single_constructed", "oneExpansion", "moreExpansions", "funCallStmt",
    "outputParameters", "inputParameters", "iterativeStmt", "conditionalStmt",
    "elsePart", "ioStmt", "arithmeticExpression", "expPrime", "term",
    "termPrime", "factor", "highPrecedenceOperators", "lowPrecedenceOperators",
    "booleanExpression", "var", "logicalOp", "relationalOp", "returnStmt",
    "optionalReturn", "idList", "more_ids", "definetypestmt", "A"
};

int strToEnum(const char* sym) {
    if (strcmp(sym, "TK_ASSIGNOP") == 0) return TK_ASSIGNOP;
    if (strcmp(sym, "TK_COMMENT") == 0) return TK_COMMENT;
    if (strcmp(sym, "TK_FIELDID") == 0) return TK_FIELDID;
    if (strcmp(sym, "TK_ID") == 0) return TK_ID;
    if (strcmp(sym, "TK_NUM") == 0) return TK_NUM;
    if (strcmp(sym, "TK_RNUM") == 0) return TK_RNUM;
    if (strcmp(sym, "TK_FUNID") == 0) return TK_FUNID;
    if (strcmp(sym, "TK_RUID") == 0) return TK_RUID;
    if (strcmp(sym, "TK_WITH") == 0) return TK_WITH;
    if (strcmp(sym, "TK_END") == 0) return TK_END;
    if (strcmp(sym, "TK_WHILE") == 0) return TK_WHILE;
    if (strcmp(sym, "TK_UNION") == 0) return TK_UNION;
    if (strcmp(sym, "TK_ENDUNION") == 0) return TK_ENDUNION;
    if (strcmp(sym, "TK_DEFINETYPE") == 0) return TK_DEFINETYPE;
    if (strcmp(sym, "TK_AS") == 0) return TK_AS;
    if (strcmp(sym, "TK_TYPE") == 0) return TK_TYPE;
    if (strcmp(sym, "TK_MAIN") == 0) return TK_MAIN;
    if (strcmp(sym, "TK_GLOBAL") == 0) return TK_GLOBAL;
    if (strcmp(sym, "TK_PARAMETER") == 0) return TK_PARAMETER;
    if (strcmp(sym, "TK_PARAMETERS") == 0) return TK_PARAMETERS;
    if (strcmp(sym, "TK_LIST") == 0) return TK_LIST;
    if (strcmp(sym, "TK_SQL") == 0) return TK_SQL;
    if (strcmp(sym, "TK_SQR") == 0) return TK_SQR;
    if (strcmp(sym, "TK_INPUT") == 0) return TK_INPUT;
    if (strcmp(sym, "TK_OUTPUT") == 0) return TK_OUTPUT;
    if (strcmp(sym, "TK_INT") == 0) return TK_INT;
    if (strcmp(sym, "TK_REAL") == 0) return TK_REAL;
    if (strcmp(sym, "TK_COMMA") == 0) return TK_COMMA;
    if (strcmp(sym, "TK_SEM") == 0) return TK_SEM;
    if (strcmp(sym, "TK_COLON") == 0) return TK_COLON;
    if (strcmp(sym, "TK_DOT") == 0) return TK_DOT;
    if (strcmp(sym, "TK_ENDWHILE") == 0) return TK_ENDWHILE;
    if (strcmp(sym, "TK_OP") == 0) return TK_OP;
    if (strcmp(sym, "TK_CL") == 0) return TK_CL;
    if (strcmp(sym, "TK_IF") == 0) return TK_IF;
    if (strcmp(sym, "TK_THEN") == 0) return TK_THEN;
    if (strcmp(sym, "TK_ENDIF") == 0) return TK_ENDIF;
    if (strcmp(sym, "TK_READ") == 0) return TK_READ;
    if (strcmp(sym, "TK_WRITE") == 0) return TK_WRITE;
    if (strcmp(sym, "TK_RETURN") == 0) return TK_RETURN;
    if (strcmp(sym, "TK_PLUS") == 0) return TK_PLUS;
    if (strcmp(sym, "TK_MINUS") == 0) return TK_MINUS;
    if (strcmp(sym, "TK_MUL") == 0) return TK_MUL;
    if (strcmp(sym, "TK_DIV") == 0) return TK_DIV;
    if (strcmp(sym, "TK_CALL") == 0) return TK_CALL;
    if (strcmp(sym, "TK_RECORD") == 0) return TK_RECORD;
    if (strcmp(sym, "TK_ENDRECORD") == 0) return TK_ENDRECORD;
    if (strcmp(sym, "TK_ELSE") == 0) return TK_ELSE;
    if (strcmp(sym, "TK_AND") == 0) return TK_AND;
    if (strcmp(sym, "TK_OR") == 0) return TK_OR;
    if (strcmp(sym, "TK_NOT") == 0) return TK_NOT;
    if (strcmp(sym, "TK_LT") == 0) return TK_LT;
    if (strcmp(sym, "TK_LE") == 0) return TK_LE;
    if (strcmp(sym, "TK_EQ") == 0) return TK_EQ;
    if (strcmp(sym, "TK_GT") == 0) return TK_GT;
    if (strcmp(sym, "TK_GE") == 0) return TK_GE;
    if (strcmp(sym, "TK_NE") == 0) return TK_NE;
    if (strcmp(sym, "TK_DOLLAR") == 0) return TK_DOLLAR;

    // Critical: Returns -1 if it's not a terminal, allowing getNonTerminalID to take over.
    return -1; 
}

static Set FIRST[NONTERMINAL_COUNT];
static Set FOLLOW[NONTERMINAL_COUNT];
static bool FIRST_HAS_EPS[NONTERMINAL_COUNT];


void setAdd(Set* s, int t){
    *s |= (1ULL << t);
}

bool setContains(Set s, int t){
    return (s & (1ULL << t)) != 0;
}


void initProduction(Production* p){
    p->len = 0;
    p->cap = 4;
    p->rhs = malloc(sizeof(int)*p->cap);
}

void addSymbol(Production* p, int sym){
    if(p->len == p->cap){
        p->cap *= 2;
        p->rhs = realloc(p->rhs,sizeof(int)*p->cap);
    }
    p->rhs[p->len++] = sym;
}



void initGrammar(Grammar* g){
    for(int i=0;i<NONTERMINAL_COUNT;i++){
        g->rules[i].count = 0;
        g->rules[i].cap = 4;
        g->rules[i].prods = malloc(sizeof(Production)*4);
    }
}

void addProductionToGrammar(Grammar* g,int nt,Production p){
    RuleSet* rs = &g->rules[nt];
    if(rs->count == rs->cap){
        rs->cap *= 2;
        rs->prods = realloc(rs->prods,sizeof(Production)*rs->cap);
    }
    rs->prods[rs->count++] = p;
}


static int getNonTerminalID(char* str){

#define MAP(x) if(strcmp(str,#x)==0) return NT_##x

    MAP(program);
    MAP(mainFunction);
    MAP(otherFunctions);
    MAP(function);
    MAP(input_par);
    MAP(output_par);
    MAP(parameter_list);
    MAP(remaining_list);
    MAP(dataType);
    MAP(primitiveDatatype);
    MAP(constructedDatatype);
    MAP(stmts);
    MAP(typeDefinitions);
    MAP(actualOrRedefined);
    MAP(typeDefinition);
    MAP(fieldDefinitions);
    MAP(fieldDefinition);
    MAP(fieldType);
    MAP(moreFields);
    MAP(declarations);
    MAP(declaration);
    MAP(global_or_not);
    MAP(otherStmts);
    MAP(stmt);
    MAP(assignmentStmt);
    MAP(singleOrRecId);
    MAP(option_single_constructed);
    MAP(oneExpansion);
    MAP(moreExpansions);
    MAP(funCallStmt);
    MAP(outputParameters);
    MAP(inputParameters);
    MAP(iterativeStmt);
    MAP(conditionalStmt);
    MAP(elsePart);
    MAP(ioStmt);
    MAP(arithmeticExpression);
    MAP(expPrime);
    MAP(term);
    MAP(termPrime);
    MAP(factor);
    MAP(highPrecedenceOperators);
    MAP(lowPrecedenceOperators);
    MAP(booleanExpression);
    MAP(var);
    MAP(logicalOp);
    MAP(relationalOp);
    MAP(returnStmt);
    MAP(optionalReturn);
    MAP(idList);
    MAP(more_ids);
    MAP(definetypestmt);
    MAP(A);

    return -1;
}


void loadGrammarFromFile(Grammar* g, const char* filename){
    FILE* fp = fopen(filename,"r");
    if(!fp){
        printf("Cannot open grammar file\n");
        exit(1);
    }

    char line[512];
    int lineNum = 0;

    while(fgets(line,sizeof(line),fp)){
        lineNum++;
        
        // CRITICAL FIX: Strip all newlines and carriage returns immediately
        line[strcspn(line, "\r\n")] = 0;
        
        if(strlen(line) == 0) continue;

        char* lhs = strtok(line," ");
        if (!lhs) continue;

        strtok(NULL," "); // skip ->

        int nt = getNonTerminalID(lhs);
        if(nt == -1) {
            printf("CRITICAL ERROR: Unknown LHS '%s' on line %d of grammar.txt\n", lhs, lineNum);
            exit(1);
        }

        Production p;
        initProduction(&p);

        char* sym;
        // Notice we only delimit by space now, because \r and \n are gone
        while((sym=strtok(NULL," "))!=NULL){ 

            if(strcmp(sym,"eps")==0 || strcmp(sym,"E")==0){
                p.len = 0;
                break;
            }

            int t = strToEnum(sym);
            if(t != -1){
                addSymbol(&p,t);
            }
            else{
                int nt2 = getNonTerminalID(sym);
                if(nt2 == -1) {
                    printf("CRITICAL ERROR: Unknown RHS symbol '%s' on line %d of grammar.txt\n", sym, lineNum);
                    exit(1);
                }
                addSymbol(&p,-nt2-1);
            }
        }
        addProductionToGrammar(g,nt,p);
    }
    fclose(fp);
}


void computeFirst(Grammar* g){

    for(int i=0;i<NONTERMINAL_COUNT;i++){
        FIRST[i]=0;
        FIRST_HAS_EPS[i]=false;
    }

    bool changed=true;

    while(changed){

        changed=false;

        for(int A=0;A<NONTERMINAL_COUNT;A++){

            RuleSet* rs=&g->rules[A];

            for(int r=0;r<rs->count;r++){

                Production* p=&rs->prods[r];

                if(p->len==0){
                    if(!FIRST_HAS_EPS[A]){
                        FIRST_HAS_EPS[A]=true;
                        changed=true;
                    }
                    continue;
                }

                bool nullablePrefix=true;

                for(int i=0;i<p->len;i++){

                    int sym=p->rhs[i];

                    if(sym>=0){
                        if(!setContains(FIRST[A],sym)){
                            setAdd(&FIRST[A],sym);
                            changed=true;
                        }
                        nullablePrefix=false;
                        break;
                    }
                    else{
                        int B=-sym-1;

                        Set before=FIRST[A];
                        FIRST[A]|=FIRST[B];

                        if(before!=FIRST[A])
                            changed=true;

                        if(!FIRST_HAS_EPS[B]){
                            nullablePrefix=false;
                            break;
                        }
                    }
                }

                if(nullablePrefix && !FIRST_HAS_EPS[A]){
                    FIRST_HAS_EPS[A]=true;
                    changed=true;
                }
            }
        }
    }
}

/* ===================== FOLLOW ===================== */

void computeFollow(Grammar* g,int startNT){

    for(int i=0;i<NONTERMINAL_COUNT;i++)
        FOLLOW[i]=0;

    setAdd(&FOLLOW[startNT],TK_DOLLAR);

    bool changed=true;

    while(changed){

        changed=false;

        for(int A=0;A<NONTERMINAL_COUNT;A++){

            RuleSet* rs=&g->rules[A];

            for(int r=0;r<rs->count;r++){

                Production* p=&rs->prods[r];

                for(int i=0;i<p->len;i++){

                    int sym=p->rhs[i];

                    if(sym<0){

                        int B=-sym-1;
                        bool nullableTrail=true;

                        for(int j=i+1;j<p->len;j++){

                            int beta=p->rhs[j];

                            if(beta>=0){
                                if(!setContains(FOLLOW[B],beta)){
                                    setAdd(&FOLLOW[B],beta);
                                    changed=true;
                                }
                                nullableTrail=false;
                                break;
                            }

                            int C=-beta-1;

                            Set before=FOLLOW[B];
                            FOLLOW[B]|=FIRST[C];

                            if(before!=FOLLOW[B])
                                changed=true;

                            if(!FIRST_HAS_EPS[C]){
                                nullableTrail=false;
                                break;
                            }
                        }

                        if(nullableTrail){
                            Set before=FOLLOW[B];
                            FOLLOW[B]|=FOLLOW[A];

                            if(before!=FOLLOW[B])
                                changed=true;
                        }
                    }
                }
            }
        }
    }
}



void initParseTable(ParseTable* pt){
    for(int i=0;i<NONTERMINAL_COUNT;i++)
        for(int j=0;j<TOTAL_TERMINALS_COUNT;j++)
            pt->table[i][j].state=BLANK;
}

void setRule(ParseTable* pt,int nt,int t,int ruleIndex){
    if(pt->table[nt][t].state == RULE){
        printf("LL(1) Conflict at [%d,%d]\n",nt,t);
    }
    pt->table[nt][t].state=RULE;
    pt->table[nt][t].ruleIndex=ruleIndex;
}

void buildParseTable(ParseTable* pt,Grammar* g){

    for(int A=0;A<NONTERMINAL_COUNT;A++){

        RuleSet* rs=&g->rules[A];

        for(int r=0;r<rs->count;r++){

            Production* p=&rs->prods[r];
            bool nullablePrefix=true;

            for(int i=0;i<p->len;i++){

                int sym=p->rhs[i];

                if(sym>=0){
                    setRule(pt,A,sym,r);
                    nullablePrefix=false;
                    break;
                }
                else{
                    int B=-sym-1;

                    for(int t=0;t<TOTAL_TERMINALS_COUNT;t++)
                        if(setContains(FIRST[B],t))
                            setRule(pt,A,t,r);

                    if(!FIRST_HAS_EPS[B]){
                        nullablePrefix=false;
                        break;
                    }
                }
            }

            if(nullablePrefix){
                for(int t=0;t<TOTAL_TERMINALS_COUNT;t++)
                    if(setContains(FOLLOW[A],t))
                        setRule(pt,A,t,r);
            }
        }
    }

    for(int A=0;A<NONTERMINAL_COUNT;A++){
        for(int t=0;t<TOTAL_TERMINALS_COUNT;t++){
            if(pt->table[A][t].state==BLANK &&
               setContains(FOLLOW[A],t)){
                pt->table[A][t].state=SYNCH;
            }
        }
    }
}



TreeNode* createNode(int symbol){
    TreeNode* n = malloc(sizeof(TreeNode));
    n->symbol = symbol;
    n->tk = NULL;
    n->childCount = 0;
    n->cap = 4;
    n->children = malloc(sizeof(TreeNode*)*4);
    return n;
}

void addChild(TreeNode* parent, TreeNode* child){
    if(parent->childCount == parent->cap){
        parent->cap *= 2;
        parent->children =
            realloc(parent->children,
                    sizeof(TreeNode*)*parent->cap);
    }
    parent->children[parent->childCount++] = child;
}


TreeNode* parse(ParseTable* pt,
                Grammar* g,
                token** tokens,
                int tokenCount,
                int startSymbol){

    int errorCount = 0;

    typedef struct{
        int symbol;
        TreeNode* node;
    } StackEntry;

    StackEntry stack[2000];
    int top=-1;

    TreeNode* root = createNode(startSymbol);

    stack[++top] = (StackEntry){TK_DOLLAR,NULL};
    stack[++top] = (StackEntry){-startSymbol - 1, root};

    int ip=0;
    bool errorRecovery = false;
    while(top>=0){

        StackEntry curr = stack[top];
        
        // Grab values safely to prevent segmentation faults
        int currTok = (ip < tokenCount) ? tokens[ip]->tokenName : TK_DOLLAR;
        int lineNum = (ip < tokenCount) ? tokens[ip]->lineNum : -1;
        char* lexStr = (ip < tokenCount) ? tokens[ip]->lexeme : "$";

        if(curr.symbol >= 0){

            if(curr.symbol == currTok){
                if(curr.node) curr.node->tk = tokens[ip];
                top--;
                ip++;
                errorRecovery = false;
            }
            else {
                // TERMINAL MISMATCH
                printf("Line %d Error: The token %s for lexeme %s does not match with the expected token %s\n",
                       lineNum, TerminalNames[currTok], lexStr, TerminalNames[curr.symbol]);
                errorCount++;
                top--; // Pop the expected terminal to attempt recovery
                errorRecovery=true;
            }
        }
        else {

            top--;
            int nt = -curr.symbol - 1;
            TableCell cell = pt->table[nt][currTok];

            if(cell.state == RULE){
                // Normal derivation
                Production* p = &g->rules[nt].prods[cell.ruleIndex];

                if(p->len == 0){
                    TreeNode* epsNode = createNode(EPSILON);
                    addChild(curr.node, epsNode);
                    continue;
                }

                TreeNode* children[p->len];
                for(int i=0; i<p->len; i++){
                    children[i] = createNode(p->rhs[i]);
                    addChild(curr.node, children[i]);
                }
                for(int i=p->len-1; i>=0; i--)
                    stack[++top] = (StackEntry){p->rhs[i], children[i]};
            }
            else if(cell.state == SYNCH){
                // SYNCH RECOVERY
                if (!errorRecovery) { // Only print if not muted
                    printf("Line %d Error: Invalid token %s encountered with value %s stack top %s\n",
                           lineNum, TerminalNames[currTok], lexStr, NonTerminalNames[nt]);
                }
                errorRecovery = true;
                errorCount++;
                continue; // NT is popped, keep token to try against the next stack item
            }
            else { 
                // BLANK STATE (PANIC MODE)
                if (!errorRecovery) { // Only print if not muted
                    printf("Line %d Error: Invalid token %s encountered with value %s stack top %s\n",
                           lineNum, TerminalNames[currTok], lexStr, NonTerminalNames[nt]);
                }
                errorRecovery = true;
                errorCount++;
                
                // THE CURE FOR THE CASCADE:
                // We advance the input pointer to skip the bad token, 
                // BUT we DO NOT push the Non-Terminal back onto the stack!
                // By letting it stay popped, the parser unwinds and perfectly syncs 
                // back up with the next valid block of code (like 'else' or 'end').
                
                if (currTok != TK_DOLLAR) {
                    ip++;
                    stack[++top] = curr;
                }
            }
        }
    }

    if(errorCount==0)
        printf("Input source code is syntactically correct\n");
    else
        printf("Parsing completed with %d errors\n",
               errorCount);

    return root;
}
void printTree(TreeNode* root, int level, FILE* fp) {
    if (root == NULL || fp == NULL) return;

    for (int i = 0; i < level; i++) {
        fprintf(fp, "  ");
    }

    if (root->symbol >= 0) {
        if (root->tk != NULL) {
            fprintf(fp, "-- Terminal: %s (Line %d)\n", root->tk->lexeme, root->tk->lineNum);
        } else {
            fprintf(fp, "-- Terminal ID: %d (Epsilon/Empty)\n", root->symbol);
        }
    } else {
        fprintf(fp, "-> Non-Terminal ID: %d\n", -root->symbol - 1);
    }

    for (int i = 0; i < root->childCount; i++) {
        printTree(root->children[i], level + 1, fp);
    }
}
