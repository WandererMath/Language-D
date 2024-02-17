#ifndef CODEGEN_C
#define CODEGEN_C

#include "AST.c"

#define WIN32 1

FILE* f;

struct idList{
    int id; // Starting from 0
    char* name;
    struct idList* next;
};

struct idList* initIDList(char* name){
    struct idList* myTable=malloc(sizeof(struct idList));
    myTable->next=NULL;
    myTable->name=name;
    myTable->id=0;
    return myTable;
}

void appendID(struct idList* list,char* name){
    struct idList* next=initIDList(name);
    while(list->next!=NULL)
        list=list->next;
    next->id=list->id+1;
    list->next=next;
}

void printIDList(struct idList* list){
    while(list!=NULL){
        printf("%d\t%s\n", list->id, list->name);
        list=list->next;
    }
}

int id2int(struct idList* list, char* id){
    while(list!=NULL){
        if(strcmp(list->name, id)==0)
            return list->id;
        list=list->next;
    }
    return -1;
}

// for paras and local vars
int var2int(char* name, struct idList* IDs, struct idList* paras){
    int X;
    X=id2int(paras, name)+1;
    if(X!=-1)
        return X*4+4;
    X=id2int(IDs, name)+1;
    if(X!=-1)
        return -X*4;
    printf("Variable Name %s NOT DEFINED\n", name);
    return -1;
}

int getN_IDLIST(struct idList* list){
    int c=0;
    while(list!=NULL){
        c++;
        list=list->next;
    }
    return c;
}

void initCodeGen(char* fileName){
    f=fopen(fileName, "w");
    fprintf(f, "extern _print, _end\nsection .text\n\tglobal _WinMain@16\n");
}

void endCodeGen(){
    fprintf(f, "\tCALL _end\n");
    fclose(f);
}

int string2int(char* name){
    for (int i=0; i<symbols->numSymbols; i++){
        if(strcmp(name, symbols->allSymbols[i].symbol)==0)
            return i;
    }
}

void evaluate(struct AST* E, struct idList* IDs, int r/*0 EAX, 1 EBX*/){
    int literal=string2int("literal");
    int identifier=string2int("id");
    int mul=string2int("*");
    int plus=string2int("+");
    int leftp=string2int("(");
    int var;
    int op;

    //Preserving Data
    if(r==0)
        fprintf(f, "\tPUSH EBX\n");
    else
        fprintf(f, "\tPUSH EAX\n");
    
    //Case for (E)
    if(E->childs[0]->node->token==leftp){
        E=E->childs[1];
    }

    //Directly getting value
    if (E->numChild==1){
        if(E->childs[0]->node->token==literal){
            //printf("%s\n",E->childs[0]->node->name);
            if(r==0)
                fprintf(f, "\tMOV EAX, %s\n", E->childs[0]->node->name);
            else
                fprintf(f, "\tMOV EBX, %s\n", E->childs[0]->node->name);
        }
        if(E->childs[0]->node->token==identifier){
            var=id2int(IDs, E->childs[0]->node->name);
            var=var*4+4;
            if(r==0)
                fprintf(f, "\tMOV EAX, DWORD [ECX+%d]\n", var);
            else 
                fprintf(f, "\tMOV EBX, DWORD [ECX+%d]\n", var);
        }    
    }

    //Binary Operator Evaluation
    else{

        op=E->childs[1]->node->token;
        evaluate(E->childs[0], IDs, 0);
        evaluate(E->childs[2], IDs, 1);
        if(op==plus){
            if(r==0)
                fprintf(f,"\tADD EAX, EBX\n");
            else 
                fprintf(f,"\tADD EBX, EAX\n");
        }
        if(op==mul){
            if(r==0)
                fprintf(f,"\tIMUL EAX, EBX\n");
            else 
                fprintf(f,"\tIMUL EBX, EAX\n");
        }
    }

    //Restoring Data
    if(r==0)
        fprintf(f, "\tPOP EBX\n");
    else
        fprintf(f, "\tPOP EAX\n");
}



void asg(struct AST* assign, struct idList* IDs, struct idList* paras){
    int literal=string2int("literal");
    int identifier=string2int("id");
    int id=id2int(IDs,assign->childs[0]->node->name);
    //Getting Space for Local Variables A, B
    
    fprintf(f,"\tMOV ECX, ESP\n");
    evaluate(assign->childs[2], IDs, 0);
    fprintf(f,"\tMOV DWORD [ESP+%d], EAX\n", 4*id+4);
}

