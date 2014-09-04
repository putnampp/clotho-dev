#ifndef RECOMBINE_BITSET_SORTED_TCC_
#define RECOMBINE_BITSET_SORTED_TCC_

#include "recombine_bitset.hpp"
#include "sorted_allele_alphabet2.h"

#include <boost/icl/interval.hpp>
#include <boost/icl/interval_map.hpp>
#include <set>

template <>
class recombine_bitset< typename SortedAlleleAlphabet2::bitset_type::block_type,
    typename SortedAlleleAlphabet2::bitset_type::allocator_type,
    SortedAlleleAlphabet2 > {
public:
    typedef recombine_bitset< typename SortedAlleleAlphabet2::bitset_type::block_type, typename SortedAlleleAlphabet2::bitset_type::allocator_type, SortedAlleleAlphabet2 > self_type;

    typedef SortedAlleleAlphabet2 Alphabet;
    typedef typename SortedAlleleAlphabet2::bitset_type::block_type Block;
    typedef typename SortedAlleleAlphabet2::bitset_type::allocator_type Allocator;

    typedef SortedAlleleAlphabet2::bitset_type   bitset_type;
    typedef std::vector< typename Alphabet::locus_t >   recombination_points;
    typedef typename recombination_points::iterator     recombination_iterator;
    typedef typename Alphabet::active_iterator          active_iterator;

    typedef boost::icl::interval< locus_t >::type       locus_range_type;
    typedef boost::icl::interval_bounds                 bound_type;
    typedef boost::icl::interval_map< Alphabet::locus_t, Block > base_recomb_ranges;

    typedef lowest_bit_64K                              lowest_bit_map;

    struct result_stats {
        bool match_base, match_alt, is_empty;
#if LOGGING
        unsigned int gen;
        result_stats(unsigned int g = 0) : match_base(false), match_alt(false), is_empty(false), gen(g) {}
#else
        result_stats() : match_base(false), match_alt(false), is_empty(false) {}
#endif
    };

    typedef result_stats result_type;

    friend void boost::to_block_range< Block, Allocator, Alphabet >( const boost::dynamic_bitset< Block, Allocator > &, recombine_bitset< Block, Allocator, Alphabet>  );

    recombine_bitset( bitset_type * base, bitset_type * res,
                      typename Alphabet::pointer alpha,
                      recombination_points * rp, result_type * stats = NULL ) :
        m_base( base )
        , m_result( res )
        , m_alpha( alpha )
        , rec_points( rp )
        , m_stats( stats ) {
    }

    recombine_bitset( const self_type & other ) :
        m_base( other.m_base )
        , m_result( other.m_result )
        , m_alpha( other.m_alpha )
        , rec_points( other.rec_points )
        , m_stats( other.m_stats ) {
    }

    template < class BlockIterator >
    void operator()( BlockIterator base_first, BlockIterator base_last, BlockIterator alt_first, BlockIterator alt_last ) {
        bool match_base = true, match_alt = true, is_empty = true;

        active_iterator seq_pos = m_alpha->active_begin();

        Block base_mask = 0;
        base_recomb_ranges mask_intervals;

        std::set< unsigned int > contension_indices;

        Alphabet::locus_t lo = 0.0;
        bool is_base = true;
        for( recombination_iterator it = rec_points->begin(); it != rec_points->end(); it++ ) {
            if( is_base ) {

                unsigned int lo_offset = lo * Alphabet::bits_per_range;
                unsigned int hi_offset = (*it) * Alphabet::bits_per_range;
                Block m = 0;
                if( lo_offset == hi_offset ) {
                    contension_indices.insert( lo_offset );
                }

                double lo_bound = (double)lo_offset / (double)Alphabet::bits_per_range;

                if( lo_bound < (*it) ) {

                }
                mask_intervals += std::make_pair( locus_range_type( lo, (*it), bound_type::open()), m );
            }
            lo = (*it);
            is_base = !is_base;
        }
    }

protected:
    bitset_type * m_base, * m_result;
    typename Alphabet::pointer  m_alpha;
    recombination_points * rec_points;
    result_type        * m_stats;

    static const lowest_bit_map low_bit_map;
};

#endif  // RECOMBINE_BITSET_SORTED_TCC_
