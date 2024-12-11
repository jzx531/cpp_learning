#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include "hashTable.hpp"

class LZWcode{
    private:
        HashTable<int, std::string> decodeTable;
        HashTable<std::string, int> encodeTable;
        int divisor;
        int en_new_place; 
        int de_new_place;
    public:
        LZWcode(int divisor=10000);
        void printEnCodeTable();
        void printDeCodeTable();
        std::string encode(std::string input);
        std::string decode(std::string input);
        void encodeToFile(std::string infilename,std::string ofilename);
        void decodeToFile(std::string infilename,std::string ofilename);
};

LZWcode::LZWcode(int divisor)
{
    this->divisor = divisor;

    //构造基础码本
    this->encodeTable = HashTable<std::string, int> (this->divisor);
    //设置码本元素索引起始位置
    this->en_new_place = 256;
    for(int i = (int)'0'; i <= (int)'9'; i++)
    {
        this->encodeTable.insert(std::make_pair(std::string(1, i), i));
    }
    for (int i = (int)'a'; i <= (int)'z'; i++)
    {
        this->encodeTable.insert(std::make_pair(std::string(1, i), i));
    }    
    this->encodeTable.insert(std::make_pair(std::string(1, ','),(int)','));
    this->encodeTable.insert(std::make_pair(std::string(1, ' '),(int)' '));
    this->encodeTable.insert(std::make_pair(std::string(1, '.'),(int)'.'));

    //构造解码码本
    this->decodeTable = HashTable<int, std::string> (this->divisor);
    for (int i = (int)'a'; i <= (int)'z'; i++) {
        this->decodeTable.insert(std::make_pair(i, std::string(1, i)));
    }
    for (int i = (int)'0'; i <= (int)'9'; i++) {
        this->decodeTable.insert(std::make_pair(i, std::string(1, i)));
    }
    this->decodeTable.insert(std::make_pair((int)' ', std::string(1, ' ')));
    this->decodeTable.insert(std::make_pair((int)',', std::string(1, ',')));
    this->decodeTable.insert(std::make_pair((int)'.', std::string(1, '.')));
    // 设置新的码本元素索引起始位置
    this->de_new_place = 256;

}

void LZWcode::printDeCodeTable()
{
    for (int i = 0; i < this->divisor; i++) {
        if (decodeTable.find(i) == nullptr) {
            continue;
        }
        std::cout << i << " : " << decodeTable.find(i)->second << std::endl;
    }
}

void LZWcode::printEnCodeTable()
{
    for (int i = 0; i <255; i++) {
        if (encodeTable.find(std::string(1,i)) == nullptr) {
            continue;
        }
        std::cout << std::string(1,i) << " : " << encodeTable.find(std::string(1,i))->second << std::endl;
    }
}

std::string LZWcode::encode(std::string input)
{
    std::stringstream ss;
    int index = 0;
    std::string prev = "";
    std::string curr = "";
    for(index = 0; index < input.length(); index++)
    {
        curr = input.at(index);
        // std::cout << "curr: " << curr << std::endl;
        std::string temp = prev + curr;
        if(encodeTable.find(temp) != nullptr)
        {
            prev = temp;
            continue;
        }
        //每次编码为新的码本元素的重复码本前缀
        //编入新元素后将该码本元素的重复前缀清零后从末尾元素开始继续创建新码本元素
        else
        {
            // std::cout << "temp: " << temp << std::endl;
            // std::cout << "prev: " << prev << std::endl;
            encodeTable.insert(std::make_pair(temp, en_new_place++));
            ss << encodeTable.find(prev)->second<<" ";
            prev = curr;
            continue;
        }
    }
    ss << encodeTable.find(prev)->second;
    return ss.str();
}

std::string LZWcode::decode(std::string input)
{
    std::stringstream ss;
    std::stringstream in;
    int curr = 0;
    int prev = 0;
    std::string prev_c = "";
    std::string curr_c = "";
    bool start = false;
    for(int i = 0; i < input.length(); i++)
    {
        if(!start)
        {
            in << input.at(i);
            if(input.at(i)!=' '&&i<input.length()-1)
            {
                continue;
            }
            curr = atoi(in.str().c_str());
            in.str("");
            curr_c = decodeTable.find(curr)->second;
            ss << curr_c;

            start = true;
        }
        //step 4 
        //完成第一次循环后，prev_c和curr_c已经确定，可以开始解码
        //step 5
        //如果curr_c在码本中，则直接输出curr_c，并将prev_c和curr_c加入码本
        else 
        {
            prev = curr;
            in << input.at(i);
            if(input.at(i)!=' '&&i<input.length()-1)
            {
                continue;
            }
            curr = atoi(in.str().c_str());

            in.str("");
            if(decodeTable.find(curr) != nullptr)
            {

                curr_c = decodeTable.find(curr)->second;
                ss << curr_c;

                prev_c = decodeTable.find(prev)->second;
                decodeTable.insert(std::make_pair(de_new_place++, prev_c + curr_c.at(0)));
            }
            else
            {
                prev_c = decodeTable.find(prev)->second;
                curr_c = decodeTable.find(prev)->second.at(0);
                ss << (prev_c + curr_c);
                decodeTable.insert(std::make_pair(de_new_place++, prev_c + curr_c));
            }
        }
    }
    return ss.str();
}

void LZWcode::encodeToFile(std::string infilename,std::string ofilename)
{
    std::ifstream infile(infilename);
    if(!infile.is_open())
    {
        std::cout<<"Error: can not open file"<<std::endl;
        return;
    }
    std::ofstream outfile(ofilename);
    std::string line;
    while(getline(infile,line))
    {
        std::string encoded = encode(line);
        outfile<<encoded<<std::endl;
    }
    infile.close();
    outfile.close();
}

void LZWcode::decodeToFile(std::string infilename,std::string ofilename)
{
    std::ifstream infile(infilename);
    if(!infile.is_open())
    {
        std::cout<<"Error: can not open file"<<std::endl;
        return;
    }
    std::ofstream outfile(ofilename);
    std::string line;
    while(getline(infile,line))
    {
        std::string decoded = decode(line);
        outfile<<decoded<<std::endl;
    }
    infile.close();
    outfile.close();
}