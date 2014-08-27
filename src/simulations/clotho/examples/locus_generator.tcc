#ifndef LOCUS_GENERATOR_TCC_
#define LOCUS_GENERATOR_TCC_

#include "genetics/locus.h"

template < >
struct locus_generator< double, RandomProcess::rng_pointer > {
    typedef double return_type;
    typedef RandomProcess::rng_pointer parameter_type;

    locus_generator() : m_params( RandomProcess::getRNG() ) {}
    locus_generator( parameter_type p ) : m_params( p ) {}

    return_type operator()() {
        return m_params->nextUniform();
    }
    parameter_type m_params;
};

#endif  // LOCUS_GENERATOR_TCC_
