int play(int x, int y){
    int t;
    t=(y+4*7*y)*(x*y+4*(x*y+9));
    print(t);
}

int main(){
    int brenda;
    int lucy;
    brenda=5;
    lucy=17;
    play ( brenda , lucy);
    if(lucy<7){
        print(lucy);
    }
    if(brenda<7){
        brenda=brenda+1;
        print(brenda);
    }
    while(brenda<lucy){
        brenda=brenda+1;
        print(brenda);
    }
    if(2<lucy){
        print(lucy);
    }
}