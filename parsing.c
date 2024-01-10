#ifndef PARSING_C
#define PARSING_C

#include "parse3.c"
#define MAX_ITEMS_OF_STATE 100





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


struct LR1Item{
    const struct GrammarRule* rule;
    int position;
    struct intSet* lookahead;
};

struct LR1Item* initLR1Item(const struct GrammarRule* rule){
    struct LR1Item* item=malloc(sizeof(struct LR1Item));
    item->rule=rule;
    item->position=0;
    item->lookahead=initSet();
}


struct LR1State{
    struct LR1Item* items[MAX_ITEMS_OF_STATE];
    int num;
};


struct LR1State* initLR1State(){
    struct LR1State* state=malloc(sizeof(struct LR1State));
    state->num=0;
    return state;
}

struct LR1Table{
    // index: state, token
    // 1 reduce, 0 shift, -1 error
    int actions[MAX_STATE_NUMBER][MAX_STATE_NUMBER];
    struct LR1State* states[MAX_STATE_NUMBER];
    void* ref[MAX_STATE_NUMBER][MAX_STATE_NUMBER];
    int num; //number of states
};

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

struct AllLR1Items4NT{
    int num;
    // index for i-th token
    struct LR1State* itemsSets[MAX_TOKEN_NUMBER];
};

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