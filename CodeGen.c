#ifndef CODEGEN_C
#define CODEGEN_C

#include "AST.c"

#define WIN32 1
#define ACCESS ->node->token

FILE* f;
int L_count=0;

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

int get_N(struct idList* ids){
    int r=0;
    while(ids!=NULL){
        r++;
        ids=ids->next;
    }
    return r;
}

void funCall(struct AST *call, struct idList *IDs, struct idList *paras);

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
    if(X!=0)
        return X*4;
    X=id2int(IDs, name)+1;
    if(X!=0)
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
    fprintf(f, "extern _print, _input, _end\nsection .text\n\tglobal _WinMain@16\n");
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

char* var2mem(char* name, struct idList* IDs, struct idList* paras){
    char* buffer=malloc(100);
    int d=var2int(name, IDs, paras);
    if(d==-1){
        return name;
    }
    if (d>0)
        sprintf(buffer, "DWORD [EBP+%d]", d);
    else{
        sprintf(buffer, "DWORD [EBP%d]", d);
    }
    printf("%s\n", buffer);
    return buffer;
}


void funCall(struct AST* call, struct idList* IDs, struct idList* paras){
    // FILO
    // PUSH EBP
    // PUSH paras
    
    // CALL
    // POP paras
    // POP EBP
    

    /*

        Var2                <-[ESP]
        Var1                <-[EBP-4]
        Return Address      <-[EBP now]
        Para1               <-[EBP+4]
        Para2
        Para3
        EBP
    */
    int ids=string2int("ids");
    int id=string2int("id");

    // Push EBP
    fprintf(f, "\tPUSH EBP\n");

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


    // Pop paras
    fprintf(f, "\tADD ESP, %d\n", count*4);

    // Pop EBP
    fprintf(f, "\tPOP EBP\n");
}



void evaluate(struct AST* E, struct idList* IDs,struct idList* paras, int r/*0 EAX, 1 EBX*/){
    int literal=string2int("literal");
    int identifier=string2int("id");
    int mul=string2int("*");
    int plus=string2int("+");
    int sub=string2int("-");
    int leftp=string2int("(");
    int functionCall=string2int("funCall");
    int var;
    int op;
    int flag;
    char* regs[]={"EAX", "EBX"};
    char* regs_r[]={"EBX", "EAX"};

    /*
        EBP constant all the time
        --Preserving EAX or EBX
    */


    //Preserving Data
    if(r==0)
        fprintf(f, "\tPUSH EBX\n");
    else
        fprintf(f, "\tPUSH EAX\n");
L1:
    flag=1; 
    // funCall
    if(E->node->token==functionCall){
        //Call function
        if(r==1)
            fprintf(f,"\tMOV ECX, EAX\n");

        funCall(E, IDs, paras);
        fprintf(f, "\tMOV %s, EAX\n", regs[r]);

        if(r==1)
            fprintf(f,"\tMOV EAX, ECX\n");
        
        
        goto L2;
    }

    //Case for (E)
    if(E->childs[0]->node->token==leftp){
        E=E->childs[1];
    }

    //Directly getting value
    if (E->numChild==1){
        if(E->childs[0]->node->token==literal){
            //printf("%s\n",E->childs[0]->node->name);
            fprintf(f, "\tMOV %s, %s\n", regs[r],E->childs[0]->node->name);
            flag=0;
        }
        if(E->childs[0]->node->token==identifier){
            var=var2int(E->childs[0]->node->name, IDs, paras);
            flag=0;
            if(var>0)
                fprintf(f, "\tMOV %s, DWORD [EBP+%d]\n",regs[r], var);
            else 
                fprintf(f, "\tMOV %s, DWORD [EBP%d]\n",regs[r], var);
        }
        if(flag){
            E=E->childs[0];
            goto L1;
        }    
    }

    //Binary Operator Evaluation
    else{

        op=E->childs[1]->node->token;
        evaluate(E->childs[0], IDs,paras, 0);
        evaluate(E->childs[2], IDs,paras, 1);
        if(op==plus){
            if(r==0)
                fprintf(f,"\tADD EAX, EBX\n");
            else 
                fprintf(f,"\tADD EBX, EAX\n");
        }
        if(op==sub){
            if(r==0)
                fprintf(f,"\tSUB EAX, EBX\n");
            else 
                fprintf(f,"\tSUB EBX, EAX\n");
        }
        if(op==mul){
            if(r==0)
                fprintf(f,"\tIMUL EAX, EBX\n");
            else 
                fprintf(f,"\tIMUL EBX, EAX\n");
        }
    }
L2:
    //Restoring Data
    if(r==0)
        fprintf(f, "\tPOP EBX\n");
    else
        fprintf(f, "\tPOP EAX\n");
}



