#ifndef GENERAL_RNG_H_
#define GENERAL_RNG_H_

#include "irng.h"

#include <cassert>

template < class T >
class RNG : virtual public iRNG {
public:
    RNG( ) : m_rng(), m_type(""), m_seed(0) { initialize(); }
    RNG( seed_t s ) : m_rng(s), m_type(""), m_seed(s) { initialize(); }
    RNG( const type_t & type, seed_t s ) : m_rng(s), m_type(type), m_seed(s) { initialize(); }

    const type_t & getType() const {
        return m_type;
    }

    seed_t  getSeed() const {
        return m_seed;
    }

    void    reseed( seed_t s ) {
        m_seed = s;
    }

    unsigned int    getMin() const {
        return 0;
    }

    unsigned int    getMax() const {
        return (unsigned int)-1;
    }

    unsigned int    nextInt() {
        assert(false);
    }

    unsigned int    nextInt( unsigned int ) {
        assert(false);
    }

    bool            nextBool() {
        assert(false);
    }

    double          nextUniform() {
        assert(false);
    }

    unsigned int    nextPoisson( double mu ) {
        assert(false);
    }

    virtual ~RNG() {}
protected:
    void    initialize() {}

    T       m_rng;
    type_t  m_type;
    seed_t  m_seed;
};

#endif  // GENERAL_RNG_H_
