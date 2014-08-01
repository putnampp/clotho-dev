#ifndef LOCUS_BITSET_MUTATE_TCC_
#define LOCUS_BITSET_MUTATE_TCC_

#include "processes/reproduction_models.hpp"
#include "genetics/locus_bitset.h"

namespace reproduction {
namespace models {
namespace mutation {

template < >
class mutate_site< locus_bitset::allele_type, locus_bitset::alphabet_t, locus_bitset > : public RandomProcess {
public:
    typedef locus_bitset                    gamete_t;
    typedef typename gamete_t::allele_type  variant_type;
    typedef typename gamete_t::alphabet_t   variant_map_t;
    typedef typename gamete_t::pointer      gamete_ptr;

    typedef typename variant_map_t::pointer map_pointer;

//    mutate_site( map_pointer mp, double mu = 0.0001, bool bFixed = false );
    static void initialize( double mu = 0.0001, bool bFixed = false );

    static map_pointer  getVariantMap();

    static gamete_ptr   mutate( gamete_ptr gm );
    static gamete_ptr   mutate( gamete_ptr gm, double mu, bool bFixed );
protected:

    static gamete_ptr mutate_infinite( gamete_ptr, double );
    static gamete_ptr mutate_fixed( gamete_ptr, double );

    static map_pointer m_variants;
    static double      m_mu;
    static bool        m_bFixed;
};

#define THEADER template<>
#define SPECIALIZATION mutate_site< locus_bitset::allele_type, locus_bitset::alphabet_t, locus_bitset >

typename SPECIALIZATION::map_pointer SPECIALIZATION::m_variants;

double SPECIALIZATION::m_mu = 0.0;

bool SPECIALIZATION::m_bFixed = false;

void SPECIALIZATION::initialize( double mu, bool bFixed ) {
    m_variants = locus_bitset::alphabet_t::getInstance();
    m_mu = mu;
    m_bFixed = bFixed;
}

typename SPECIALIZATION::map_pointer SPECIALIZATION::getVariantMap() {
    return m_variants;
}

typename SPECIALIZATION::gamete_ptr SPECIALIZATION::mutate( gamete_ptr gm ) {
    return mutate( gm, m_mu, m_bFixed );
}

typename SPECIALIZATION::gamete_ptr SPECIALIZATION::mutate( gamete_ptr gm, double mu, bool bFixed) {
    if( !bFixed ) {
        return mutate_infinite(gm, mu);
    } else {
        return mutate_fixed(gm, mu);
    }
}

typename SPECIALIZATION::gamete_ptr SPECIALIZATION::mutate_infinite( gamete_ptr gm, double mu ) {
    unsigned int nMut = m_rng->nextPoisson( mu );
    gamete_ptr res = NULL;
    if( nMut > 0) {
        res = gm->clone();
        assert( res != NULL );

        do {
//            typename variant_map_t::value_ptr_t var = getNewVariant();
            typedef symbol_generator< variant_map_t::locus_t, variant_map_t::allele_t, variant_map_t::index_type, variant_map_t > sgen_type;
            typedef typename sgen_type::symbol_type symbol_type;

            static sgen_type sgen;
            symbol_type s = sgen( m_variants, (infinite_site * ) NULL );
            //std::cout << "Adding variant: " << s << std::endl;
            res->addVariant( s );
        } while( --nMut );
    } else {
        res = gm->copy();
    }
    return res;
}

typename SPECIALIZATION::gamete_ptr SPECIALIZATION::mutate_fixed( gamete_ptr g, double mu ) {
    if( g->size() >= m_variants->size() ) return g; // no sites available for novel mutation

    unsigned int nMut = m_rng->nextPoisson( mu );   // how many mutations should occur
    if( nMut ) {
        gamete_ptr res = (( g != NULL ) ? g->clone() : gamete_t::EMPTY.clone());
        while( nMut-- ) {
            typename variant_map_t::index_type idx = m_rng->nextInt( m_variants->size() );
//            typename variant_map_t::value_ptr_t vptr = (*m_variants)[idx];

            while( (*res)[idx] ) {
                // random site already mutated. check another
                idx = m_rng->nextInt( m_variants->size() );
            }

            res->addVariant( idx );   // site is unmutated. mutate it
        }
        return res;
    }
    return g->copy();
}


#undef THEADER
#undef SPECIALIZATION
}   // namespace mutation
}   // namespace models
}   // namespace reproduction
#endif  // LOCUS_BITSET_MUTATE_TCC_
