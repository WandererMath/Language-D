int fac(int n){
    int t;
    if(n<2){
        return 1;
    }
    t=n-1;
    return fac(t)*n;
}
int main(){
    int k;
    int x;
    x=input();
    k=fac(x);
    print(k);
}