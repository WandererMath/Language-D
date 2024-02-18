int main(){
    int a;
    int b;
    int c;
    int N;
    N=1;
    a=1;
    b=1;
    while(N<20){
        c=a+b;
        a=b;
        b=c;
        print(c);
        N=N+1;
    }

}