struct idList* decl(struct AST* decl, struct idList* IDs){
    int INT=string2int("int");
    //printIDList(IDs);
    char* id=decl->childs[1]->node->name;
    if(IDs==NULL)
        IDs=initIDList(id);
    else
        appendID(IDs, id);
    
    return IDs;
}

void funCall(struct AST* call, struct idList* IDs, struct idList* paras){
    // FILO
    // PUSH paras
    // PUSH EBP
    // CALL
    // POP EBP
    // POP paras

    /*

        Var2                <-[ESP]
        Var1                <-[EBP-4]
        Return Address      <-[EBP now]
        EBP                 <-
        Para1               <-[EBP+8]
        Para2
        Para3
    */
    int ids=string2int("ids");
    int id=string2int("id");

    // Push paras
    char* name;
    int d;
    int count=0;//Number of paras
    struct AST* args=call->childs[2];
    // Now call = ids
    if(args->node->token==ids){
    // The function has parameter(s)
        while(args->numChild==3)
        {
            count++;
            name=args->childs[2]->node->name;
            d=var2int(name, IDs, paras);
            if(d>0)
                fprintf(f, "\tPUSH DWORD [EBP+%d]\n", d);
            else
                fprintf(f, "\tPUSH DWORD [EBP%d]\n", d);
            args=args->childs[0];
        }
        count++;
        name=args->childs[0]->node->name;
        d=var2int(name, IDs, paras);
        if(d>0)
            fprintf(f, "\tPUSH DWORD [EBP+%d]\n", d);
        else
            fprintf(f, "\tPUSH DWORD [EBP%d]\n", d);
    }

    // CALL
    char* funcName=call->childs[0]->node->name;
    fprintf(f, "\tCALL _%s\n", funcName);

    // Push EBP
    fprintf(f, "\tPUSH EBP\n");

    // Pop EBP
    fprintf(f, "\tPOP EBP\n");

    // Pop paras
    fprintf(f, "\tADD ESP, %d\n", count*4);
}



struct idList* code(struct AST* code, struct idList* IDs, struct idList* paras){
/*
    printAST(code);
    printf("\n");
*/
    int ASG=string2int("assign");
    int DECL=string2int("decl");
    int CALL=string2int("funCall");
    
    code=code->childs[0];
    int what=code->node->token;

    if(what==ASG){
        asg(code, IDs, paras);
    }
    if(what==CALL){
        funCall(code, IDs, paras);
    }
    if(what==DECL){
        IDs=decl(code, IDs);
    }
    return IDs;
}


struct idList* para(struct AST* func){
    if(func->numChild==5)
        return NULL;
    struct AST* ps=func->childs[3];
    struct idList* result=NULL;
    while(ps->numChild==3){
        if(result==NULL)
            result=initIDList(ps->childs[0]->childs[1]->node->name);
        else
            appendID(result, ps->childs[0]->childs[1]->node->name);
        ps=ps->childs[2];
    }
    if(result==NULL)
        result=initIDList(ps->childs[0]->childs[1]->node->name);
    else
    //printf("%s\n", ps->childs[0]->childs[1]->node->name);
        appendID(result, ps->childs[0]->childs[1]->node->name);
    return result;
}

void code_block(struct AST* block, struct idList* IDs, struct idList* paras){
    IDs=code(block->childs[0], IDs, paras);
    if(block->numChild==2){
        code_block(block->childs[1], IDs, paras);
        return;
    }
    //printIDList(paras);
    //printIDList(IDs);
}




void func(struct AST* func){
    char* name=func->childs[1]->node->name;
    int flag=0;
    if(strcmp(name, "main")==0){
        flag=1;
        if(WIN32)
            fprintf(f, "_WinMain@16:\n");
        else
            fprintf(f, "_%s:\n", name);
    }
    else{
        fprintf(f, "_%s:\n", name);
    }
    int N=func->numChild;
    struct idList* paras=para(func);

    /*
        MOV EBP, ESP
        SUB ESP, ...
        {CodeBlock}
        ADD ESP, ...
        RET
    */
    int N1=getN_IDLIST(paras)*4;
    fprintf(f,"\tMOV EBP, ESP\n");
    fprintf(f,"\tSUB ESP, %d\n", N1);
    
    code_block(func->childs[N-1]->childs[1], NULL, paras);
    fprintf(f,"\tADD ESP, %d\n", N1);
    if(!flag)
        fprintf(f,"\tRET\n");
}

void functions(struct AST* funcs){
    int N;
    N=funcs->numChild;
    if(N==1){
        func(funcs->childs[0]);
    }
    else{
        func(funcs->childs[0]);
        functions(funcs->childs[1]);
    }
}

void Program(){
    functions(ast->childs[0]);
}

#endif