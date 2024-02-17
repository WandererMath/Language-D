#ifndef PARSING_C
#define PARSING_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_GRAMMAR_SYMBOL_LENGTH 30
#define MAX_TOKEN_NUMBER 100
#define MAX_STATE_NUMBER 1000
#define MAX_TREE_CHILD 10
#define MAX_NUM_VAR 1000
#define MAX_ITEMS_OF_STATE 100

#define SYMBOLS_FILE "Symbols.txt"
// 0 - $
// 1 - P
// 2 - id
// 3 - literal
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

struct Identifier{
    void* father;
    int type;
    // number -1 = index FOR ALL.
    // int #16
    // float #17

    char name[100];
    void* value;
};

struct LR1Item{
    const struct GrammarRule* rule;
    int position;
    struct intSet* lookahead;
};


struct LR1State{
    struct LR1Item* items[MAX_ITEMS_OF_STATE];
    int num;
};


struct LR1Table{
    // index: state, token
    // 1 reduce, 0 shift, -1 error
    int actions[MAX_STATE_NUMBER][MAX_STATE_NUMBER];
    struct LR1State* states[MAX_STATE_NUMBER];
    void* ref[MAX_STATE_NUMBER][MAX_STATE_NUMBER];
    int num; //number of states
};

struct AllLR1Items4NT{
    int num;
    // index for i-th token
    struct LR1State* itemsSets[MAX_TOKEN_NUMBER];
};


////////////////////////////////////////////
// Struct Decls Above



struct AllSymbols getSymbols(){
    FILE* sfpt;
    char symbol[MAX_GRAMMAR_SYMBOL_LENGTH];
    int isTerminal;
    struct AllSymbols result;
    struct Symbol tempSymbol;

    sfpt=fopen(SYMBOLS_FILE, "r");

    result.numSymbols=0;

    while (!feof(sfpt)){
        memset(tempSymbol.symbol, 0, strlen(tempSymbol.symbol));
        fscanf(sfpt, "%s\t%d", tempSymbol.symbol, &isTerminal);
        tempSymbol.isTerminal=isTerminal;

        result.allSymbols[result.numSymbols]=tempSymbol;
        result.numSymbols+=1;
    }
    fclose(sfpt);
    return result;
}

struct AllSymbols* getSymbols2(){
    FILE* sfpt;
    char symbol[MAX_GRAMMAR_SYMBOL_LENGTH];
    int isTerminal;
    struct AllSymbols* result=malloc(sizeof(struct AllSymbols));
    struct Symbol tempSymbol;

    sfpt=fopen(SYMBOLS_FILE, "r");

    result->numSymbols=0;

    while (!feof(sfpt)){
        memset(tempSymbol.symbol, 0, strlen(tempSymbol.symbol));
        fscanf(sfpt, "%s\t%d", tempSymbol.symbol, &isTerminal);
        tempSymbol.isTerminal=isTerminal;

        result->allSymbols[result->numSymbols]=tempSymbol;
        result->numSymbols+=1;
    }
    fclose(sfpt);
    return result;
}



void printAllSymbols(struct AllSymbols* result){
    for (int i=0; i<result->numSymbols; i++){
        printf("%s\t%d\n", result->allSymbols[i].symbol, result->allSymbols[i].isTerminal);
    }
}

struct GrammarRule createRule(char* s1, struct AllSymbols* allSymbols){
    struct GrammarRule rule;
    char s[100];
    strcpy(s, s1);
    char delim[] = " ";
    char* ptr=strtok(s, delim);

    for (int i=0; i<allSymbols->numSymbols; i++){
        if(strcmp(allSymbols->allSymbols[i].symbol, strdup(ptr))==0){
            rule.left=i;
            break;
        }
    }
    int k=0;
    ptr=strtok(NULL, " ");
    while (ptr!=NULL){
        for (int i=0; i<allSymbols->numSymbols; i++){
            if(strcmp(allSymbols->allSymbols[i].symbol, strdup(ptr))==0){
                rule.right[k]=i;
                break;
            }
        }
        k+=1;
        ptr=strtok(NULL, " ");
    }
    rule.numRight=k;
    return rule;
}

