#ifndef IDENTITY_H
#define IDENTITY_H

#include "tuple.h"
// yield T when using member Type:
template<typename T>
struct IdentityT {
    using Type = T;
};


#endif

