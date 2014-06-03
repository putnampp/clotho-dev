#ifndef RECOMBINATION_H_
#define RECOMBINATION_H_

#include "genome.h"

struct recombination {
    virtual genome * recombine( const genome * ) = 0;
};

#endif  // RECOMBINATION_H_