void printRule(struct GrammarRule* rule){
    printf("%d --> ", rule->left);
    for (int i=0; i<rule->numRight; i++){
        printf("%d ", rule->right[i]);
    }
    printf("\n");
}


void printRuleChar(struct GrammarRule* rule){
    printf("%s --> ", symbols->allSymbols[rule->left].symbol);
    for (int i=0; i<rule->numRight; i++){
        printf("%s ", symbols->allSymbols[rule->right[i]].symbol);
    }
    printf("\n");
}

void printRuleText(struct GrammarRule* rule, struct AllSymbols* symbols){
    printf("%s --> ", symbols->allSymbols[rule->left].symbol);
    for (int i=0; i<rule->numRight; i++){
        printf("%s ", symbols->allSymbols[rule->right[i]].symbol);
    }
    printf("\n");
}

struct Grammar readGrammars(struct AllSymbols* allSymbols){
    FILE* fpt=fopen(GRAMMAR_FILE, "r");
    struct Grammar grammar;
    char line[256];
    int i=0;
    while(fgets(line, sizeof(line), fpt)!=NULL){
        if(line[strlen(line)-1]=='\n')
            line[strlen(line)-1]='\0';
        grammar.rules[i]=createRule(line, allSymbols);
        i+=1;
    }
    grammar.num=i;
    fclose(fpt);
    return grammar;
}


struct Grammar* readGrammar2(){
    struct AllSymbols* allSymbols=symbols;
    FILE* fpt=fopen(GRAMMAR_FILE, "r");
    struct Grammar* grammar =malloc(sizeof(struct Grammar));
    char line[256];
    int i=0;
    while(fgets(line, sizeof(line), fpt)!=NULL){
        if(line[strlen(line)-1]=='\n')
            line[strlen(line)-1]='\0';
        grammar->rules[i]=createRule(line, allSymbols);
        i+=1;
    }
    grammar->num=i;
    fclose(fpt);
    return grammar;
}


void printGrammar(struct Grammar* g){
    for (int i=0; i<g->num; i++){
        printRule(&(g->rules[i]));
    }
}



int inSet( int a, const struct intSet* s){
    for (int i=0; i<s->num; i++){
        if (a==s->elements[i]){
            return 1;
        }
    }
    return 0;
}

//return 0 if no change
int addElement(int a, struct intSet* s){
    if (inSet(a, s)){
        return 0;
    }
    s->num+=1;
    s->elements[s->num-1]=a;
    return 1;
}

//add arg1 to arg2
int addSet(const struct intSet* x, struct intSet*s){
    int flag=0;
    for (int i=0;i<x->num; i++){
        if(addElement(x->elements[i], s)){
            flag=1;
        }
    }
    return flag;
}

void printSet(struct intSet* s){
    for (int i=0; i<s->num; i++){
        printf("%d ", s->elements[i]);
    }
    printf("\n");
}


//returns 1 if there is a change
//NO epsilon
int firstSetIter(struct firstSets* fS, int t){
    struct Symbol sym=fS->symbols.allSymbols[t];
    int flag=0;
    if(sym.isTerminal){
        flag=addElement(t, &fS->firstSets[t]);
        return flag;
    }
    //printf("\n%s\n", fS->symbols->allSymbols[t].symbol);
    struct Grammar* g=fS->g;
    for (int i=0; i<g->num; i++){
        //i-th rule:
        if(g->rules[i].left==t){
            if(addSet(&fS->firstSets[g->rules[i].right[0]], &fS->firstSets[t]))
                flag=1;
        }
    }
    return flag;  
}


struct firstSets initializefS(){
    struct AllSymbols allSymbols;
    allSymbols=getSymbols();
    struct Grammar* g=malloc(sizeof(struct Grammar));
    *g=readGrammars(&allSymbols);
    //printGrammar(&g);
    
