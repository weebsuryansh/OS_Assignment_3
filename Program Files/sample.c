#include <stdio.h>

int fibonacci(int num){
    if(num==1||num==2){
        return 1;
    }
    else{
        return fibonacci(num-1)+fibonacci(num-2);
    }
}

void main(){
    printf("The %dth term in fibonacci is: %d\n", 45, fibonacci(45));
}