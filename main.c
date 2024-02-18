#include "CodeGen.c"

#define MAX_FILE_NAME_LENGTH 100
char* output_filename(char* name){
    char* file=malloc(MAX_FILE_NAME_LENGTH);
    int i=0;
    while(name[i]!='d'){
        file[i]=name[i];
        i++;
    }
    file[i]='a';
    file[i+1]='s';
    file[i+2]='m';
    return file;
}

int main(int argc, char** argvs){
    // Make LALR table
    init();

    // Build AST
    initLex(argvs[1]);
    makeAST();
    printAST(ast);
    
    // Code Generation
    initCodeGen(output_filename(argvs[1]));
    Program();
    endCodeGen();
}