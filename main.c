#include "CodeGen.c"


int main(int argc, char** argvs){
    // Make LALR table
    init();

    // Build AST
    //initLex(argvs[1]);
    initLex("ex1.d");
    makeAST();
    printAST(ast);
    
    // Code Generation
    initCodeGen("test.asm");
    Program();
    endCodeGen();
}