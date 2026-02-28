#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "lexer.h"
#include "parser.h"

#define INITIAL_TOKEN_CAP 1024

void printImplementationStatus() {
    printf("--------------------------------------------------\n");
    printf("Implementation Status:\n");
    printf("(a) FIRST and FOLLOW set automated\n");
    printf("(b) Lexical analyzer module developed\n");
    printf("(c) Both lexical and syntax analysis modules implemented\n");
    printf("--------------------------------------------------\n\n");
}

token** collectTokens(char* inputFile, int* tokenCount) {

    FILE* fp = fopen(inputFile, "r");
    if (!fp) {
        printf("Error opening file\n");
        return NULL;
    }

    buffer* twinBuffer = makeBuffer();
    getStream(fp, twinBuffer);

    int capacity = INITIAL_TOKEN_CAP;
    token** tokens = malloc(sizeof(token*) * capacity);

    *tokenCount = 0;

    token* tok;

    while ((tok = getNextToken(twinBuffer)) != NULL) {

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

    removeComments(inputFile, cleanFile);

    FILE* fp = fopen(cleanFile, "r");
    if (!fp) {
        printf("Error opening clean file\n");
        return;
    }

    char ch;
    while ((ch = fgetc(fp)) != EOF)
        printf("%c", ch);

    fclose(fp);
}

void optionPrintTokens(char* inputFile) {

    int tokenCount;
    token** tokens = collectTokens(inputFile, &tokenCount);

    if (!tokens) return;

    printf("\n%-15s %-20s %-10s\n", "Token", "Lexeme", "LineNo");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < tokenCount; i++) {

        if (tokens[i]->tokenName == TK_DOLLAR)
            break;

        printf("%-15d %-20s %-10d\n",
               tokens[i]->tokenName,
               tokens[i]->lexeme,
               tokens[i]->lineNum);
    }

    for (int i = 0; i < tokenCount; i++)
        free(tokens[i]);

    free(tokens);
}

void optionParse(char* inputFile, char* parseTreeFile) {

    int tokenCount;
    token** tokens = collectTokens(inputFile, &tokenCount);
    if (!tokens) return;

    Grammar g;
    ParseTable pt;

    initGrammar(&g);
    initParseTable(&pt);

    /* YOU must populate grammar + parse table here */

    int startSymbol = 0;   // replace with actual start symbol enum

    TreeNode* root = parse(&pt, &g, tokens, tokenCount, startSymbol);

    if (!root) {
        printf("Parsing failed.\n");
    } else {
        printf("Input source code is syntactically correct.\n");

        FILE* out = fopen(parseTreeFile, "w");
        if (!out) {
            printf("Error opening parse tree output file\n");
        } else {
            printTree(root, 0);  // console version
            fclose(out);
        }
    }

    for (int i = 0; i < tokenCount; i++)
        free(tokens[i]);

    free(tokens);
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

    Grammar g;
    ParseTable pt;

    initGrammar(&g);
    initParseTable(&pt);

    int startSymbol = 0;

    parse(&pt, &g, tokens, tokenCount, startSymbol);

    end_time = clock();

    total_CPU_time = (double)(end_time - start_time);
    total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;

    printf("\nTotal CPU time: %lf\n", total_CPU_time);
    printf("Total CPU time in seconds: %lf\n", total_CPU_time_in_seconds);

    for (int i = 0; i < tokenCount; i++)
        free(tokens[i]);

    free(tokens);
}

/* ------------------------------------------------------------ */
/* MAIN                                                          */
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

        scanf("%d", &option);

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
