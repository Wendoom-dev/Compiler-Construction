#include "lexer_new.h"

int dfaState = 0;
int currLineNum = 1; // 1-indexed for better error reporting
symbolTable* symbol_table = NULL;
buffer* twin_buffer = NULL;

// --- Utility Functions ---

buffer* makeBuffer(FILE* fp) {
    buffer* buf = (buffer*)malloc(sizeof(buffer));
    if(buf == NULL) return NULL;

    buf->fp = fp;
    buf->fwdPtr = 0;
    buf->currPtr = 0;
    buf->eof = 0;
    buf->currBuffer = 1;

    memset(buf->buf1, 0, BUFFER_SIZE);
    memset(buf->buf2, 0, BUFFER_SIZE);
    
    // Initial load
    getStream(buf);
    return buf;
}

void freeBuffer(buffer* twinBuffer) {
    if (twinBuffer != NULL) {
        free(twinBuffer);
    }
}

void getStream(buffer* twinBuffer) {
    if (twinBuffer->eof) return;

    char* target = (twinBuffer->currBuffer == 1) ? twinBuffer->buf1 : twinBuffer->buf2;
    int bytesRead = fread(target, sizeof(char), BUFFER_SIZE, twinBuffer->fp);

    if (bytesRead < BUFFER_SIZE) {
        target[bytesRead] = '\0'; 
        twinBuffer->eof = 1;   
    }
}

void increaseBuffer(buffer* twinBuffer) {
    twinBuffer->fwdPtr = (twinBuffer->fwdPtr + 1) % (2 * BUFFER_SIZE);
    
    // Cross-buffer load management
    if (twinBuffer->fwdPtr == BUFFER_SIZE) {
        twinBuffer->currBuffer = 2;
        getStream(twinBuffer);
    } else if (twinBuffer->fwdPtr == 0) {
        twinBuffer->currBuffer = 1;
        getStream(twinBuffer);
    }
}

void retract(buffer* twinBuffer) {
    twinBuffer->fwdPtr = (twinBuffer->fwdPtr + 2 * BUFFER_SIZE - 1) % (2 * BUFFER_SIZE);
}

void retractK(buffer* twinBuffer, int k) {
    twinBuffer->fwdPtr = (twinBuffer->fwdPtr + 2 * BUFFER_SIZE - k) % (2 * BUFFER_SIZE);
}

char getNextChar(buffer* twinBuffer) {
    int index = twinBuffer->fwdPtr;
    return (index < BUFFER_SIZE) ? twinBuffer->buf1[index] : twinBuffer->buf2[index - BUFFER_SIZE];
}

void extractLexeme(buffer* twinBuffer, char* dest) {
    int len = 0;
    int ptr = twinBuffer->currPtr;
    while(ptr != twinBuffer->fwdPtr && len < MAX_LEX_LENGTH - 1) {
        dest[len++] = (ptr < BUFFER_SIZE) ? twinBuffer->buf1[ptr] : twinBuffer->buf2[ptr - BUFFER_SIZE];
        ptr = (ptr + 1) % (2 * BUFFER_SIZE);
    }
    dest[len] = '\0';
}

token* createToken(vocab v, char* lexeme, int linenumber) {
    token* tok = (token*)malloc(sizeof(token));
    tok->tokenName = v;
    tok->lineNum = linenumber;
    strncpy(tok->lexeme, lexeme, MAX_LEX_LENGTH - 1);
    tok->lexeme[MAX_LEX_LENGTH - 1] = '\0';
    return tok;
}

token* createErrorToken(char* lexeme, int linenumber) {
    // 1. Check for identifier length errors
    if (strlen(lexeme) > 20 && lexeme[0] >= 'a' && lexeme[0] <= 'z') {
        printf("Line %d \tError: Variable Identifier is longer than the prescribed length of 20 characters.\n", linenumber);
    } 
    else if (strlen(lexeme) > 30 && lexeme[0] == '_') {
        printf("Line %d \tError: Function Identifier is longer than the prescribed length of 30 characters.\n", linenumber);
    }
    // 2. Check for unknown single symbols
    else if (strlen(lexeme) == 1 && (lexeme[0] == '$' || lexeme[0] == '=' || lexeme[0] == '|')) {
        printf("Line %d Error: Unknown Symbol <%s>\n", linenumber, lexeme);
    }
    // 3. Default to unknown pattern for numbers and weird operators
    else {
        printf("Line %d Error: Unknown pattern <%s>\n", linenumber, lexeme);
    }
    
    return createToken(TK_ERROR, lexeme, linenumber);
}

