#include "IR.c"

void myStrcpy(char* a, char* b){
    int i=0;
    while(1){
        a[i]=b[i];
        if(b[i]=='\0')
            break;
        i++;
    }
}




struct DoubleInt getTypesN(struct CodeBlock* cb){
    struct DoubleInt result;
    result.a=0;
    result.b=0;
    int t;
    for(int i=0; i<cb->declsN; i++){
        t=cb->decls[i]->childs[0]->childs[0]->node->token;
        if(strcmp(int2symbol(t, symbols), "int")==0){
            result.a++;
        }
        else{
            result.b++;
        }
    }
    return result;
}

//cb->bd will be init.d
void initBlockData(struct CodeBlock* cb){
    struct BlockData* bd=malloc(sizeof(struct BlockData));
    struct DoubleInt tmp=getTypesN(cb);
    
    bd->INTs=malloc(tmp.a*sizeof(int));
    bd->FLOATs=malloc(tmp.b*sizeof(float));


    struct IDTable* tableInt=malloc(sizeof(struct IDTable));
    struct IDTable* tableFloat=malloc(sizeof(struct IDTable));
    tableInt->ids=malloc(tmp.a*sizeof(struct ID_NAME*));
    tableFloat->ids=malloc(tmp.b*sizeof(struct ID_NAME*));
    int x=0;
    int y=0;
    int token;
    char* key;
    for(int i=0; i<cb->declsN; i++){
        token=cb->decls[i]->childs[0]->childs[0]->node->token;
        key=int2symbol(token, symbols);
        if(strcmp( key, "int")==0){
            tableInt->ids[x]=malloc(sizeof(struct ID_NAME));
            tableInt->ids[x]->a=x;
            tableInt->ids[x]->name=malloc(MAX_ID_NAME_LENGTH*sizeof(char));
            strcpy(tableInt->ids[x]->name,cb->decls[i]->childs[1]->node->name);
            x++;
        }
        if(strcmp(key, "float")==0){
            tableFloat->ids[y]=malloc(sizeof(struct ID_NAME));
            tableFloat->ids[y]->a=y;
            tableFloat->ids[y]->name=malloc(MAX_ID_NAME_LENGTH*sizeof(char));
            strcpy(tableFloat->ids[y]->name,cb->decls[i]->childs[1]->node->name);
            y++;
        }  
    }
    bd->idTableFLOAT=tableFloat;
    bd->idTableINT=tableInt;
    //printf("%d %d\n", x, y);
    //printf("%s\n", bd->idTableINT->ids[1]->name);
    
    cb->bd=bd;
}


int id2Index(char* id, struct BlockData* bd){
    int i=0;
    while(1){
        if(strcmp(bd->idTableINT->ids[i]->name, id)==0)
            return i;
        i++;
    }
}

// Level 3
int evaluateInt(struct AST* ast, struct BlockData* bd){
    int E;
    int t_op;
    char* t_char;
    t_op=ast->childs[0]->node->token;
    t_char=int2symbol(t_op, symbols);
    int index;
    if(t_op==3){
        return atoi(ast->childs[0]->node->name);
    }

    if(t_op==2){
        index=id2Index(ast->childs[0]->node->name, bd);
        return bd->INTs[index];
    }

    if(strcmp(t_char, "(")==0){
        return evaluateInt(ast->childs[1], bd);
    }

    t_op=ast->childs[1]->node->token;
    t_char=int2symbol(t_op, symbols);
    if(strcmp(t_char, "+")==0){
        E=evaluateInt(ast->childs[0], bd)+evaluateInt(ast->childs[2], bd);
    }

    if(strcmp(t_char, "*")==0){
        E=evaluateInt(ast->childs[0], bd)*evaluateInt(ast->childs[2], bd);
    }

    return E;
}

// Code Level - Level 2
void executeCode(struct AST* ast, struct BlockData* bd){
    int t_op;
    char* t_char;
    int index;
    t_op=ast->node->token;
    t_char=int2symbol(t_op, symbols);

    if(strcmp(t_char, "assign")==0){
        index=id2Index(ast->childs[0]->node->name, bd);
        bd->INTs[index]=evaluateInt(ast->childs[2], bd);
        return ;
    }

    if(strcmp(t_char, "internalCMD")==0){
        index=id2Index(ast->childs[1]->node->name, bd);
        printf("%d\n", bd->INTs[index]);
        return ;
    }
}


// CodeBlock Level - Level 1
void executeCodeBlock(struct CodeBlock* cb){
    for (int i=0; i<cb->asgnsN; i++){
        executeCode(cb->asgns[i], cb->bd);
    }
}




int main(int argc, char** argvs){
    init();
    initLex(argvs[1]);
    makeAST();
    struct AST* ast=treeStack->trees[0];
    struct CodeBlock* cb= initCodeBlock(ast->childs[0]->childs[1]);
    initBlockData(cb);

    executeCodeBlock(cb);
}