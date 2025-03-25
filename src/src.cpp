#include "head.h"

template<>
void print<const char*>(const char* str){
    std::cout<<"this is a const char* string: "<<str<<std::endl;
}

void hello() {
    print("Hello, world!");
}

void print_string(char *str,int size){
    print_array(str,size);
}
