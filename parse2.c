#include "parse1.c"



struct Identifier{
    void* father;
    int type;
    // number -1 = index FOR ALL.
    // int #16
    // float #17

    char name[100];
    void* value;
};



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