    struct firstSets fS;
    fS.g=g;
    fS.numSym=allSymbols.numSymbols;
    fS.symbols=allSymbols;
    for (int i=0; i<fS.numSym; i++){
        fS.firstSets[i].num=0;
    }
    //printfS(&fS);
    return fS;
}

void printSetSymbol(struct intSet* s, struct AllSymbols* symbols){
    for(int i=0; i<s->num; i++){
        printf("%s ", symbols->allSymbols[s->elements[i]].symbol);
    }
    printf("\n");
}


void printfS(struct firstSets* fS){
    for(int t=0; t<fS->numSym; t++){
        printf("%s: ", fS->symbols.allSymbols[t].symbol);
        printSetSymbol(&fS->firstSets[t], &fS->symbols);

    }
}


struct firstSets getFirstSets(){
    struct firstSets fS=initializefS();
    int tmp=1;
    while(tmp){
        tmp=0;
        for (int t=0; t<fS.numSym; t++){
            if(firstSetIter(&fS, t)){
                tmp=1;
            }
        }
    }
    return fS;
}





struct followSets initializeFollowSets(struct firstSets* fS){
    struct followSets result;
    result.num=fS->numSym;
    result.fS=fS;
    for (int i=0; i<result.num; i++){
        result.followSets[i].num=0;
    }
    addElement(0, &result.followSets[1]);
    return result;
}

int getFollowSetsIter(struct followSets* result){
    struct Grammar* g=result->fS->g;
    struct GrammarRule* rule;
    int flag=0;
    for (int i=0; i<g->num; i++){
        //i-th rule:
        rule=&g->rules[i];
        for (int j=0; j<rule->numRight-1; j++){
            flag=flag | addSet( &result->fS->firstSets[rule->right[j+1]], &result->followSets[rule->right[j]] );
        }
        flag =flag | addElement(0, &result->followSets[rule->right[rule->numRight-1]]);
        flag =flag | addSet( &result->followSets[rule->left],&result->followSets[rule->right[rule->numRight-1]]);
    }
    return flag;
}

void getFollowSets(struct followSets* result){
    while(getFollowSetsIter(result)){
    }
}

void printFollowSets(struct followSets* fS){
    for(int t=0; t<fS->num; t++){
        printf("%s: ", fS->fS->symbols.allSymbols[t].symbol);
        printSetSymbol(&fS->followSets[t], &fS->fS->symbols);

    }
}







struct AllItems4NonT* getRuleSets4NonTs(const struct Grammar* g, const struct AllSymbols* symbols){
    struct AllItems4NonT* result=malloc(sizeof(struct AllItems4NonT));
    int k=0;
    //k-th nonTerminal

    int m;
    for (int i=0; i<symbols->numSymbols; i++){
        //i-th symbol
        if (symbols->allSymbols[i].isTerminal==1)
            continue;

        // k-th nonTerminal
        // Alloc Memory
        result->itemsSets[k]=malloc(sizeof(struct Items4NonT));
        result->itemsSets[k]->token=i;

        m=0;
        // m-th rule added
        for (int j=0; j<g->num; j++){
            //j-th rule
            if(g->rules[j].left==i){
                //copy j-th rule to stateItem and add position 0
                
                // Alloc Memory
                result->itemsSets[k]->items[m]=malloc(sizeof(struct StateItem));
                result->itemsSets[k]->items[m]->position=0;
                result->itemsSets[k]->items[m]->rule=g->rules[j];
                m++;
            }
            result->itemsSets[k]->num=m;
        }
        k++;
    }
    result->num=k;
    return result;
}

const char* int2symbol(int a, const struct AllSymbols* symbols){
    const char* s= symbols->allSymbols[a].symbol;
    return s;
}


