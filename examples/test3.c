typedef int (*FuncPtr)(int, int);

int addNum(int a, int b) {
    if(b == 0)
        return a;
    return addNum(a, b-1);
}

int mulNum(int a, int b) {
    return a * b;
}

FuncPtr getFunc(int op) {
    return op == 1 ? &addNum :
           op == 2 ? &mulNum :
           (FuncPtr)0;
}

int main() {
    int ret = getFunc( 1+0   )(  5 , 6   );
    return 0;
}
