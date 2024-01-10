#include "parse2.c"


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
