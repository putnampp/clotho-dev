#ifndef GENOME_H_
#define GENOME_H_

#include "zygote.h"

struct genome {
    virtual genome * clone() const;
    zygote * mother, * father;
};

#endif  // GENOME_H_
