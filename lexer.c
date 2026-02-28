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

char getNextChar(int index, buffer* buffer) {
    if(twin_buffer -> eof != -1 && index == twin_buffer -> fwdPtr > twin_buffer -> eof) {
        return '\0';
    return (index < BUFFER_SIZE) ? twin_buffer->buf1[index]  : twin_buffer->buf2[index - BUFFER_SIZE];
    }
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

token* getNextToken(buffer* twinBuffer) {
    char c;
    char lexeme[MAX_LEX_LENGTH];
    int lexLen = 0;

    while(1) {
        c = getNextChar(twinBuffer->fwdPtr, twinBuffer);

        switch(dfaState) {
            case 0: //Start state

                if (c == '\0' || c == EOF) {
                    return NULL;
                }

                else if(c == ' ' || c == '\t' || c == '\n') {
                    increaseBuffer(twinBuffer);
                    twinBuffer->currPtr = twinBuffer->fwdPtr;
                }

                else if (c == '\n') {
                    currLineNum++;
                    increaseBuffer(twinBuffer);
                    twinBuffer->currPtr = twinBuffer->fwdPtr;
                }

                else if(c == '<') {
                    increaseBuffer(twinBuffer);
                    dfaState = 1;
                }
                else if(c == '&') {
                    increaseBuffer(twinBuffer);
                    dfaState = 7;
                }
                else if(c == '@') {
                    increaseBuffer(twinBuffer);
                    dfaState = 10;
                }
                else if(c == '+' || c == '*' || c == '-' || c == '/' || c == '~' || c == ',' || c == ';' || c == ':' || c == '[' || c == ']' || c == '(' || c == ')') {
                    dfaState = 13;
                }
                else if (c == '!') {
                    increaseBuffer(twinBuffer);
                    dfaState = 14;
                }
                else if (c >= 'b' && c <= 'd') {
                    increaseBuffer(twinBuffer);
                    dfaState = 16;
                }
                else if (c >= 'a' && c <= 'z') {
                    increaseBuffer(twinBuffer);
                    dfaState = 20;
                }
                else if (c >= '0' && c <= '9') {
                    increaseBuffer(twinBuffer);
                    dfaState = 22;
                }
                else if (c == '#') {
                    increaseBuffer(twinBuffer);
                    dfaState = 31;
                }
                else if (c == '_') {
                    increaseBuffer(twinBuffer);
                    dfaState = 34;
                }
                else if (c == '>') {
                    increaseBuffer(twinBuffer);
                    dfaState = 38;
                }

                else {
                    increaseBuffer(twinBuffer);
                    twinBuffer->currPtr = twinBuffer->fwdPtr;
                }
                break;

            case 1: // read '<'
                if(c == '=') {
                    increaseBuffer(twinBuffer);
                    dfaState = 2;
                }

                else if(c == '-') {
                    increaseBuffer(twinBuffer);
                    dfaState = 4;
                }

                else {
                    retract(twinBuffer);
                    dfaState = 3;
                }
                break;
                
            case 2: // read '<='
                dfaState = 0;
                twinBuffer->currPtr = twinBuffer->fwdPtr;
                return createToken(TK_LE, "<=", currLineNum);

            case 3: // read other
                dfaState = 0;
                twinBuffer->currPtr = twinBuffer->fwdPtr;
                return createToken(TK_LT, "<", currLineNum);
            
            case 4: // read '<-'
                if(c == '-') {
                    increaseBuffer(twinBuffer);
                    dfaState = 5;
                }
                //double retraction to be implemented
                break;
            
            case 5: // read '<--'
                if(c == '-') {
                    increaseBuffer(twinBuffer);
                    dfaState = 6;
                }
                // incomplete error
                break;
            
            case 6: // read '<---'
                dfaState = 0;
                twinBuffer->currPtr = twinBuffer->fwdPtr;
                return createToken(TK_ASSIGNOP, "<--", currLineNum);
                
            case 7: // read '&'
                if(c == '&') {
                    increaseBuffer(twinBuffer);
                    dfaState = 8;
                }
                // unknown error
                break;
                
            case 8: // read '&&'
                if(c == '&') {
                    increaseBuffer(twinBuffer);
                    dfaState = 9;
                }
                // unknown error
                break;
                
            case 9: // read '&&&'
                dfaState = 0;
                twinBuffer->currPtr = twinBuffer->fwdPtr;
                return createToken(TK_AND, "&&&", currLineNum);

            case 10: // read '@'
                if(c == '@') {
                    increaseBuffer(twinBuffer);
                    dfaState = 11;
                }
                // unknown error
                break;
            
            case 11: // read '@@'
                if(c == '@') {
                    increaseBuffer(twinBuffer);
                    dfaState = 12;
                }
                // unknown error
                break;

            case 12: // read '@@@'
                dfaState = 0;
                twinBuffer->currPtr = twinBuffer->fwdPtr;
                return createToken(TK_ASSIGNOP, "@@@", currLineNum);
                break;
            
            case 13:
                increaseBuffer(twinBuffer);
                dfaState = 0;
                twinBuffer->currPtr = twinBuffer->fwdPtr;
                switch(c) {
                    case '+': return createToken(TK_PLUS, "+", currLineNum);
                    case '-': return createToken(TK_MINUS, "-", currLineNum);
                    case '*': return createToken(TK_MUL, "*", currLineNum);
                    case '/': return createToken(TK_DIV, "/", currLineNum);
                    case '~': return createToken(TK_NOT, "~", currLineNum);
                    case ',': return createToken(TK_COMMA, ",", currLineNum);
                    case ';': return createToken(TK_SEM, ";", currLineNum);
                    case ':': return createToken(TK_COLON, ":", currLineNum);
                    case '[': return createToken(TK_SQL, "[", currLineNum);
                    case ']': return createToken(TK_SQR, "]", currLineNum);
                    case '(': return createToken(TK_OP, "(", currLineNum);
                    case ')': return createToken(TK_CL, ")", currLineNum);
                    default: return NULL;
                }
            
            case 14: // read '!'
                if(c == '=') {
                    increaseBuffer(twinBuffer);
                    dfaState = 15;
                }
                // unknown error
                break;

            case 15: // read '!='
                dfaState = 0;
                twinBuffer->currPtr = twinBuffer->fwdPtr;
                return createToken(TK_NE, "!=", currLineNum);

            case 16: // read b-d
                if(c >= 'a' && c <= 'z') {
                    increaseBuffer(twinBuffer);
                    dfaState = 20;
                }

                else if (c >= '2' && c <= '7') {
                    increaseBuffer(twinBuffer);
                    dfaState = 17;
                }

                break;
            
            case 17: // read b-d followed by 2-7
                if (c >= 'b' && c <= 'd') {
                    increaseBuffer(twinBuffer);
                    dfaState = 17;
                }

                else if (c >= '2' && c <= '7') {
                    increaseBuffer(twinBuffer);
                    dfaState = 18;
                }

                else {
                    retract(twinBuffer);
                    dfaState = 19;
                }
                break;
            
            case 18: // read b-d followed by 2-7 followed by b-d or 2-7
                if(c >= '2' && c <= '7') {
                    increaseBuffer(twinBuffer);
                    dfaState = 18;
                }

                else {
                    retract(twinBuffer);
                    dfaState = 19;
                }
            
            case 19: // read other
                dfaState = 0;
                twinBuffer->currPtr = twinBuffer->fwdPtr;
                return createToken(TK_ID, lexeme, currLineNum);
            
            case 20: // read a-z
                if(c >= 'a' && c <= 'z') {
                    increaseBuffer(twinBuffer);
                    dfaState = 20;
                }

                else {
                    retract(twinBuffer);
                    dfaState = 21;
                }
                break;
            
            case 21: // read other
                dfaState = 0;
                twinBuffer->currPtr = twinBuffer->fwdPtr;
                return createToken(TK_FIELDID, lexeme, currLineNum);

            case 22: // read 0-9
                if(c >= '0' && c <= '9') {
                    increaseBuffer(twinBuffer);
                    dfaState = 22;
                }
                
                else if (c == '.') {
                    increaseBuffer(twinBuffer);
                    dfaState = 23;
                }

                else {
                    retract(twinBuffer);
                    dfaState = 30;
                }
                break;

            case 23: // read 0-9 followed by '.'
                if(c >= '0' && c <= '9') {
                    increaseBuffer(twinBuffer);
                    dfaState = 24;
                }
                break;

            case 24: // read 0-9 followed by '.' followed by 0-9
                if (c >= '0' && c <= '9') {
                    increaseBuffer(twinBuffer);
                    dfaState = 24;
                }
                break;
            
            case 25: // read 0-9 followed by '.' followed by 0-9 followed by 0-9
                if(c == 'E') {
                    increaseBuffer(twinBuffer);
                    dfaState = 26;
                }

                else {
                    retract(twinBuffer);
                    dfaState = 30;
                }
                break;
            
            case 26: // read 0-9 followed by '.' followed by 0-9 followed by 0-9 followed by 'E'
                if(c == '+' || c == '-') {
                    increaseBuffer(twinBuffer);
                    dfaState = 27;
                }
                else if(c >= '0' && c <= '9') {
                    increaseBuffer(twinBuffer);
                    dfaState = 28;
                }
                break;
            
            case 27: // read 0-9 followed by '.' followed by 0-9 followed by 0-9 followed by 'E' followed by '+' or '-'
                if(c >= '0' && c <= '9') {
                    increaseBuffer(twinBuffer);
                    dfaState = 28;
                }
                break;

            case 28: // read 0-9 followed by '.' followed by 0-9 followed by 0-9 followed by 'E' followed by '+' or '-' followed by 0-9
                if(c >= '0' && c <= '9') {
                    increaseBuffer(twinBuffer);
                    dfaState = 29;
                }
                break;

            case 29:
                retract(twinBuffer);
                dfaState = 30;
                break;

            case 30: // read other
                dfaState = 0;
                twinBuffer->currPtr = twinBuffer->fwdPtr;
                return createToken(TK_NUM, lexeme, currLineNum);
            
            case 31: // read '#'
                if(c >= 'a' && c <= 'z') {
                    increaseBuffer(twinBuffer);
                    dfaState = 32;
                }
                break;
            
            case 32: // read '#' followed by a-z
                if(c >= 'a' && c <= 'z') {
                    increaseBuffer(twinBuffer);
                    dfaState = 32;
                }
                else {
                    retract(twinBuffer);
                    dfaState = 33;
                }
                break;

            case 33: // read other 
                dfaState = 0;
                twinBuffer->currPtr = twinBuffer->fwdPtr;
                return createToken(TK_RUID, lexeme, currLineNum);
            
            case 34: // read '_'
                if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                    increaseBuffer(twinBuffer);
                    dfaState = 35;
                }
                break;
            
            case 35: // read '_' followed by a-z or A-Z
                if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                    increaseBuffer(twinBuffer);
                    dfaState = 35;
                }
                else if(c >= '0' && c <= '9') {
                    increaseBuffer(twinBuffer);
                    dfaState = 36;
                }
                else {
                    retract(twinBuffer);
                    dfaState = 37;
                }
                break;
            
            case 36: // read '_' followed by a-z or A-Z followed by 0-9
                if(c >= '0' && c <= '9') {
                    increaseBuffer(twinBuffer);
                    dfaState = 36;
                }
                else {
                    retract(twinBuffer);
                    dfaState = 37;
                }
                break;
            
            case 37: // read other
                dfaState = 0;
                twinBuffer->currPtr = twinBuffer->fwdPtr;
                return createToken(TK_FUNID, lexeme, currLineNum);

            case 38: // read '>'
                if(c == '=') {
                    increaseBuffer(twinBuffer);
                    dfaState = 40;
                }
                else {
                    retract(twinBuffer);
                    dfaState = 39;
                }
                break;
            
            case 39: // read other
                dfaState = 0;
                twinBuffer->currPtr = twinBuffer->fwdPtr;
                return createToken(TK_GT, ">", currLineNum);

            case 40: // read '>='
                dfaState = 0;
                twinBuffer->currPtr = twinBuffer->fwdPtr;
                return createToken(TK_GE, ">=", currLineNum);
        }
}


