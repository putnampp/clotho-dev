#ifndef RECOMBINE_BITSET_SORTED_TCC_
#define RECOMBINE_BITSET_SORTED_TCC_

#include "recombine_bitset.hpp"
#include "sorted_allele_alphabet2.h"

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
    typedef typename Alphabet::active_iterator          set_iterator;

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

        set_iterator seq_pos = m_alpha->active_begin();

        Block rec_point_mask = 0, base_mask = 0, alt_mask = 0;
        
        buildRecombinationMasks( rec_point_mask, base_mask, alt_mask );

/*
        std::cerr << "Recombination Points: {";
        for( recombination_iterator it = rec_points->begin(); it != rec_points->end(); it++ ) {
            std::cerr << (*it) << ",";
        }
        std::cerr << "}" << std::endl;
        std::cerr << "Ambiguous Mask: " << std::hex << rec_point_mask << std::endl;
        std::cerr << "Base Mask: " << std::hex << base_mask << std::endl;
        std::cerr << "Alt Mask: " << std::hex << alt_mask << std::endl;
        std::cerr << std::endl;
*/
    
        m_result->resize( m_alpha->size(), false);
        typename std::vector< Block, Allocator>::iterator res_it = m_result->m_bits.begin();

        while( true ) {
            if( base_first == base_last ) {
                while( alt_first != alt_last ) {
                    Block alt = (*alt_first++);

                    Block res = combineBlocks( 0, alt, seq_pos, rec_point_mask, base_mask, alt_mask );

                    is_empty = ((is_empty) && (res == 0));
                    match_base = ((match_base) && (res == 0 ));
                    match_alt = ((match_alt) && (res == alt));

                    (*res_it) = res;
                    ++res_it;
                    seq_pos += bitset_type::bits_per_block;
                }
                break;
            }

            if( alt_first == alt_last ) {
                while( base_first != base_last ) {
                    Block base = (*base_first++);
                    Block res = combineBlocks( base, 0, seq_pos, rec_point_mask, base_mask, alt_mask );

                    is_empty = ((is_empty) && (res == 0));
                    match_base = ((match_base) && ( res == base ));
                    match_alt = ((match_alt) && (res == 0));

                    (*res_it) = res;
                    ++res_it;
                    seq_pos += bitset_type::bits_per_block;
                }
                break;
            }

            Block base = (*base_first++), alt = (*alt_first++);

            Block res = combineBlocks( base, alt, seq_pos, rec_point_mask, base_mask, alt_mask );

            is_empty = ((is_empty) && (res == 0));
            match_base = ((match_base) && (res == base ));
            match_alt = ((match_alt) && (res == alt ));

            (*res_it) = res;
            ++res_it;
            seq_pos += bitset_type::bits_per_block;
        }

        if( m_stats ) {
            m_stats->match_base = match_base;
            m_stats->match_alt = match_alt;
            m_stats->is_empty = is_empty;
        }
    }

protected:

    Block combineBlocks( const Block & base, const Block & alt, set_iterator alpha_it, 
                        const Block & rec_mask, const Block & base_mask, const Block & alt_mask )
    {
        Block res = 0;

        bit_walker_unrolled( res, ((base ^ alt) & rec_mask), base, alt, alpha_it);

        Block hom = ((base & alt) & rec_mask);

        res |= ((base & base_mask) | (alt & alt_mask) | hom);

        return res;
    }

    void buildRecombinationMasks( Block & rec_point_mask, Block & base_mask, Block & alt_mask ) {
        bool is_base = true;
        Block prev_mask = (Block)-1;
        for( recombination_iterator it = rec_points->begin(); it != rec_points->end(); it++ ) {
            unsigned int hi_offset = (*it) * Alphabet::bits_per_range;

            rec_point_mask |= SortedAlleleAlphabet2::bit_position_masks[ hi_offset ];

            if( is_base ) {
                base_mask |= (prev_mask & SortedAlleleAlphabet2::low_order_bit_masks[hi_offset]);
            }

            is_base = !is_base;
            prev_mask = ~SortedAlleleAlphabet2::low_order_bit_masks[hi_offset];
        }
        if( is_base ) {
            base_mask |= (prev_mask & SortedAlleleAlphabet2::low_order_bit_masks[SortedAlleleAlphabet2::bits_per_range - 1]);
        }

        base_mask &= (~rec_point_mask);
        alt_mask = (~base_mask) & (~rec_point_mask);

        if( rec_points->front() > 0.0 ) {
            rec_points->insert( rec_points->begin(), 0.0);
        }
        if( rec_points->back() < 1.0 ) {
            rec_points->push_back( 1.0 );
        }
    }

    inline void bit_walk( Block & res, typename lowest_bit_map::block_type low_byte, const Block & base, const Block & alt, set_iterator alpha_it, unsigned int _offset ) {
        const lowest_bit_map::value_type * v = low_bit_map.begin() + low_byte;
        do {
            unsigned int idx = _offset + v->bit_index;
            typename Alphabet::locus_t loc = accessor::get< set_iterator, typename Alphabet::locus_t >(alpha_it + idx);

            recombination_iterator rit = std::upper_bound( rec_points->begin(), rec_points->end(), loc);

            res |= ((((rit - rec_points->begin()) % 2) ? base : alt) & SortedAlleleAlphabet2::bit_position_masks[idx]);

            _offset += v->bit_shift_next;
            v = v->next_ptr;
        } while( v != NULL );
    }

    inline void block_walker( Block & res, unsigned long _bits, const Block & base, const Block & alt, set_iterator base_it ) {
        typename lowest_bit_map::block_type low_byte = (typename lowest_bit_map::block_type)(_bits);

        if( low_byte ) {    // block 0
            bit_walk( res, low_byte, base, alt, base_it, 0 );
        }
        _bits /= lowest_bit_map::max_values;

        if( !_bits ) return;

        low_byte = (typename lowest_bit_map::block_type)(_bits);

        if( low_byte ) {    // block 1
            bit_walk( res, low_byte, base, alt, base_it, lowest_bit_map::block_width );
        }
        _bits /= lowest_bit_map::max_values;

        if( !_bits ) return;
        low_byte = (typename lowest_bit_map::block_type)(_bits);

        if( low_byte ) {    // block 2
            bit_walk( res, low_byte, base, alt, base_it, 2 * lowest_bit_map::block_width );
        }
        _bits /= lowest_bit_map::max_values;

        low_byte = (typename lowest_bit_map::block_type)(_bits);

        if( low_byte ) {    // block 3
            bit_walk( res, low_byte, base, alt, base_it, 3 * lowest_bit_map::block_width );
        }
    }

    inline void bit_walker_unrolled( Block & res, const Block _bits, const Block base, const Block alt, set_iterator base_it ) {
        if( !_bits ) return;

        block_walker( res, _bits, base, alt, base_it);
    }

    bitset_type * m_base, * m_result;
    typename Alphabet::pointer  m_alpha;
    recombination_points * rec_points;
    result_type        * m_stats;

    static const lowest_bit_map low_bit_map;
};

const typename recombine_bitset< typename SortedAlleleAlphabet2::bitset_type::block_type, typename SortedAlleleAlphabet2::bitset_type::allocator_type, SortedAlleleAlphabet2 >::lowest_bit_map recombine_bitset< typename SortedAlleleAlphabet2::bitset_type::block_type, typename SortedAlleleAlphabet2::bitset_type::allocator_type, SortedAlleleAlphabet2 >::low_bit_map;
#endif  // RECOMBINE_BITSET_SORTED_TCC_
