#ifndef REPRODUCTION_H_
#define REPRODUCTION_H_

#include "genome.h"

struct reproduction {
    virtual zygote * reproduce( genome * g ) = 0;
    virtual ~reproduction() {}
};

#endif  // REPRODUCTION_H_
