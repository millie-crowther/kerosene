#include <stdio.h>

#include "json.h"

int main(){
    if (!json_compile_regular_expressions()){
        printf("Failed to compile regular expressions\n");
        return -1;
    }
    printf("hello world!");
}
