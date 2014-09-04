#ifndef FITNESS_BITSET_2_HPP_
#define FITNESS_BITSET_2_HPP_

#include "locus_bitset.h"

template < class Alphabet, class HomPolicy, class HetPolicy, class ResultType >
class fitness_bitset2 {
public:
    typedef fitness_bitset2< Alphabet, HomPolicy, HetPolicy, ResultType > self_type;

    typedef locus_bitset        locus_bitset_type;
    typedef locus_bitset_type::bitset_type bitset_type;
    typedef ResultType          result_type;

    fitness_bitset2( typename Alphabet::pointer alpha, HomPolicy & homozygous, HetPolicy & heterozygous, result_type init ) :
        m_alpha( alpha ), m_hom( &homozygous ), m_het( &heterozygous ), m_result( init )
    {}

    fitness_bitset2( const self_type & other ) :
        m_alpha( other.m_alpha ), m_hom(other.m_hom), m_het( other.m_het ), m_result( other.m_result)
    {}

    void operator()( locus_bitset_type & g1, locus_bitset_type & g2, result_type & res ) {
        operator()( *g1.getBits(), *g2.getBits(), res );
    }

    void operator()( const bitset_type & g1, const bitset_type & g2, result_type & res ) {
        if( g1.size() == g2.size() ) {
            _union = (g1 | g2);
            _intersect = (g1 & g2);
        } else if( g1.size() < g2.size() ) {
            _union = g1;
            _union.resize( g2.size(), false);
            _intersect = _union;
            _union |= g2;
            _intersect &= g2;
        } else {
            _union = g2;
            _union.resize( g1.size(), false );
            _intersect = _union;
            _union |= g1;
            _intersect &= g1;
        }

        typename Alphabet::active_iterator seq_pos = m_alpha->active_begin();
        bitset_type::size_type seq_idx = 0;
        bitset_type::size_type idx = _union.find_first();
        while( idx != bitset_type::npos ) {
            seq_pos += (idx - seq_idx);
            seq_idx = idx;

            if( _intersect[idx] ) {
                (*m_hom)(res, accessor::get< typename Alphabet::active_iterator, typename Alphabet::allele_t >( seq_pos ));
            } else {
                (*m_het)(res, accessor::get< typename Alphabet::active_iterator, typename Alphabet::allele_t >( seq_pos ));
            }
            idx = _union.find_next(idx);
        }
    }

    result_type getResult() {
        return m_result;
    }

    virtual ~fitness_bitset2() {}
protected:
    typename Alphabet::pointer  m_alpha;
    HomPolicy * m_hom;
    HetPolicy * m_het;
    result_type m_result;

    bitset_type _union, _intersect;
};

#endif  // FITNESS_BITSET_2_HPP_