vocab checkKeyword(char* lexeme) {
    if (strcmp(lexeme, "with") == 0) return TK_WITH;
    if (strcmp(lexeme, "parameters") == 0) return TK_PARAMETERS;
    if (strcmp(lexeme, "parameter") == 0) return TK_PARAMETER;
    if (strcmp(lexeme, "end") == 0) return TK_END;
    if (strcmp(lexeme, "while") == 0) return TK_WHILE;
    if (strcmp(lexeme, "union") == 0) return TK_UNION;
    if (strcmp(lexeme, "endunion") == 0) return TK_ENDUNION;
    if (strcmp(lexeme, "definetype") == 0) return TK_DEFINETYPE;
    if (strcmp(lexeme, "as") == 0) return TK_AS;
    if (strcmp(lexeme, "type") == 0) return TK_TYPE;
    if (strcmp(lexeme, "global") == 0) return TK_GLOBAL;
    if (strcmp(lexeme, "list") == 0) return TK_LIST;
    if (strcmp(lexeme, "input") == 0) return TK_INPUT;
    if (strcmp(lexeme, "output") == 0) return TK_OUTPUT;
    if (strcmp(lexeme, "int") == 0) return TK_INT;
    if (strcmp(lexeme, "real") == 0) return TK_REAL;
    if (strcmp(lexeme, "endwhile") == 0) return TK_ENDWHILE;
    if (strcmp(lexeme, "if") == 0) return TK_IF;
    if (strcmp(lexeme, "then") == 0) return TK_THEN;
    if (strcmp(lexeme, "endif") == 0) return TK_ENDIF;
    if (strcmp(lexeme, "read") == 0) return TK_READ;
    if (strcmp(lexeme, "write") == 0) return TK_WRITE;
    if (strcmp(lexeme, "return") == 0) return TK_RETURN;
    if (strcmp(lexeme, "call") == 0) return TK_CALL;
    if (strcmp(lexeme, "record") == 0) return TK_RECORD;
    if (strcmp(lexeme, "endrecord") == 0) return TK_ENDRECORD;
    if (strcmp(lexeme, "else") == 0) return TK_ELSE;
    return TK_FIELDID;
}

// --- Main Lexer DFA ---

