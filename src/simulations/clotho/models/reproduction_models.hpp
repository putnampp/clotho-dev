#ifndef REPRODUCTION_MODELS_HPP_
#define REPRODUCTION_MODELS_HPP_

#include "reproduction.hpp"

#include "../clotho.h"
#include "variant_map.hpp"

#include "rng/random_process.hpp"

#include <iostream>

namespace reproduction {
namespace models {
namespace mutation {

template < class V, class VM = variant_map< V >, class GM = typename VM::gamete_t >
class infinite_site : public RandomProcess {
public:
    typedef V   variant_type;
    typedef VM  variant_map_t;
    typedef GM  gamete_t;

    static void initialize( double mu = 0.0001 ) {
        m_variants.reset( new variant_map_t() );
        m_mu = mu;
    }

    static std::shared_ptr< variant_map_t > getVariantMap() {
        return m_variants;
    }

    static void mutate( gamete_t * gm ) {
        return mutate( gm, m_mu );
    }

    static void mutate( gamete_t * gm, double mu ) {
        unsigned int nMut = m_rng->nextPoisson( mu );

        for( unsigned int i = 0; i < nMut; ++i ) {
            typename variant_map_t::value_ptr_t var = getNewVariant();

            gm->addVariant( var );
        }
    }

    static typename variant_map_t::value_ptr_t getNewVariant( ) {
        typename variant_type::key_t k;

        do {
            k = m_rng->nextUniform();
        } while( m_variants->is_known( k ) );

        typename variant_map_t::value_ptr_t vptr = m_variants->createVariant( k );

        return vptr;
    }
protected:
    static std::shared_ptr< variant_map_t > m_variants;
    static double m_mu;
};

template < class V, class VM, class GM >
std::shared_ptr< VM > infinite_site< V, VM, GM >::m_variants;

template < class V, class VM, class GM >
double infinite_site< V, VM, GM >::m_mu = 0.0;

}   // namespace mutation

namespace recombination {

template < unsigned char P >
class no_recomb : public RandomProcess {
public:
    template < class IP, class GM >
    static GM * recombine( IP * ind, GM * ) {
        GM * g = ind->cloneGamete( m_rng->nextInt( P ) );
        return g;
    }
};

template <>
template < class IP, class GM >
GM * no_recomb< 1 >::recombine( IP * ind, GM * ) {
    return ind->cloneGamete( 0 );
}

template <>
template < class IP, class GM >
GM * no_recomb< 2 >::recombine( IP * ind, GM * ) {
    return ind->cloneGamete( (m_rng->nextBool() ? 1 : 0) );
}

}   // namespace recombination
}   // namespace models
}   // namespace reproduction

#endif  // REPRODUCTION_MODELS_HPP_
