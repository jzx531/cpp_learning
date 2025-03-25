#ifndef COPYCOUNTER_H
#define COPYCOUNTER_H

template<int N>
struct CopyCounter{
    inline static unsigned numCopies = 0;
    CopyCounter(){}
    CopyCounter(const CopyCounter&){
        // std::cout << "Copy constructor called " << numCopies << " times." << std::endl;
        ++numCopies;
    }
};


#endif

