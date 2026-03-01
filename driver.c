#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Ensure this matches your actual header filename
#include "lexer_new.h" 
#include "parser.h"

#define INITIAL_TOKEN_CAP 1024

// Access global variables from lexer to reset them between runs
extern int currLineNum;
extern int dfaState;

void printImplementationStatus() {
    printf("--------------------------------------------------\n");
    printf("Implementation Status:\n");
    printf("(a) FIRST and FOLLOW set automated\n");
    printf("(b) Lexical analyzer module developed\n");
    printf("(c) Both lexical and syntax analysis modules implemented\n");
    printf("--------------------------------------------------\n\n");
}

// Helper function to convert enum to string for clean output
const char* getTokenName(vocab v) {
    static const char* tokenNames[] = {
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
    
    if (v >= 0 && v <= TK_ERROR) return tokenNames[v];
    return "UNKNOWN";
}

token** collectTokens(char* inputFile, int* tokenCount) {

    FILE* fp = fopen(inputFile, "r");
    if (!fp) {
        printf("Error opening file %s\n", inputFile);
        return NULL;
    }

    // CRITICAL: Reset lexer state for multiple menu selections
    currLineNum = 1;
    dfaState = 0;

    // Use updated makeBuffer signature which handles initial getStream
    buffer* twinBuffer = makeBuffer(fp);
    if (!twinBuffer) {
        fclose(fp);
        return NULL;
    }

    int capacity = INITIAL_TOKEN_CAP;
    token** tokens = malloc(sizeof(token*) * capacity);

    *tokenCount = 0;
    token* tok;

    while ((tok = getNextToken(twinBuffer)) != NULL) {
        if (tok->tokenName == TK_ERROR || tok->tokenName == TK_COMMENT) {
            // Lexer already printed the error. Drop it so the parser doesn't crash!
            free(tok);
            continue; 
        }

        if (*tokenCount >= capacity) {
            capacity *= 2;
            tokens = realloc(tokens, sizeof(token*) * capacity);
        }

        tokens[*tokenCount] = tok;
        (*tokenCount)++;

        if (tok->tokenName == TK_DOLLAR)
            break;
    }

    freeBuffer(twinBuffer);
    fclose(fp);

    return tokens;
}

void optionRemoveComments(char* inputFile) {

    char cleanFile[] = "cleanOutput.txt";

    // removeComments is implemented in your lexer code
    removeComments(inputFile, cleanFile);

    FILE* fp = fopen(cleanFile, "r");
    if (!fp) {
        printf("Error opening clean file\n");
        return;
    }

    printf("\n--- Cleaned Source Code ---\n");
    char ch;
    while ((ch = fgetc(fp)) != EOF)
        printf("%c", ch);
    printf("\n---------------------------\n");

    fclose(fp);
}

void optionPrintTokens(char* inputFile) {

    int tokenCount;
    token** tokens = collectTokens(inputFile, &tokenCount);

    if (!tokens) return;

    printf("\n%-18s %-25s %-10s\n", "Token", "Lexeme", "LineNo");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < tokenCount; i++) {
        // Skip printing comment tokens to output if you prefer
        //if (tokens[i]->tokenName == TK_COMMENT) continue;

        printf("%-18s %-25s %-10d\n",
               getTokenName(tokens[i]->tokenName), // Use mapping
               tokens[i]->lexeme,
               tokens[i]->lineNum);
               
        if (tokens[i]->tokenName == TK_DOLLAR)
            break;
    }

    // Clean up memory
    // (Note: Since tokens point to malloc'd structs, we normally free them.
    // However, if your createToken dynamically allocates the token, make sure 
    // it gets freed here. If it allocates the lexeme inside the token, free that too.)
    /* for (int i = 0; i < tokenCount; i++) free(tokens[i]);
    free(tokens); 
    */
}

void optionParse(char* inputFile, char* parseTreeFile) {

    int tokenCount;
    token** tokens = collectTokens(inputFile, &tokenCount);
    if (!tokens) return;

    Grammar g;
    ParseTable pt;

    // 1. Initialize structures
    initGrammar(&g);
    initParseTable(&pt);

    // 2. Load grammar from file, compute sets, and build table
    loadGrammarFromFile(&g, "grammar.txt");
    computeFirst(&g);
    computeFollow(&g, NT_program);
    buildParseTable(&pt, &g);

    // 3. Define the start symbol and parse
    int startSymbol = NT_program; 
    TreeNode* root = parse(&pt, &g, tokens, tokenCount, startSymbol);

    if (!root) {
        printf("Parsing failed.\n");
    } else {
        //printf("Input source code is syntactically correct.\n");

        FILE* out = fopen(parseTreeFile, "w");
        if (!out) {
            printf("Error opening parse tree output file\n");
        } else {
            printTree(root, 0, out);  // Pass the file pointer here!
            fclose(out);
        }
    }
}

/* ------------------------------------------------------------ */
/* Option 4 - Time Measurement                                  */
/* ------------------------------------------------------------ */
void optionTime(char* inputFile) {

    clock_t start_time, end_time;
    double total_CPU_time, total_CPU_time_in_seconds;

    start_time = clock();

    int tokenCount;
    token** tokens = collectTokens(inputFile, &tokenCount);
    
    if(!tokens) return;

    Grammar g;
    ParseTable pt;

    initGrammar(&g);
    initParseTable(&pt);
    
    loadGrammarFromFile(&g, "grammar.txt");
    computeFirst(&g);
    computeFollow(&g, NT_program);
    buildParseTable(&pt, &g);

    int startSymbol = NT_program;

    parse(&pt, &g, tokens, tokenCount, startSymbol);

    end_time = clock();

    total_CPU_time = (double)(end_time - start_time);
    total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;

    printf("\nTotal CPU time: %lf ticks\n", total_CPU_time);
    printf("Total CPU time in seconds: %lf sec\n", total_CPU_time_in_seconds);
}

/* ------------------------------------------------------------ */
/* MAIN                                                         */
/* ------------------------------------------------------------ */
int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Usage: ./stage1exe testcase.txt parsetreeOutFile.txt\n");
        return 1;
    }

    char* inputFile = argv[1];
    char* parseTreeFile = argv[2];

    printImplementationStatus();

    int option;

    while (1) {

        printf("\n0: Exit\n");
        printf("1: Remove comments\n");
        printf("2: Print token list\n");
        printf("3: Parse and print parse tree\n");
        printf("4: Print total time taken\n");
        printf("Enter option: ");

        // Check scanf return to avoid infinite loops on bad input
        if (scanf("%d", &option) != 1) { 
            printf("Invalid input. Exiting...\n");
            break;
        }

        switch (option) {
            case 0:
                printf("Exiting...\n");
                return 0;

            case 1:
                optionRemoveComments(inputFile);
                break;

            case 2:
                optionPrintTokens(inputFile);
                break;

            case 3:
                optionParse(inputFile, parseTreeFile);
                break;

            case 4:
                optionTime(inputFile);
                break;

            default:
                printf("Invalid option.\n");
        }
    }

    return 0;
}