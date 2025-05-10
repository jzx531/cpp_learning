#include <iostream>
#include "LZWcode.hpp"
#include <cstring>

int main(int argc, char const *argv[]) {
    LZWcode lzw;
    std::cout<<argc<<std::endl;
    // std::cout<<argv[1]<<std::endl;
    // std::cout<<argv[2]<<std::endl;
    // std::cout<<argv[3]<<std::endl;
    if(argc ==4){
        if(!strcmp(argv[1],"-c")) {
            std::cout << "Usage: LZWcode -c input_file output_file" << std::endl;
            lzw.encodeToFile(std::string(argv[2]), std::string(argv[3]));
            
        }
        if(!strcmp(argv[1],"-d") ){
            std::cout << "Usage: LZWcode -d input_file output_file" << std::endl;
            lzw.decodeToFile(std::string(argv[2]), std::string(argv[3]));
            
        } 
    }
    else{
        std::cout<<"false"<<std::endl;
    }
    return 0;
}