#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define MAX_GRAMMAR_SYMBOL_LENGTH 30
#define MAX_TOKEN_NUMBER 100
#define MAX_STATE_NUMBER 1000
#define MAX_TREE_CHILD 10
#define MAX_NUM_VAR 1000

#define SYMBOLS_FILE "Symbols.txt"
// 0 - $
// 1 - P 
#define GRAMMAR_FILE "Grammar.txt"
// 0: P->...

// Global Data
struct AllSymbols* symbols;
struct Grammar* g;
struct firstSets* first_sets;
struct LR1Table* tableLR1;
struct AllLR1Items4NT* r4NT;

struct Symbol{
    char symbol[MAX_GRAMMAR_SYMBOL_LENGTH];
    char isTerminal;
};


//Symbols are processed as int in memory
//i.e. index of struct Symbol* symbols array
//
// Manually define in Symbols.txt:
// 0 - $
// 1 - P
struct AllSymbols{
    struct Symbol allSymbols[MAX_TOKEN_NUMBER];
    int numSymbols;
};

struct GrammarRule{
    int left;
    int right[100];
    int numRight;
};

struct Grammar{
    struct GrammarRule rules[100];
    int num;
};

struct intSet{
    int elements[200];
    int num;
};

struct firstSets{
    int numSym;
    struct intSet firstSets[200];
    struct Grammar* g;
    struct AllSymbols symbols;
};

struct followSets{
    struct firstSets* fS;
    int num;
    struct intSet followSets[200];
};



struct Items4NonT{
    int token;
    struct StateItem* items[200];
    int num;
};

struct AllItems4NonT{
    int num;
    struct Items4NonT* itemsSets[100];
};


struct StateItem{
    struct GrammarRule rule;
    int position;
};

struct State{
    struct StateItem* items[200];
    int num;
};


//Each row for SLRTable
struct Actions4State{
    int num; //number of columns

    // 1 reduce
    // 0 shift
    // -1 error
    int action[MAX_TOKEN_NUMBER];

    // if it's shift
    struct State* GOTOs[MAX_TOKEN_NUMBER];
    int index_GOTO_state[MAX_TOKEN_NUMBER]; //the index of the staet to goto

    // if it's reduce
    const struct StateItem* reduceRules[MAX_TOKEN_NUMBER];
}; 


struct SLRTable{
    int numState;
    
    struct State* states[MAX_STATE_NUMBER];

    // index for i-th State*
    struct Actions4State* actions4state[MAX_STATE_NUMBER];
};


struct Tree{
    int node;
    int numChild;
    struct Tree* childs[MAX_TREE_CHILD];
    struct Tree* parent;

    int id;
    void* ptr;
    void* nodeValue;
};



