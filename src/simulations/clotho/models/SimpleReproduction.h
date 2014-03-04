#ifndef SIMPLE_REPRODUCTION_H_
#define SIMPLE_REPRODUCTION_H_

#include "../clotho.h"
#include "reproduction.h"

/*
 * Round-robin style of zygote cloning 
 * of the supplied genome
 *
 */
class SimpleReproduction : public reproduction {
public:
    SimpleReproduction();

    zygote * reproduce( genome * g );

    virtual ~SimpleReproduction();
protected:
    zygote::zygote_source_t m_source;
};

#endif  // SIMPLE_REPRODUCTION_H_
