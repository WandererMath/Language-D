#ifndef LEXICAL_C
#define LEXICAL_C

#include<stdio.h>
#include<stdlib.h>
#include <string.h>

#include "parsing.c"

#define MAX_SOURCE_TOKENS 100000
#define MAX_ID_NAME_LENGTH 100

/*
MUST
$   1
P   0
id  1
literal 1
*/

char single_tokens[]={'+', '*', '(', ')', ';', '=', '{', '}', ','};
struct TokenizedCode* t_code;

struct Token{
    int token;
    char *name;
};

struct AST{
    struct Token* node;
    struct AST* parent;

    int numChild;
    struct AST* childs[MAX_TREE_CHILD];

    //int numExpand; //not used

};

struct TokenizedCode{
    int num;
    struct Token* tokens[MAX_SOURCE_TOKENS];
};


char* readFile(char* fileName){
    FILE* f;
    int length;
    char* string;
    f=fopen(fileName, "r");
    fseek(f, 0, SEEK_END);
    length=ftell(f);
    fseek(f, 0, SEEK_SET);
    string=malloc(sizeof(char)*(length+1));
    int i=0;
    char c;
    while ((c=fgetc(f))!=EOF){
        string[i]=c;
        i++;
    }
    string[i]='\0';
    fclose(f);
    return string;
}

char* substring(char* string, int start, int length){
    char* s=malloc(sizeof(char)*(length+1));
    for (int i=0; i<length; i++){
        s[i]=string[i+start];
    }
    s[length]='\0';
    return s;

}


int isName(char c){
    if ('A'<=c && c<='Z'){
        return 1;
    }
    if ('a'<=c && c<='z'){
        return 1;
    }
    if ('0'<=c && c<='9'){
        return 1;
    }
    return 0;
}

int isNumber(char c){
    if ('0'<=c && c<='9'){
        return 1;
    }
    return 0;
}


struct Token* string2token(char* s, const struct AllSymbols* symbols){
    struct Token* token=malloc(sizeof(struct Token));
    token->name=NULL;
    token->token=-1;
    for (int i=0; i<symbols->numSymbols; i++){
        if(strcmp(symbols->allSymbols[i].symbol, s)==0){
            token->token=i;
            return token;
        }
    }

    

    //id
    token->token=2;
    token->name=s;

    //literal
    if(isNumber(token->name[0])){
        token->token=3;
    }

    return token;
}

int isSingleToken(char c){
    for (int i=0; i<sizeof(single_tokens)/sizeof(single_tokens[0]); i++){
        if(c==single_tokens[i]){
            return 1;
        }
    }
    return 0;
}




void printToken(const struct Token* t){
    if (t->token==2){
        printf("IDENTIFIER %s\t\n", t->name);
        return;
    }
    if(t->token==3){
        printf("LITERAL %s\t\n", t->name);
        return;
    }
    else{
        printf("%s\t\n",symbols->allSymbols[t->token].symbol);
    }
}


void printTokens(struct TokenizedCode* t_code){
    for(int i=0; i<t_code->num; i++){
        printToken(t_code->tokens[i]);
        printf("\n");
    }
    
}


struct TokenizedCode* tokenize(char* source, const struct AllSymbols* symbols){
    int ptr1, ptr2;
    ptr1=0;
    ptr2=0;
    char* tmps;
    struct TokenizedCode* result=malloc(sizeof(struct TokenizedCode));
    result->num=0;
    while(source[ptr2]!='\0'){
        // Move ptr1, ptr2 to first non-space
        while (!(isName(source[ptr1])||isSingleToken(source[ptr1]))){
            ptr1++;
        }
        ptr2=ptr1;

        if(isSingleToken(source[ptr1])){
            tmps=substring(source, ptr1,1);
            result->tokens[result->num]=string2token(tmps, symbols);
            result->num+=1;
            ptr1+=1;
            ptr2+=1;
            continue;
        }

        while(isName(source[ptr2])){
            ptr2++;
        }

        tmps=substring(source, ptr1, ptr2-ptr1);
        result->tokens[result->num]=string2token(tmps, symbols);
        result->num+=1;
        ptr1=ptr2;

    }
    tmps="$";
    result->tokens[result->num]=string2token(tmps, symbols);
    result->num+=1;
    return result;
}

struct TokenizedCode* getTCodeFromFile(char* filename, const struct AllSymbols* symbols){
    
    char* source=readFile(filename);
    struct TokenizedCode* t_code=tokenize(source, symbols);
}

void initLex(char* filename){
    t_code=getTCodeFromFile(filename, symbols);
}


#endif