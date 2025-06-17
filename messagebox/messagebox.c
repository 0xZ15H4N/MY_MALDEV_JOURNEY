#include<windows.h>
#include<stdio.h>
int main(void){

    int res = MessageBoxA
    (
        NULL,
        "MY First MessageBox!",
        "hello world",
        MB_YESNOCANCEL | MB_ICONERROR
    );

    if(res ==1 ){
        printf("You pressed Ok!\n");
    }else{
        printf("%d ",res);
    }
    return EXIT_SUCCESS;

}