void asg(struct AST* assign, struct idList* IDs, struct idList* paras){
    int literal=string2int("literal");
    int identifier=string2int("id");
    int var=var2int(assign->childs[0]->node->name, IDs, paras);

    /*
        {Evaluation Code}
        Save result in EAX
        MOV [VAR], EBX 
    */
    
    evaluate(assign->childs[2], IDs,paras, 0);
    if(var>0)
        fprintf(f,"\tMOV DWORD [EBP+%d], EAX\n", var);
    else
        fprintf(f,"\tMOV DWORD [EBP%d], EAX\n", var);
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


int condition(struct AST* cdt, struct idList* IDs, struct idList* paras){
    //printAST(cdt);
    int LESST=string2int("<");
    int EQ=string2int("=");
    char* var1=var2mem(cdt->childs[0]->node->name, IDs, paras);
    char* var2=var2mem(cdt->childs[2]->node->name, IDs, paras);
    struct AST* cmp=cdt->childs[1];
    fprintf(f, "L%d:\n", L_count);
    fprintf(f, "\tMOV EAX, %s\n", var1);
    fprintf(f, "\tCMP EAX, %s\n", var2);
    if(cmp->childs[0]ACCESS==LESST){
        //jmp to end if false
        fprintf(f, "\tJGE L%d\n", L_count+3);
        return ;
    }
    if(cmp->childs[0]ACCESS==EQ){
        //jmp to end if false
        fprintf(f, "\tJG L%d\n", L_count+3);
        fprintf(f, "\tJL L%d\n", L_count+3);
        return ;
    }
    
}

void control(struct AST* crl, struct idList* IDs, struct idList* paras){
    int T_IF=string2int("if");
    int T_WHILE=string2int("while");
    condition(crl->childs[2], IDs, paras);
    /*
    L_count:
        cmp
    Above implemented in condition function
    --------------------
    L_count+1:(jmp for true)
    L_count+2:(jmp for false)
    L_count+3:(end)
    */
    if(crl->childs[0]ACCESS==T_IF){
        fprintf(f, "L%d:\n", L_count+1);
        code_block(crl->childs[5], IDs, paras);
        fprintf(f, "L%d:\n", L_count+2);
    }
    if(crl->childs[0]ACCESS==T_WHILE){
        //true
        fprintf(f, "L%d:\n", L_count+1);
        code_block(crl->childs[5], IDs, paras);
        fprintf(f, "JMP L%d\n", L_count);
        //false
        fprintf(f, "L%d:\n", L_count+2);
    }
    fprintf(f, "L%d:\n", L_count+3);
    L_count+=4;
}

void ret(struct AST* re, struct idList* IDs, struct idList* paras){
    //To be implemented
    //printAST(re);
    evaluate(re->childs[1], IDs, paras, 0);
    fprintf(f,"\tADD ESP, %d\n", get_N(IDs)*4);
    fprintf(f,"\tRET\n");
    
}

struct idList* code(struct AST* code, struct idList* IDs, struct idList* paras){
/*
    printAST(code);
    printf("\n");
*/
    int ASG=string2int("assign");
    int DECL=string2int("decl");
    int CALL=string2int("funCall");
    int CONTROL=string2int("control");
    int RET=string2int("ret");
    
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
    if(what==CONTROL){
        control(code, IDs, paras);
    }
    if(what==RET){
        ret(code, IDs, paras);
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

int code_block_get_N_local_vars(struct AST* block){
    int c=0;
    struct AST* code;
    int decl=string2int("decl");
    while(1){
        code=block->childs[0];
        if(code->childs[0]->node->token==decl)
            c++;
        if(block->numChild==1)
            break;
        block=block->childs[1];
    }
    return c;
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

    int N1=code_block_get_N_local_vars(func->childs[N-1]->childs[1])*4;
    fprintf(f,"\tMOV EBP, ESP\n");

    //Creating Local Vars
    fprintf(f,"\tSUB ESP, %d\n", N1);
    code_block(func->childs[N-1]->childs[1], NULL, paras);
    //Destroying Local Vars
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