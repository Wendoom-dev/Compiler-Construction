#include "lexer.h"

int dfaState=0;
int currLineNum =0;
symbolTable* symbol_table=NULL;
buffer* twin_buffer;


buffer* makeBuffer(){
    buffer* buf=(buffer*)malloc(sizeof(buffer));
    if(buf==NULL) return NULL;

    buf->fwdPtr=0;
    buf->currPtr=0;
    buf->eof=0;
    buf->currBuffer=1;

    memset(buf->buf1,0 , BUFFER_SIZE);
    memset(buf->buf2,0, BUFFER_SIZE);

    return buf;
}


void freeBuffer(buffer* twinBuffer){
    if(twinBuffer==NULL) return;
    free(twinBuffer);
}

void resetBuffer(buffer* twinBuffer){
    twinBuffer->fwdPtr=0;
    twinBuffer->currPtr=0;
    twinBuffer->eof=0;
    twinBuffer->currBuffer=1;

    memset(twinBuffer->buf1,0 , BUFFER_SIZE);
    memset(twinBuffer->buf2,0, BUFFER_SIZE);

    return;
}

FILE *getStream(FILE *fp, buffer* twinBuffer) {
    if (fp == NULL) {
        printf("File not found\n");
        return NULL;
    }

    if (twinBuffer == NULL) {
        printf("Buffer not found\n");
        return NULL;
    }

    char* target;

    if (twinBuffer->currBuffer == 1) {
        target = twinBuffer->buf1;
        twinBuffer->currBuffer = 2;   
    } else {
        target = twinBuffer->buf2;
        twinBuffer->currBuffer = 1;   
    }

    int bytesRead = fread(target, sizeof(char), BUFFER_SIZE, fp);

    if (bytesRead < BUFFER_SIZE) {
        twinBuffer->eof = 1;   
    }

    return fp;
}

//DJB2 Hash Function - we can change later
unsigned int hashFunction(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;   // hash * 33 + c
    }

    return hash % SYMBOL_TABLE_SIZE;
}


symbolTable* makeSymbolTable(){
    symbolTable* st = (symbolTable*) malloc(sizeof(symbolTable));

    if(st==NULL) return NULL;

    for(int i=0;i<SYMBOL_TABLE_SIZE;i++){
        st->table[i]=NULL;
    }

    return st;
}


void freeSymbolTable(symbolTable* symt){
    if(symt==NULL) return;

    for(int i=0;i<SYMBOL_TABLE_SIZE;i++){
        node* curr= symt->table[i];
        while(curr!=NULL){
            node*temp = curr;
            curr=curr->next;

            free(temp->key);
            free(temp->data);
            free(temp);
        }
    }

    free(symt);
}

void insertIntoSymbolTable(symbolTable* symt, token* key) {
    if (symt == NULL || key == NULL) return;

    unsigned int index = hashFunction(key->lexeme);

    
    node* curr = symt->table[index];
    while (curr != NULL) {
        if (strcmp(curr->key, key->lexeme) == 0) {
            return; 
        }
        curr = curr->next;
    }

   
    node* newNode = (node*)malloc(sizeof(node));
    if (newNode == NULL) return;

    newNode->key = strdup(key->lexeme);   

    
    token* tokCopy = (token*)malloc(sizeof(token));
    if (tokCopy == NULL) {
        free(newNode->key);
        free(newNode);
        return;
    }
    *tokCopy = *key;

    newNode->data = tokCopy;

    
    newNode->next = symt->table[index];
    symt->table[index] = newNode;
}

char* lookupSymbolTable(symbolTable* symt, token* key) {
    if (symt == NULL || key == NULL) return NULL;

    unsigned int index = hashFunction(key->lexeme);

    node* curr = symt->table[index];

    while (curr != NULL) {
        if (strcmp(curr->key, key->lexeme) == 0) {
            return curr->key;   
        }
        curr = curr->next;
    }

    return NULL; 
}

//circular increment and decrement formula used

void retract(buffer* twinBuffer){
    if(twinBuffer == NULL) return;

    twinBuffer->fwdPtr = (twinBuffer->fwdPtr + 2 * BUFFER_SIZE - 1) % (2 * BUFFER_SIZE);
}

void increaseBuffer(buffer* twinBuffer){
    if(twinBuffer == NULL) return;

    twinBuffer->fwdPtr = (twinBuffer->fwdPtr + 1) % (2 * BUFFER_SIZE);
}

void doubleRetract(buffer* twinBuffer){
    if(twinBuffer == NULL) return;

    twinBuffer->fwdPtr  = (twinBuffer->fwdPtr  + 2 * BUFFER_SIZE - 1) % (2 * BUFFER_SIZE);
    twinBuffer->currPtr = (twinBuffer->currPtr + 2 * BUFFER_SIZE - 1) % (2 * BUFFER_SIZE);
}

token* createToken(vocab v, char* lexeme, int linenumber) {
    if (lexeme == NULL) return NULL;

    token* tok = (token*)malloc(sizeof(token));
    if (tok == NULL) return NULL;

    tok->tokenName = v;
    tok->lineNum = linenumber;

    strncpy(tok->lexeme, lexeme, MAX_LEX_LENGTH - 1);
    tok->lexeme[MAX_LEX_LENGTH - 1] = '\0'; // added null termination -> may not be needed?

    return tok;
}


