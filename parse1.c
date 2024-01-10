#include "parseDecl.c"

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