#ifndef IR_C
#define IR_C
#include "AST2.c"

struct CodeBlock{
    struct AST** decls;
    struct AST** asgns; //asgns and prints

    int declsN;
    int asgnsN;

    struct BlockData* bd;

};


struct ID_NAME{
    int a;
    char* name;
};

struct IDTable{
    struct ID_NAME** ids;
};

struct BlockData{
    int* INTs;
    float* FLOATs;
    struct IDTable* idTableINT;
    struct IDTable* idTableFLOAT;
};

struct DoubleInt{
    int a;
    int b;
};


struct DoubleInt getBothN(struct AST* ast){
    struct DoubleInt result;
    result.a=0;
    result.b=0;
    int flag;
    int t;
    while(1){
        t=ast->childs[0]->childs[0]->node->token;
        if(strcmp(int2symbol(t, symbols), "decl")==0){
            result.a++;
        }
        else{
            result.b++;
        }
        
        if(ast->numChild==1){
            return result;
        }
        ast=ast->childs[1];


    }
}

struct CodeBlock* initCodeBlock(struct AST* ast){
    struct DoubleInt AB=getBothN(ast);
    struct CodeBlock* code_block=malloc(sizeof(struct CodeBlock));
    code_block->declsN=AB.a;
    code_block->asgnsN=AB.b;

    code_block->decls=malloc(AB.a*sizeof(struct AST*));
    code_block->asgns=malloc(AB.b*sizeof(struct AST*));

    int X=0;
    int Y=0;

    int t;

    while(1){
        t=ast->childs[0]->childs[0]->node->token;
        if(strcmp(int2symbol(t, symbols), "decl")==0){
            code_block->decls[X]=ast->childs[0]->childs[0];
            X++;
        }
        else{
            code_block->asgns[Y]=ast->childs[0]->childs[0];
            Y++;
        }
        
        if(ast->numChild==1){
            if(!(X==code_block->declsN & Y==code_block->asgnsN))
                printf("ERROR initCodeBlock\n");
            return code_block;
        }
        ast=ast->childs[1];
    }
}




#endif