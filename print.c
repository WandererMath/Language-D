#include <stdio.h>
#include <windows.h>
void print(int a){
    printf("%d\n", a);
}

int input(){
    int a;
    scanf("%d", &a);
    return a;
}

void end(){
    ExitProcess(0);
}