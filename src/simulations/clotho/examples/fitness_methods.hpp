#ifndef FITNESS_METHODS_HPP_
#define FITNESS_METHODS_HPP_

//#include "genetics/fitness_bitset.hpp"
#include "genetics/fitness_bitset2.hpp"

template < class Allele >
struct het_fitness {
    typedef Allele allele_type;
    typedef allele_type * allele_pointer;

    inline void operator()( double & f, allele_pointer v ) {
        f *= (1. + v->dominance * v->selection);
    }

    inline void operator()( double & f, const allele_type & v ) {
        f *= (1. + v.dominance * v.selection);
    }
};

template < class Allele >
struct hom_fitness {
    typedef Allele allele_type;
    typedef allele_type * allele_pointer;

    hom_fitness( double s = 1. ) : m_scaling(1.) {}
    inline void operator()( double & f, allele_pointer v ) {
        f *= (1. + v->selection * m_scaling);
    }

    inline void operator()( double & f, const allele_type & v ) {
        f *= (1. + v.selection * m_scaling);
    }

    double m_scaling;
};

template < class het_policy, class hom_policy, class Gamete, class IndividualPointer >
class fitness_multiplicative {
public:
    typedef Gamete gamete_type;
    typedef gamete_type * gamete_pointer;

    fitness_multiplicative() {}

    fitness_multiplicative( het_policy & het, hom_policy & hom ) :
        m_het_case(het),
        m_hom_case(hom) {
    }

    double operator()( double f, IndividualPointer ind ) {
        return (*this)(f, ind->getProperties()->getGamete(0), ind->getProperties()->getGamete(1) );
    }

    double operator()( double f, gamete_pointer g1, gamete_pointer g2 ) {
        if( g1 == g2 ) return f;

        typename gamete_type::alphabet_t::pointer alpha = g1->getAlphabet();

        //fitness_bitset< typename gamete_type::bitset_type::block_type, typename gamete_type::bitset_type::allocator_type, typename gamete_type::alphabet_t, hom_policy, het_policy, double > fit( g1->getBits(), alpha, m_hom_case, m_het_case, f);
//        boost::to_block_range( *g2->getBits(), fit );
        fitness_bitset2< typename gamete_type::alphabet_t, hom_policy, het_policy, double > fit( alpha, m_hom_case, m_het_case, f);

        fit( *g1, *g2, f);

        //return fit.getResult();
        return f;
    }

    virtual ~fitness_multiplicative() {}
protected:
    het_policy m_het_case;
    hom_policy m_hom_case;
};

#endif  // FITNESS_METHODS_HPP_
