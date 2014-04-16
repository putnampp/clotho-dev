#ifndef GENOME_H_
#define GENOME_H_

#include "gamete.h"

/*
 *  A Genome is considered an indexed set of gametes
 */
struct genome {
    virtual genome * clone() const = 0;

    virtual gamete::gamete_source_t getSource() const = 0;
    virtual gamete::gamete_source_t getMaxSources() const = 0;

    virtual gamete * getGamete( gamete::gamete_source_t ) const = 0;
    virtual void setGamete( gamete * ) = 0;

    virtual ~genome() {}
};

#endif  // GENOME_H_
