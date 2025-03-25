#ifndef HEAD_H
#define HEAD_H

#include <iostream>
#include <string>

void hello();

template<typename T>
void print(T t){
    std::cout << typeid(t).name() << " : " << t << std::endl;
}

template<>
inline void print<int>(int num){
    std::cout<<"this is an int: "<<num<<std::endl;
}

template<typename T>
void print_array(T arr[], int size){
    for(int i=0; i<size; i++){
        std::cout<<arr[i]<<" ";
    }
    std::cout<<std::endl;
}

void print_string(char *str,int size);


#endif