void printItems4NonT(const struct Items4NonT* items, struct AllSymbols* symbols){
    printf("%s:\n", int2symbol(items->token, symbols));
    for (int i=0; i<items->num; i++){
        printRuleText(&items->items[i]->rule, symbols);
    }
}

void printAllItems4NonT(const struct AllItems4NonT* result, struct AllSymbols* symbols){
    for (int i=0; i<result->num; i++){
        printItems4NonT(result->itemsSets[i], symbols);
    }
}


void printStateItem(const struct StateItem* item, const struct AllSymbols* symbols){
    printf("%s --> ", symbols->allSymbols[item->rule.left].symbol);
    for (int i=0; i<item->rule.numRight; i++){
        if(item->position==i)
            printf(".");
        printf("%s ", int2symbol(item->rule.right[i], symbols));
    }
    printf("\n");
}

void printState(const struct State* state, const struct AllSymbols* symbols){
    for (int i=0; i<state->num; i++){
        printStateItem(state->items[i], symbols);
    }
}

struct State* createState(struct State* state, int t){
    struct State* result=malloc(sizeof(struct State));
    for (int i=0; i<state->num; i++){
        // i-th item in the state
        if(state->items[i]->rule.right[state->items[i]->position]==t){
            // if next token is t
            
        }
    }
    return result;
}


int itemEquals(const struct StateItem* s1, const struct StateItem* s2){
    if(s1->position!=s2->position)
        return 0;
    if(s1->rule.left!=s2->rule.left)
        return 0;
    if (s1->rule.numRight!=s2->rule.numRight)
        return 0;
    for (int i=0; i<s1->rule.numRight; i++){
        if(s1->rule.right[i]!=s2->rule.right[i])
            return 0;
    }
    return 1;
}

// 1 if equals
// 0 not
int stateEquals(struct State* s1, struct State* s2){
    if (s1->num!=s2->num)
        return 0;
    for (int i=0; i<s1->num; i++){
        if (itemEquals(s1->items[i], s2->items[i])==0)
            return 0;
    }
    return 1;
}

struct StateItem* generateItem(const struct StateItem* s1, int t){
    struct StateItem* newItem=malloc(sizeof(struct StateItem));
    if(s1->rule.right[s1->position]!=t)
        printf("ERROR!!\n");
    *newItem=*s1;
    newItem->position+=1;
    return newItem;
}


struct State* generateStateInit(const struct State* state, int t, const struct AllItems4NonT* itemsNT, const struct AllSymbols* symbols){
    struct State* newState=malloc(sizeof(struct State));
    newState->num=0;

    struct StateItem* tmpItem;
    for (int i=0; i<state->num; i++){
        //i-th item in old state
        if(state->items[i]->position==state->items[i]->rule.numRight)
            continue;
        if(state->items[i]->rule.right[state->items[i]->position]==t){
            tmpItem=generateItem(state->items[i], t);
            newState->items[newState->num]=tmpItem;
            newState->num+=1;
        }
    }
    return newState;
}

int intIsTerminal(int t, const struct AllSymbols* symbols){
    return symbols->allSymbols[t].isTerminal;
}

// token general index -> index in items3nt 
int getIndex4NTinSet(int token, const struct AllItems4NonT* itemsNT, const struct AllSymbols* symbols){
    for (int i=0; i<itemsNT->num; i++){
        if( strcmp(int2symbol(token, symbols), int2symbol(itemsNT->itemsSets[i]->items[0]->rule.left, symbols))==0)
            return i;
    }
}

