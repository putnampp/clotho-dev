#ifndef GENOME_H_
#define GENOME_H_

#include "zygote.h"

/*
 *  A Genome is considered an indexed set of zygotes
 */
struct genome {
    virtual genome * clone() const = 0;

    virtual zygote::zygote_source_t getSource() const = 0;
    virtual zygote::zygote_source_t getMaxSources() const = 0;

    virtual zygote * getZygote( zygote::zygote_source_t ) const = 0;
    virtual void setZygote( zygote * ) = 0;

    virtual ~genome() {}
};

#endif  // GENOME_H_
