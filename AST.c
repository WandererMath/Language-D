#ifndef AST_C
#define AST_C

#include "lexical.c"
#include "parsing.c"

#define MAX_STACK_SIZE 100000

int parsing_position=0;
struct Stack* stack;
struct TreeStack* treeStack;
struct AST* ast;


struct Stack{
    int num;
    int states[MAX_STACK_SIZE];
};


struct TreeStack{
    int num;
    struct AST* trees[MAX_STACK_SIZE];
    int position;
};

struct AST* initAST(struct Token* token){
    struct AST* ast=malloc(sizeof(struct AST));
    ast->numChild=0;
    ast->parent=NULL;
    ast->node=token;
    //ast->numExpand=1;
    return ast;
}

void push(int state){
    stack->states[stack->num]=state;
    stack->num++;
}

void pop(int N){
    stack->num-=N;
}

//for nonTerminals
struct Token* initToken(int a){
    struct Token* t;
    t->token=a;
    t->name=NULL;
    return t;
}

struct Token* getNextToken(){
    return t_code->tokens[parsing_position];
}

struct LR1State* currentState(){
    return tableLR1->states[stack->num-1];
}

int currentState_index(){
    return stack->states[stack->num-1];
}

// Need test
int getStateIndex(struct LR1State* state){
    for (int i=0; i<tableLR1->num; i++){
        if(tableLR1->states[i]==state)
            return i;
    }
}

void printStack(){
    for (int i=0; i<stack->num; i++)
        printf("%d ", stack->states[i]);
    printf("\n");
}

void makeASTIter(){
    struct Token* nextToken;
    struct AST* nextTree;
    if(treeStack->num==treeStack->position){
        nextToken=getNextToken();
        nextTree=initAST(nextToken);
    }
    else{
        nextTree=treeStack->trees[treeStack->position];
        nextToken=nextTree->node;
    } 
    int action=tableLR1->actions[currentState_index()][nextToken->token];
    struct LR1State* nextState;
    int nextState_index;
    struct LR1Item* reduceRule;
    struct AST* newTree;
    int newToken;
    int reduceNum;
    if (action==-1){
        printf("SYNTAX ERROR\n");
    }
    if (action==0){
        nextState=(struct LR1State*)tableLR1->ref[currentState_index()][nextToken->token];
        //printLR1State(nextState);
        nextState_index=getStateIndex(nextState);
        push(nextState_index);

        treeStack->trees[treeStack->num]=nextTree;
        if(treeStack->num==treeStack->position){
            treeStack->num++;
            parsing_position++;
        }
        treeStack->position++;
    }
    if(action==1){
        reduceRule=(struct LR1Item*)tableLR1->ref[currentState_index()][nextToken->token];
        newToken=reduceRule->rule->left;
        newTree=initAST(initToken(newToken));
        reduceNum=reduceRule->rule->numRight;
        treeStack->position-=reduceNum;
        treeStack->num-=(reduceNum-1);
        for (int i=0; i<reduceNum; i++){
            newTree->childs[i]=treeStack->trees[treeStack->position+i];
            treeStack->trees[treeStack->position+i]->parent=newTree;
        }
        newTree->numChild=reduceNum;
        treeStack->trees[treeStack->num-1]=newTree;
        pop(reduceNum);
    }
}



void initAST_C(){
    stack=malloc(sizeof(struct Stack));
    stack->num=1;
    stack->states[0]=0;
    treeStack=malloc(sizeof(struct TreeStack));
    treeStack->num=0;
    treeStack->position=0;
}

void printTreeStack(){
    for (int i=0; i<treeStack->num; i++){
        printToken(treeStack->trees[i]->node);
    }
    printf("\n");
}


void printASTIter(struct AST* ast, int level){
    for (int i=0; i<level; i++)
        printf("-");
    printToken(ast->node);
    for (int k=0; k<ast->numChild; k++)
        printASTIter(ast->childs[k], level+1);
}

void printAST(struct AST* ast){
    printASTIter(ast, 0);
}

void makeAST(){
    initAST_C();
    makeASTIter();
    while (treeStack->trees[0]->node->token!=1){
        makeASTIter();
        //printStack();
        //printTreeStack();
    }
    ast=treeStack->trees[0];
}



#endif