void completeState(struct State* state,  const struct AllItems4NonT* rules4NTs, const struct AllSymbols* symbols){
    int k=0;
    int token;
    int flag;
    int t; //for converting index
    struct StateItem* tmpItem;
    while (k<state->num){
        //complete k-th item in the state
        if(state->items[k]->position==state->items[k]->rule.numRight){
            //if the rule ends with dot
            k+=1;
            continue;
        }
        token=state->items[k]->rule.right[state->items[k]->position];
        if (intIsTerminal(token, symbols)){
            k+=1;
            continue;
        }
        t=getIndex4NTinSet(token, rules4NTs, symbols);
        for (int m=0; m<rules4NTs->itemsSets[t]->num; m++){
            flag=0;
            tmpItem=malloc(sizeof(struct StateItem));
            tmpItem=rules4NTs->itemsSets[t]->items[m];
            for (int i=0; i<state->num; i++){
                if(itemEquals(tmpItem, state->items[i])){
                    flag=1;
                    break;
                }
            }
            if(flag){
                continue;
            }
            // To this point, all are checked.
            // The item should be added.
            state->items[state->num]=tmpItem;
            state->num+=1;
            
        }
        k+=1;
    }
}

struct State* createState0(const struct Grammar* g, const struct AllItems4NonT* nt, const struct AllSymbols* symbols){
    struct State* state0=malloc(sizeof(struct State));
    struct StateItem* item;
    state0->num=1;
    for (int i=0; i<1; i++){
        item=malloc(sizeof(struct StateItem));
        item->rule=g->rules[i];
        item->position=0;
        state0->items[i]=item;
    }
    completeState(state0, nt, symbols);
    return state0;
}





//generate for k-th state
void generateSLRTableIter(struct SLRTable* table, int k, const struct followSets* follow_sets, const struct Grammar* g, const struct AllSymbols* symbols, struct AllItems4NonT *itemsNT){
    table->actions4state[k]=malloc(sizeof(struct Actions4State));
    table->actions4state[k]->num=symbols->numSymbols;

    //initialize
    for (int token=0; token <symbols->numSymbols; token++){        
        table->actions4state[k]->action[token]=-1;
    }

    int this_token;
    int next_token;

    int flag;

    struct State* tmpState;

    for (int i=0; i<table->states[k]->num; i++){
        //i-th item of the state

        // reduce
        if(table->states[k]->items[i]->position==table->states[k]->items[i]->rule.numRight){
            this_token=table->states[k]->items[i]->rule.left;
            for (int m=0; m<follow_sets->followSets[this_token].num; m++){
                // Process every element in the follow set of this_token
                next_token=follow_sets->followSets[this_token].elements[m];
                table->actions4state[k]->action[next_token]=1;
                table->actions4state[k]->reduceRules[next_token]=table->states[k]->items[i];
            }
        }
        // shift
        else{
            next_token=table->states[k]->items[i]->rule.right[table->states[k]->items[i]->position];
            table->actions4state[k]->action[next_token]=0;

            // create next state
            tmpState=generateStateInit(table->states[k], next_token, itemsNT, symbols);
            completeState(tmpState, itemsNT, symbols);

            // check whether the tmpState is already in table
            flag=1;
            for (int j=0; j<table->numState; j++){
                if (stateEquals(tmpState, table->states[j])==1){
                    flag=0;
                    free(tmpState);
                    tmpState=table->states[j];
                    table->actions4state[k]->index_GOTO_state[next_token]=j;
                    break;
                }
            }


            table->actions4state[k]->GOTOs[next_token]=tmpState;
            
            if (flag){
                // register the new state in SLRTable
                table->actions4state[k]->index_GOTO_state[next_token]=table->numState;
                table->numState+=1;
                table->states[table->numState-1]=tmpState;

                //Debug
                //printState(tmpState, symbols);
                //printf("\n");
            }
        }

    }
}

struct SLRTable* generateSLRTable(const struct Grammar* g, const struct AllSymbols* symbols, const struct followSets* follow_sets){
    struct SLRTable* table=malloc(sizeof(struct SLRTable));
    struct AllItems4NonT* rules4NTs=getRuleSets4NonTs(g, symbols);
    struct State* state0=createState0(g, rules4NTs, symbols);
    table->states[0]=state0;
    table->numState=1;
    int k=0;
    while (k<table->numState){
        generateSLRTableIter(table, k, follow_sets, g, symbols, rules4NTs);
        k+=1;
    }
    return table;
}


