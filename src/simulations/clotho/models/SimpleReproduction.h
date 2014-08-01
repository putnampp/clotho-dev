#ifndef SIMPLE_REPRODUCTION_H_
#define SIMPLE_REPRODUCTION_H_

#include "../clotho.h"
#include "reproduction.h"

#include "rng/rng.hpp"

/*
 * Round-robin style of gamete cloning
 * of the supplied genome
 *
 */
class SimpleReproduction : public reproduction {
public:
    SimpleReproduction( shared_ptr< iRNG > rng );

    gamete * reproduce( genome * g );

    virtual ~SimpleReproduction();
protected:
    shared_ptr< iRNG > m_rng;
};

#endif  // SIMPLE_REPRODUCTION_H_
