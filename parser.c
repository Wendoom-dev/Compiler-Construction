#include "parser.h"


static Set FIRST[NONTERMINAL_COUNT];
static Set FOLLOW[NONTERMINAL_COUNT];
static bool FIRST_HAS_EPS[NONTERMINAL_COUNT];


static void setAdd(Set* s, int t){
    *s |= (1ULL << t);
}

static bool setContains(Set s, int t){
    return (s & (1ULL << t)) != 0;
}


static void initProduction(Production* p){
    p->len = 0;
    p->cap = 4;
    p->rhs = malloc(sizeof(int)*p->cap);
}

static void addSymbol(Production* p, int sym){
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

static void addProductionToGrammar(Grammar* g,int nt,Production p){
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

    while(fgets(line,sizeof(line),fp)){

        if(strlen(line)<=1) continue;

        char* lhs = strtok(line," ");
        strtok(NULL," "); // skip ->

        int nt = getNonTerminalID(lhs);

        Production p;
        initProduction(&p);

        char* sym;

        while((sym=strtok(NULL," \n"))!=NULL){

            if(strcmp(sym,"eps")==0){
                p.len = 0;
                break;
            }

            int t = strToEnum(sym);
            if(t != -1){
                addSymbol(&p,t);
            }
            else{
                int nt2 = getNonTerminalID(sym);
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

static void setRule(ParseTable* pt,int nt,int t,int ruleIndex){
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



static TreeNode* createNode(int symbol){
    TreeNode* n = malloc(sizeof(TreeNode));
    n->symbol = symbol;
    n->tk = NULL;
    n->childCount = 0;
    n->cap = 4;
    n->children = malloc(sizeof(TreeNode*)*4);
    return n;
}

static void addChild(TreeNode* parent, TreeNode* child){
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
    stack[++top] = (StackEntry){startSymbol,root};

    int ip=0;

    while(top>=0){

        StackEntry curr = stack[top];
        int currTok =
            (ip<tokenCount)?
            tokens[ip]->tokenName:
            TK_DOLLAR;

        if(curr.symbol>=0){

            if(curr.symbol==currTok){

                if(curr.node)
                    curr.node->tk = tokens[ip];

                top--;
                ip++;
            }
            else{
                printf("Missing terminal at line %d\n",
                       (ip < tokenCount ? tokens[ip]->lineNum : -1));
                errorCount++;
                top--;
            }
        }
        else{

            top--;

            int nt = -curr.symbol-1;
            TableCell cell =
                pt->table[nt][currTok];

            if(cell.state==RULE){

                Production* p =
                    &g->rules[nt].prods[cell.ruleIndex];

                if(p->len==0){
                    TreeNode* epsNode =
                        createNode(EPSILON);
                    addChild(curr.node,epsNode);
                    continue;
                }

                TreeNode* children[p->len];

                for(int i=0;i<p->len;i++){
                    children[i]=
                        createNode(p->rhs[i]);
                    addChild(curr.node,
                             children[i]);
                }

                for(int i=p->len-1;i>=0;i--)
                    stack[++top] =
                        (StackEntry){
                            p->rhs[i],
                            children[i]};
            }
            else if(cell.state==SYNCH){

                printf("Sync recovery at line %d\n",
                       (ip < tokenCount ? tokens[ip]->lineNum : -1));
                errorCount++;
                continue;
            }
            else{ /* BLANK */

                printf("Discarding token at line %d\n",
                       (ip < tokenCount ? tokens[ip]->lineNum : -1));
                errorCount++;
                ip++;
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