void printSLRTable(const struct SLRTable* table, struct AllSymbols* symbols){
    for (int i=0; i<table->numState; i++){
        printf("State %d:\n", i);
        printState(table->states[i], symbols);
        printf("\n");
    }
    
}







struct intSet* initSet(){
    struct intSet* s= (struct intSet*)malloc(sizeof(struct intSet));
    s->num=0;
    return s;
}

int setEquals(const struct intSet* a, const struct intSet* b){
    if(a->num!=b->num)
        return 0;
    for (int i=0; i< a->num; i++){
        if (0==inSet(a->elements[i], b))
            return 0;
    }
    return 1;
}




struct LR1Item* initLR1Item(const struct GrammarRule* rule){
    struct LR1Item* item=malloc(sizeof(struct LR1Item));
    item->rule=rule;
    item->position=0;
    item->lookahead=initSet();
}





struct LR1State* initLR1State(){
    struct LR1State* state=malloc(sizeof(struct LR1State));
    state->num=0;
    return state;
}



// Add items of s2 to s1
void mergeState(struct LR1State* s1, const struct LR1State* s2){
    for (int i=0; i<s2->num; i++){
        s1->items[s1->num]=s2->items[i];
        s1->num++;
    }
}


int ruleEquals(struct GrammarRule* rule1, struct GrammarRule* rule2){
    if(rule1->left!=rule2->left)
        return 0;
    if(rule1->numRight!=rule2->numRight)
        return 0;
    for (int i=0; i<rule1->numRight; i++){
        if(rule1->right[i]!=rule2->right[i])
            return 0;
    }
    return 1;
}


int itemCoreEquals(struct LR1Item* item1, struct LR1Item* item2){
    if(!ruleEquals(item1->rule, item2->rule))
        return 0;
    if(item1->position!=item2->position)
        return 0;
    return 1;
}


struct LR1Item* itemCoreInState(struct LR1Item* item, struct LR1State* state){
    for (int i=0; i<state->num; i++){
        if(itemCoreEquals(item, state->items[i]))
            return state->items[i];
    }
    return NULL;
}

int itemCoreInState_int(struct LR1Item* item, struct LR1State* state){
    for (int i=0; i<state->num; i++){
        if(itemCoreEquals(item, state->items[i]))
            return 1;
    }
    return 0;
}

int stateCoreEquals(struct LR1State* s1, struct LR1State* s2){
    if(s1->num!=s2->num)
        return 0;
    int flag=0;
    for (int j=0; j<s2->num; j++){
        flag=1;
        for (int i=0; i<s1->num; i++){
            if (itemCoreInState_int(s1->items[i], s2)==1){
                continue;
            }
            flag=0;
            break;
        }
        if(flag==0)
            return 0;   
    }
    return 1;
}

// Final result in s1
void combineStatesWithSameCore(struct LR1State* s1, struct LR1State* s2){
    if(!stateCoreEquals(s1, s2)){
        printf("ERROR: States core not equal\n");
        return ;
    }
    for (int i=0; i<s1->num; i++){
        for(int j=0; j<s2->num; j++){
            if(itemCoreEquals(s1->items[i], s2->items[j])){
                addSet(s2->items[j]->lookahead, s1->items[i]->lookahead);
                break;
            }
        }
    }

}


//--------------------------------------------
//The Same as LR1State
/*
struct LR1Items4NT{
    struct LR1Item* items[200];
    int num;
};*/



struct AllLR1Items4NT* initAllLR1Items4NT(){
    struct AllLR1Items4NT* result=(struct AllLR1Items4NT*)malloc(sizeof(struct AllLR1Items4NT));

