#ifndef PARSERDEF_H
#define PARSERDEF_H
#include "lexer.h"
#include "lexerDef.h"

#define grammar "grammar.txt"

typedef enum {
    NT_program,
    NT_mainFunction,
    NT_otherFunctions,
    NT_function,
    NT_input_par,
    NT_output_par,
    NT_parameter_list,
    NT_remaining_list,
    NT_dataType,
    NT_primitiveDatatype,
    NT_constructedDatatype,
    NT_stmts,
    NT_typeDefinitions,
    NT_actualOrRedefined,
    NT_typeDefinition,
    NT_fieldDefinitions,
    NT_fieldDefinition,
    NT_fieldType,
    NT_moreFields,
    NT_declarations,
    NT_declaration,
    NT_global_or_not,
    NT_otherStmts,
    NT_stmt,
    NT_assignmentStmt,
    NT_singleOrRecId,
    NT_option_single_constructed,
    NT_oneExpansion,
    NT_moreExpansions,
    NT_funCallStmt,
    NT_outputParameters,
    NT_inputParameters,
    NT_iterativeStmt,
    NT_conditionalStmt,
    NT_elsePart,
    NT_ioStmt,
    NT_arithmeticExpression,
    NT_expPrime,
    NT_term,
    NT_termPrime,
    NT_factor,
    NT_highPrecedenceOperators,
    NT_lowPrecedenceOperators,
    NT_booleanExpression,
    NT_var,
    NT_logicalOp,
    NT_relationalOp,
    NT_returnStmt,
    NT_optionalReturn,
    NT_idList,
    NT_more_ids,
    NT_definetypestmt,
    NT_A,

    NONTERMINAL_COUNT
} NonTerminal;



#define EPSILON   -1
// #define DOLLAR    -2


typedef unsigned long long Set;


typedef struct {
    int* rhs;
    int len;
    int cap;
} Production;


typedef struct {
    Production* prods;
    int count;
    int cap;
} RuleSet;


typedef struct {
    RuleSet rules[NONTERMINAL_COUNT];
} Grammar;


typedef enum { RULE, BLANK, SYNCH } State;

typedef struct {
    int ruleIndex;
    State state;
} TableCell;

typedef struct {
    TableCell table[NONTERMINAL_COUNT][TOTAL_TERMINALS_COUNT];
} ParseTable;


typedef struct TreeNode{
    int symbol;
    token* tk;
    struct TreeNode** children;
    int childCount;
    int cap;
} TreeNode;

#endif