token* getNextToken(buffer* twinBuffer) {
    char c;
    char lexeme[MAX_LEX_LENGTH];

    while(1) {
        c = getNextChar(twinBuffer);

         switch(dfaState) {
            case 0: // Start state
                if (c == '\0') return createToken(TK_DOLLAR, "$", currLineNum);
                
                else if (c == ' ' || c == '\t' || c == '\r') {
                    increaseBuffer(twinBuffer);
                    twinBuffer->currPtr = twinBuffer->fwdPtr;
                }
                else if (c == '\n') {
                    currLineNum++;
                    increaseBuffer(twinBuffer);
                    twinBuffer->currPtr = twinBuffer->fwdPtr;
                }
                else if (c == '%') { increaseBuffer(twinBuffer); dfaState = 60; }
                else if (c == '<') { increaseBuffer(twinBuffer); dfaState = 1; }
                else if (c == '>') { increaseBuffer(twinBuffer); dfaState = 4; }
                else if (c == '=') { increaseBuffer(twinBuffer); dfaState = 5; }
                else if (c == '!') { increaseBuffer(twinBuffer); dfaState = 6; }
                else if (c == '&') { increaseBuffer(twinBuffer); dfaState = 7; }
                else if (c == '@') { increaseBuffer(twinBuffer); dfaState = 9; }
                else if (c == '_') { increaseBuffer(twinBuffer); dfaState = 30; }
                else if (c == '#') { increaseBuffer(twinBuffer); dfaState = 35; }
                else if (c >= '0' && c <= '9') { increaseBuffer(twinBuffer); dfaState = 40; }
                else if (c >= 'b' && c <= 'd') { increaseBuffer(twinBuffer); dfaState = 12; }
                else if (c >= 'a' && c <= 'z') { increaseBuffer(twinBuffer); dfaState = 20; }
                else {
                    increaseBuffer(twinBuffer);
                    extractLexeme(twinBuffer, lexeme);
                    dfaState = 0;
                    twinBuffer->currPtr = twinBuffer->fwdPtr;
                    
                    switch(c) {
                        case '+': return createToken(TK_PLUS, lexeme, currLineNum);
                        case '-': return createToken(TK_MINUS, lexeme, currLineNum);
                        case '*': return createToken(TK_MUL, lexeme, currLineNum);
                        case '/': return createToken(TK_DIV, lexeme, currLineNum);
                        case '~': return createToken(TK_NOT, lexeme, currLineNum);
                        case ',': return createToken(TK_COMMA, lexeme, currLineNum);
                        case ';': return createToken(TK_SEM, lexeme, currLineNum);
                        case ':': return createToken(TK_COLON, lexeme, currLineNum);
                        case '.': return createToken(TK_DOT, lexeme, currLineNum);
                        case '[': return createToken(TK_SQL, lexeme, currLineNum);
                        case ']': return createToken(TK_SQR, lexeme, currLineNum);
                        case '(': return createToken(TK_OP, lexeme, currLineNum);
                        case ')': return createToken(TK_CL, lexeme, currLineNum);
                        default: return createErrorToken(lexeme, currLineNum);
                    }
                }
                break;

            // --- Relational and Logical Operators ---
            case 1: // '<'
                if(c == '=') { increaseBuffer(twinBuffer); extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createToken(TK_LE, lexeme, currLineNum); }
                else if(c == '-') { increaseBuffer(twinBuffer); dfaState = 2; }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createToken(TK_LT, lexeme, currLineNum); }
                break;
            case 2: // '<-'
                if(c == '-') { increaseBuffer(twinBuffer); dfaState = 3; }
                else { 
                    retract(twinBuffer); // We over-consumed '-'. Retract so the next token starts at '-'.
                    extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createToken(TK_LT, lexeme, currLineNum); 
                }
                break;
            case 3: // '<--'
                if(c == '-') { increaseBuffer(twinBuffer); extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createToken(TK_ASSIGNOP, lexeme, currLineNum); }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createErrorToken(lexeme, currLineNum); }
                break;
            case 4: // '>'
                if(c == '=') { increaseBuffer(twinBuffer); extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createToken(TK_GE, lexeme, currLineNum); }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createToken(TK_GT, lexeme, currLineNum); }
                break;
            case 5: // '=='
                if(c == '=') { increaseBuffer(twinBuffer); extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createToken(TK_EQ, lexeme, currLineNum); }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createErrorToken(lexeme, currLineNum); }
                break;
            case 6: // '!='
                if(c == '=') { increaseBuffer(twinBuffer); extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createToken(TK_NE, lexeme, currLineNum); }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createErrorToken(lexeme, currLineNum); }
                break;
            case 7: // '&'
                if(c == '&') { increaseBuffer(twinBuffer); dfaState = 8; }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createErrorToken(lexeme, currLineNum); }
                break;
            case 8: // '&&'
                if(c == '&') { increaseBuffer(twinBuffer); extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createToken(TK_AND, lexeme, currLineNum); }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createErrorToken(lexeme, currLineNum); }
                break;
            case 9: // '@'
                if(c == '@') { increaseBuffer(twinBuffer); dfaState = 10; }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createErrorToken(lexeme, currLineNum); }
                break;
            case 10: // '@@'
                if(c == '@') { increaseBuffer(twinBuffer); extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createToken(TK_OR, lexeme, currLineNum); }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createErrorToken(lexeme, currLineNum); }
                break;
            case 60: // Comments
                if (c == '\n') { 
                    int commentLine = currLineNum; // Save the line number before incrementing
                    currLineNum++; 
                    increaseBuffer(twinBuffer); 
                    
                    // Reset pointers for the next token
                    dfaState = 0; 
                    twinBuffer->currPtr = twinBuffer->fwdPtr; 
                    
                    // Return the comment token with just the "%" lexeme
                    return createToken(TK_COMMENT, "%", commentLine); 
                }
                else if (c == '\0') { 
                    // Handle file ending exactly on a comment
                    dfaState = 0;
                    twinBuffer->currPtr = twinBuffer->fwdPtr;
                    return createToken(TK_COMMENT, "%", currLineNum); 
                }
                else { 
                    // Keep consuming the text inside the comment silently
                    increaseBuffer(twinBuffer); 
                }
                break;

            // --- Identifiers ---
            case 12: // 'b'-'d' initially
                if (c >= '2' && c <= '7') { increaseBuffer(twinBuffer); dfaState = 13; }
                else if (c >= 'a' && c <= 'z') { increaseBuffer(twinBuffer); dfaState = 20; }
                else { 
                    extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; 
                    return createToken(checkKeyword(lexeme), lexeme, currLineNum); 
                }
                break;
            case 13: // TK_ID path [b-d][2-7]
                if (c >= 'b' && c <= 'd') { increaseBuffer(twinBuffer); dfaState = 13; }
                else if (c >= '2' && c <= '7') { increaseBuffer(twinBuffer); dfaState = 14; }
                else {
                    extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr;
                    if(strlen(lexeme) < 2 || strlen(lexeme) > 20) return createErrorToken(lexeme, currLineNum);
                    return createToken(TK_ID, lexeme, currLineNum);
                }
                break;
            case 14: // TK_ID path [b-d][2-7][b-d]*[2-7]*
                if (c >= '2' && c <= '7') { increaseBuffer(twinBuffer); dfaState = 14; }
                else {
                    extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr;
                    if(strlen(lexeme) < 2 || strlen(lexeme) > 20) return createErrorToken(lexeme, currLineNum);
                    return createToken(TK_ID, lexeme, currLineNum);
                }
                break;
            case 20: // TK_FIELDID or Keyword
                if (c >= 'a' && c <= 'z') { increaseBuffer(twinBuffer); dfaState = 20; }
                else {
                    extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr;
                    return createToken(checkKeyword(lexeme), lexeme, currLineNum);
                }
                break;
            case 30: // TK_FUNID '_'
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) { increaseBuffer(twinBuffer); dfaState = 31; }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createErrorToken(lexeme, currLineNum); }
                break;
            case 31: 
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) { increaseBuffer(twinBuffer); dfaState = 31; }
                else if (c >= '0' && c <= '9') { increaseBuffer(twinBuffer); dfaState = 32; }
                else {
                    extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr;
                    if(strcmp(lexeme, "_main") == 0) return createToken(TK_MAIN, lexeme, currLineNum);
                    if(strlen(lexeme) > 30) return createErrorToken(lexeme, currLineNum);
                    return createToken(TK_FUNID, lexeme, currLineNum);
                }
                break;
            case 32:
                if (c >= '0' && c <= '9') { increaseBuffer(twinBuffer); dfaState = 32; }
                else {
                    extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr;
                    if(strlen(lexeme) > 30) return createErrorToken(lexeme, currLineNum);
                    return createToken(TK_FUNID, lexeme, currLineNum);
                }
                break;
            case 35: // TK_RUID '#'
                if (c >= 'a' && c <= 'z') { increaseBuffer(twinBuffer); dfaState = 36; }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createErrorToken(lexeme, currLineNum); }
                break;
            case 36:
                if (c >= 'a' && c <= 'z') { increaseBuffer(twinBuffer); dfaState = 36; }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createToken(TK_RUID, lexeme, currLineNum); }
                break;

            // --- Numbers ---
            case 40: // TK_NUM 
                if (c >= '0' && c <= '9') { increaseBuffer(twinBuffer); dfaState = 40; }
                else if (c == '.') { increaseBuffer(twinBuffer); dfaState = 41; }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createToken(TK_NUM, lexeme, currLineNum); }
                break;
            case 41: // '.' read
                if (c >= '0' && c <= '9') { increaseBuffer(twinBuffer); dfaState = 42; }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createErrorToken(lexeme, currLineNum); } 
                break;
            case 42: // 1st dec digit
                if (c >= '0' && c <= '9') { increaseBuffer(twinBuffer); dfaState = 43; }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createErrorToken(lexeme, currLineNum); } 
                break;
            case 43: // 2nd dec digit (Valid RNUM)
                if (c == 'E') { increaseBuffer(twinBuffer); dfaState = 44; }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createToken(TK_RNUM, lexeme, currLineNum); }
                break;
            case 44: // 'E' read
                if (c == '+' || c == '-') { increaseBuffer(twinBuffer); dfaState = 45; }
                else if (c >= '0' && c <= '9') { increaseBuffer(twinBuffer); dfaState = 46; }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createErrorToken(lexeme, currLineNum); }
                break;
            case 45: // 'E+' or 'E-' read
                if (c >= '0' && c <= '9') { increaseBuffer(twinBuffer); dfaState = 46; }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createErrorToken(lexeme, currLineNum); }
                break;
            case 46: // 1st exp digit
                if (c >= '0' && c <= '9') { 
                    increaseBuffer(twinBuffer); 
                    // Complete 2-digit exponent!
                    extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createToken(TK_RNUM, lexeme, currLineNum); 
                }
                else { extractLexeme(twinBuffer, lexeme); dfaState = 0; twinBuffer->currPtr = twinBuffer->fwdPtr; return createErrorToken(lexeme, currLineNum); }
                break;
        }
    }
}

// Simple strip comments utility
void removeComments(char *testcaseFile, char *cleanFile) {
    FILE *in = fopen(testcaseFile, "r");
    FILE *out = fopen(cleanFile, "w");
    if(!in || !out) return;

    char c;
    bool inComment = false;
    while((c = fgetc(in)) != EOF) {
        if(c == '%') inComment = true;
        else if(c == '\n') inComment = false;
        
        if(!inComment) fputc(c, out);
    }
    fclose(in);
    fclose(out);
}