    int m;
    for (int i=0; i<symbols->numSymbols; i++){
        //i-th symbol
        if (symbols->allSymbols[i].isTerminal==1)
            continue;

        // Alloc Memory
        result->itemsSets[i]=malloc(sizeof(struct LR1State));

        m=0;
        // m-th rule added
        for (int j=0; j<g->num; j++){
            //j-th rule
            if(g->rules[j].left==i){
                //copy j-th rule to stateItem and add position 0
                
                // Alloc Memory
                result->itemsSets[i]->items[m]=malloc(sizeof(struct StateItem));
                result->itemsSets[i]->items[m]->position=0;
                result->itemsSets[i]->items[m]->rule=&g->rules[j];
                m++;
            }
            result->itemsSets[i]->num=m;
        }
    }
    result->num=symbols->numSymbols;
    return result;
}


void printR4NT(){
    for(int i=0;i<r4NT->num; i++){
        if(!symbols->allSymbols[i].isTerminal){
            printf("%s:\n",symbols->allSymbols[i].symbol);
            for(int j=0; j<r4NT->itemsSets[i]->num; j++){
                printRuleChar(r4NT->itemsSets[i]->items[j]->rule);
            }
            printf("\n");
        }
    }
}

int IsTerminal(int t){
    return symbols->allSymbols[t].isTerminal;
}

int completeLR1State(struct LR1State* state){
    int N0=state->num;
    int token;
    int N;
    int p;
    struct LR1Item* tmp;
    for (int i=0; i<state->num; i++){
        if(state->items[i]->position==state->items[i]->rule->numRight)
            continue;
        if(IsTerminal(state->items[i]->rule->right[state->items[i]->position]))
            continue;
        token=state->items[i]->rule->right[state->items[i]->position];
        p=state->items[i]->position;
        N=state->items[i]->rule->numRight;
        
        for(int j=0; j<r4NT->itemsSets[token]->num; j++){
            tmp=malloc(sizeof(struct LR1Item));
            *tmp=*r4NT->itemsSets[token]->items[j];
            tmp->lookahead=initSet();
                
            if(p==N-1)    
                *tmp->lookahead=*state->items[i]->lookahead;
            else
                *tmp->lookahead=first_sets->firstSets[state->items[i]->rule->right[p+1]];


            struct LR1Item* target=itemCoreInState(tmp, state);
            if(target==NULL){
                state->items[state->num]=tmp;
                state->num++;
            }
            else{
                addSet(tmp->lookahead, target->lookahead);
                free(tmp);
            }
        
        }


    }
    int N1=state->num;
    if(N0==N1)
        return 0;
    return 1;
}


void initLR1Table(){
    tableLR1=malloc(sizeof(struct LR1Table));
    tableLR1->num=1;
    tableLR1->states[0]=malloc(sizeof(struct LR1State));

    // Adding rules for P
    *tableLR1->states[0]=*r4NT->itemsSets[1];
    tableLR1->states[0]->items[0]->lookahead=initSet();
    addElement(0, tableLR1->states[0]->items[0]->lookahead);
    completeLR1State(tableLR1->states[0]);
}

struct LR1Item* shiftLR1Item(struct LR1Item* item){
    struct LR1Item* newItem=malloc(sizeof(struct LR1Item));
    if(item->position>=item->rule->numRight){
        printf("ERROR. Already Rightmost. Can't Shift");
        return ;
    }
    *newItem=*item;
    newItem->position+=1;
    return newItem;
}


void makeLR1TableIter(int state_index){
    int p;
    int N;
    int flag;
    struct LR1Item* item;
    struct LR1State* newState;
    struct LR1Item* newItem;
    //init all to error state
    for (int t=0; t<symbols->numSymbols; t++){
        tableLR1->actions[state_index][t]=-1;
    }

    for (int t=0; t<symbols->numSymbols; t++){
        // Iterate througth symbols t
        flag=0;
        for (int i=0; i<tableLR1->states[state_index]->num; i++){
            
            p=tableLR1->states[state_index]->items[i]->position;
            N=tableLR1->states[state_index]->items[i]->rule->numRight;
            item=tableLR1->states[state_index]->items[i];
            // Reduce
            if(p==N){
                if(inSet(t, item->lookahead)){
                    tableLR1->actions[state_index][t]=1;
                    tableLR1->ref[state_index][t]=item;
                }
            }

            //Shift
            else{
                
                if(t==item->rule->right[p]){
                    flag=1;
                    newItem=shiftLR1Item(item);
                    if(tableLR1->actions[state_index][t]==0){
                        newState=tableLR1->ref[state_index][t];
                        newState->items[newState->num]=newItem;
                        newState->num++;
                    }
                    else{
                        tableLR1->actions[state_index][t]=0;
                        newState=initLR1State();
                        newState->items[0]=newItem;
                        newState->num=1;
                        tableLR1->ref[state_index][t]=newState;
                        tableLR1->states[tableLR1->num]=newState;
                        tableLR1->num++;
                    }
                    while(completeLR1State(newState)){

                    }
                }
                
            }
        }

        //Check if stateCore repeated
        if(flag){
            for(int k=0; k<tableLR1->num-1; k++){
                //printf("%d\n",tableLR1->num);
                if(stateCoreEquals(newState, tableLR1->states[k])){
                    for (int m=0; m<tableLR1->states[k]->num; m++){
                        for (int n=0; n<newState->num; n++){
                            if(itemCoreEquals(tableLR1->states[k]->items[m], newState->items[n])){
                                addSet(newState->items[n]->lookahead, tableLR1->states[k]->items[m]->lookahead);
                            }
                        }
                    }
                    free(newState);
                    tableLR1->ref[state_index][t]=tableLR1->states[k];
                    tableLR1->num-=1;
                    break;

                }
            }
        }
    }
}


//--------------------------------------------------
// 2024-1-8 fix bug.
// After merging states with the same core, New lookaheads
// symbols need to be added to reduce rules.
void makeLR1TableFix(int state_index){
    int p;
    int N;
    int flag;
    struct LR1Item* item;
    struct LR1State* newState;
    struct LR1Item* newItem;
    // init all to error state
    // This Part Deleted

    for (int t=0; t<symbols->numSymbols; t++){
        // Iterate througth symbols t
        flag=0;
        for (int i=0; i<tableLR1->states[state_index]->num; i++){
            
            p=tableLR1->states[state_index]->items[i]->position;
            N=tableLR1->states[state_index]->items[i]->rule->numRight;
            item=tableLR1->states[state_index]->items[i];
            // Reduce
            if(p==N){
                if(inSet(t, item->lookahead)){
                    tableLR1->actions[state_index][t]=1;
                    tableLR1->ref[state_index][t]=item;
                }
            }


        }


    }
}





//--------------------------------------------------

void makeLR1Table(){
    initLR1Table();
    for (int i=0; i<tableLR1->num; i++){
        makeLR1TableIter(i);
    }
    for (int i=0; i<tableLR1->num; i++){
        makeLR1TableFix(i);
    }
}

void printLR1Item(const struct LR1Item* item){
    printf("%s --> ", symbols->allSymbols[item->rule->left].symbol);
    for (int i=0; i<item->rule->numRight; i++){
        if(item->position==i)
            printf(".");
        printf("%s ", int2symbol(item->rule->right[i], symbols));
    }
    printf("\tLookahead: ");
    for (int i=0; i<item->lookahead->num; i++){
        printf("%s ", symbols->allSymbols[item->lookahead->elements[i]].symbol);
    }
    printf("\n");
}

void printLR1State(struct LR1State* state){
    for (int i=0; i<state->num; i++)
        printLR1Item(state->items[i]);
}

void printLR1States(){
    for (int i=0; i<tableLR1->num; i++){
        printf("State %d\n", i);
        printLR1State(tableLR1->states[i]);
        printf("\n");
    }
}

void init(){
    symbols=getSymbols2();
    g=readGrammar2();
    first_sets=(struct firstSets*)malloc(sizeof(struct firstSets));
    *first_sets=getFirstSets();
    
    r4NT= initAllLR1Items4NT();
    makeLR1Table();
}




#endif