#ifndef RECOMBINE_BITSET_HPP_
#define RECOMBINE_BITSET_HPP_

#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <limits>

#include "utility/lowest_bit.h"

template < class Block, class Allocator, class Alphabet >
class recombine_bitset;

namespace boost {

template < typename Block, typename Allocator, typename Alphabet >
inline void to_block_range( const dynamic_bitset< Block, Allocator > & alt, recombine_bitset< Block, Allocator, Alphabet > recomb ) {
    typedef typename std::vector< Block, Allocator >::const_iterator iterator;
    typedef Block block_type;

    iterator first = recomb.m_base->m_bits.begin(), last = recomb.m_base->m_bits.end(), 
            _first = alt.m_bits.begin(), _last = alt.m_bits.end();

    recomb( first, last, _first, _last );

    // truncate trailing bits
    recomb.m_result->resize( (alt.size() >= recomb.m_base->size()) ? alt.size() : recomb.m_base->size());
}

}   // namespace boost

template < class Block, class Allocator, class Alphabet >
class recombine_bitset {
public:
    typedef recombine_bitset< Block, Allocator, Alphabet > self_type;

    typedef boost::dynamic_bitset< Block, Allocator >   bitset_type;
    typedef std::vector< typename Alphabet::locus_t >   recombination_points;
    typedef typename recombination_points::iterator     recombination_iterator;

    friend void boost::to_block_range< Block, Allocator, Alphabet >( const boost::dynamic_bitset< Block, Allocator > &, recombine_bitset< Block, Allocator, Alphabet>  );

    recombine_bitset( bitset_type * base, bitset_type * res, typename Alphabet::pointer alpha, recombination_points * rp ) :
        m_base( base ), m_result( res ), m_alpha( alpha ), rec_points( rp )
    {}

    recombine_bitset( const self_type & other ) :
        m_base( other.m_base ), m_result( other.m_result ), m_alpha( other.m_alpha ), rec_points( other.rec_points )
    {}

/*
    template < class Size, class Generator >
    void generateRecombinationPoints( Size n, Generator gen ) {
        rec_points.clear();

        if( n == 0 ) return;

        rec_points.reserve( n + 2 );
        rec_points.push_back( std::numeric_limit< Alphabet::locus_t >::min() );
        rec_points.push_back( std::numeric_limit< Alphabet::locus_t >::max() );

        std::generate_n( std::back_inserter( rec_points ), n, gen );
        std::sort( rec_points.begin(), rec_points.end() );

        recombination_iterator it = std::unique( rec_points.begin(), rec_points.end() );
        rec_points.resize((it - rec_points.begin()));

// assuming sufficient precision that generating duplicate points is very rare event
//        if( rec_points.size() != n + 2 ) {
// 
//        }
    }
*/
    template < class BlockIterator >
    void operator()( BlockIterator base_first, BlockIterator base_last, BlockIterator alt_first, BlockIterator alt_last ) {
        bool match_base = true, match_alt = true;

        unsigned int seq_pos = 0;
        
        while( true ) {
            if( base_first == base_last ) {
                while( alt_first != alt_last ) {
                    Block alt = (*alt_first++);
                    Block res = alt;
                    bit_walker(res, alt, seq_pos, &self_type::unset_if_base);

                    match_base = ((match_base) && (res == 0 ));
                    match_alt = ((match_alt) && (res == alt));

                    m_result->append(res);
                    seq_pos += bitset_type::bits_per_block;
                }
                break;
            }

            if( alt_first == alt_last ) {
                while( base_first != base_last ) {
                    Block base = (*base_first++);
                    Block res = base;
                    bit_walker( res, base, seq_pos, &self_type::unset_if_alt );

                    match_base = ((match_base) && ( res == base ));
                    match_alt = ((match_alt) && (res == 0));

                    m_result->append(res);
                    seq_pos += bitset_type::bits_per_block;
                }
                break;
            }

            Block base = (*base_first++), alt = (*alt_first++);

            Block res = (base & ~( base ^ alt));     // homozygous bits

            bit_walker( res, (base & ~alt), seq_pos, &self_type::unset_if_alt );
            bit_walker( res, (alt & ~base), seq_pos, &self_type::unset_if_base );

            match_base = ((match_base) && (res == base ));
            match_alt = ((match_alt) && (res == alt ));

            m_result->append( res );
            seq_pos += bitset_type::bits_per_block;
        }

        m_match_base = match_base;
        m_match_alt= match_alt;
    }

    //bitset_type * getBase() { return m_base; }

    bool isBaseMatch() const { return m_match_base; }
    bool isAlternateMatch() const { return m_match_alt; }

    virtual ~recombine_bitset() {}
protected:

    inline Block walk_block_bits( Block base, Block alt, unsigned int pos_offset ) {
        Block bits = (base ^ alt);      // symmetric difference
        Block res = (base & ~bits);     // homozygous bits

        Block bit_mask = 1;
        while( bits ) {     // foreach bit (locus)
            // this loop takes advantage of suffixes which are all 0
            // and truncates the computation after the last heterozygous position
            // NOTE: It may be more efficient to subdivide a block into byte regions
            // to allow for skipping of long regions of 0. Basically, recursively
            // walk the set bits by regions
            if( bits & 1 ) {    // if the locus is heterozygous
                // determine from which recombination region this locus is inherited from (base or alternate)
                recombination_iterator rit = std::lower_bound( rec_points->begin(), rec_points->end(), (*m_alpha)[pos_offset]->first);
                if( (rit - rec_points->begin()) % 2 ) {
                    res = ((res & ~bit_mask) | (alt & bit_mask));
                } else {
                    res = (( res & ~bit_mask) | (base & bit_mask));
                }
            }
            bits >>= 1;
            bit_mask <<= 1;
            ++pos_offset;
        }

        return res;
    }

    inline Block walk_block_bits_sparse( Block base, Block alt, unsigned int pos_offset ) {
        // performance should degrade as span of 0's is less than or equal to 8
        Block bits = (base ^ alt);      // symmetric difference
        Block res = (base & ~bits);     // homozygous bits

        Block bit_mask = 1;
        while( bits ) {     // foreach bit (locus)
            // this loop takes advantage of suffixes which are all 0
            // and truncates the computation after the last heterozygous position
            if( bits & 255 ) {
                unsigned int n = 8;
                while( n-- ) {
                    if( bits & 1 ) {    // if the locus is heterozygous
                        // determine from which recombination region this locus is inherited from (base or alternate)
                        recombination_iterator rit = std::lower_bound( rec_points->begin(), rec_points->end(), (*m_alpha)[pos_offset]->first);
                        if( (rit - rec_points->begin()) % 2 ) {
                            res = ((res & ~bit_mask) | (alt & bit_mask));
                        } else {
                            res = (( res & ~bit_mask) | (base & bit_mask));
                        }
                        n = 0;  // break out of loop to recheck state
                    }
                    bits >>= 1;
                    bit_mask <<= 1;
                    ++pos_offset;
                }
            } else {
                bits >>= 8;
                bit_mask <<= 8;
                pos_offset += 8;
            }
        }

        return res;
    }

    inline unsigned long walk_block_bits_sparse2( unsigned long base, unsigned long alt, unsigned int pos_offset ) {
        // performance should degrade as span of 0's is less than or equal to 8
        Block bits = (base ^ alt);      // symmetric difference
        Block res = (base & ~bits);     // homozygous bits

        Block bit_mask = 1;
        while( bits ) {     // foreach bit (locus)
            // this loop takes advantage of suffixes which are all 0
            // and truncates the computation after the last heterozygous position
            if( bits & 0x00000000000000FF ){
                unsigned int n = 8;
                while( n-- ) {
                    if( bits & 1 ) {    // if the locus is heterozygous
                        // determine from which recombination region this locus is inherited from (base or alternate)
                        recombination_iterator rit = std::lower_bound( rec_points->begin(), rec_points->end(), (*m_alpha)[pos_offset]->first);
                        if( (rit - rec_points->begin()) % 2 ) {
                            res = ((res & ~bit_mask) | (alt & bit_mask));
                        } else {
                            res = (( res & ~bit_mask) | (base & bit_mask));
                        }
                        n = 0;  // break out of loop to recheck state
                    }
                    bits >>= 1;
                    bit_mask <<= 1;
                    ++pos_offset;
                }
            } else if( bits & 0x000000000000FF00) {
                bits >>= 8;
                bit_mask <<= 8;
                pos_offset += 8;
            } else if( bits & 0x00000000FFFF0000) {
                bits >>= 16;
                bit_mask <<= 16;
                pos_offset += 16;
            } else {
                bits >>= 32;
                bit_mask <<=32;
                pos_offset += 32;
            }
        }

        return res;
    }
/*
    inline unsigned long walk_block_bits_sparse3( unsigned long base, unsigned long alt, unsigned int pos_offset ) {
        // performance should degrade as span of 0's is less than or equal to 8
        Block res = (base & ~( base ^ alt));     // homozygous bits

        static const lowest_bit_256 low_bit_map;

        Block _het = ( base & ~alt );       // heterozygous bits relative to base
        unsigned int res_offset = 0;
        while( _het ) {
            unsigned char low_byte = (unsigned char)(_het & 0x00000000000000FF);
            unsigned int _offset = res_offset;
            while( low_byte ) {
                const lowest_bit_256::value_type & v = low_bit_map[low_byte];
                unsigned int tmp_offset = _offset + v.bit_index;

                double loc = (*m_alpha)[ pos_offset + tmp_offset ]->first; 
                recombination_iterator rit = std::lower_bound( rec_points->begin(), rec_points->end(), loc);
                if( (rit - rec_points->begin()) % 2 ) {
                    // bit is supposed to come from alternate
                    // hence clear the bit in the result
                    res &= ~(((Block)1) << (tmp_offset));
                }
                low_byte = v.next;
                _offset += b.bit_shift_next;
            }

            _het >>= 8;
            res_offset += 8;
        }

        _het = (alt & ~base);         // heterozygous bits relative to alt
        res_offset = 0;
        while( _het ) {
            unsigned char low_byte = (unsigned char)(_het & 0x00000000000000FF);
            unsigned int _offset = res_offset;
            while( low_byte ) {
                const lowest_bit_256::value_type & v = low_bit_map[low_byte];
                unsigned int tmp_offset = _offset + v.bit_index;

                double loc = (*m_alpha)[ pos_offset + tmp_offset ]->first; 
                recombination_iterator rit = std::lower_bound( rec_points->begin(), rec_points->end(), loc);
                if( (rit - rec_points->begin()) % 2 == 0 ) {
                    // bit is supposed to come from base
                    // hence clear the bit in the result
                    res &= ~(((Block)1) << (tmp_offset));
                }
                low_byte = v.next;
                _offset += b.bit_shift_next;
            }

            _het >>= 8;
            res_offset += 8;
        }
        return res;
    }*/

    void unset_if_alt( Block & res, unsigned int lookup_offset, unsigned int _offset ) {
        double loc = (*m_alpha)[ lookup_offset + _offset ]->first; 
        recombination_iterator rit = std::lower_bound( rec_points->begin(), rec_points->end(), loc);
        if( (rit - rec_points->begin()) % 2 ) {
            // bit is supposed to come from alt
            // hence clear the bit in the result
            res &= ~(((Block)1) << (_offset));
        }
    }

    void unset_if_base( Block & res, unsigned int lookup_offset, unsigned int _offset ) {
        double loc = (*m_alpha)[ lookup_offset + _offset ]->first; 
        recombination_iterator rit = std::lower_bound( rec_points->begin(), rec_points->end(), loc);
        if( (rit - rec_points->begin()) % 2 == 0 ) {
            // bit is supposed to come from base
            // hence clear the bit in the result
            res &= ~(((Block)1) << (_offset));
        }
    }

    typedef void (self_type::*unset_op_ptr)( Block & res, unsigned int loffset, unsigned int boffset);

    inline void bit_walker( Block & res, Block bits, unsigned int pos_offset,  unset_op_ptr op ) {
        unsigned int res_offset = 0;
        while( bits ) {
            unsigned char low_byte = (unsigned char)(bits & 0x00000000000000FF);
            unsigned int _offset = res_offset;
            while( low_byte ) {
                const lowest_bit_256::value_type & v = low_bit_map[low_byte];
//                unsigned int tmp_offset = _offset + v.bit_index;
//
//                double loc = (*m_alpha)[ pos_offset + tmp_offset ]->first; 
//                recombination_iterator rit = std::lower_bound( rec_points->begin(), rec_points->end(), loc);
//                if( (rit - rec_points->begin()) % 2 == 0 ) {
//                    // bit is supposed to come from base
//                    // hence clear the bit in the result
//                    res &= ~(((Block)1) << (tmp_offset));
//                }
                (this->*op)( res, pos_offset, _offset + v.bit_index );

                low_byte = v.next;
                _offset += v.bit_shift_next;
            }

            bits >>= 8;
            res_offset += 8;
        }
    }

    inline Block walk_block_bits_sparse3( Block base, Block alt, unsigned int pos_offset ) {
        Block res = (base & ~( base ^ alt));     // homozygous bits

        bit_walker( res, (base & ~alt), pos_offset, &self_type::unset_if_alt );
        bit_walker( res, (alt & ~base), pos_offset, &self_type::unset_if_base );

        return res;
    }

    bitset_type * m_base, * m_result;
    typename Alphabet::pointer  m_alpha;
    recombination_points * rec_points;
    bool m_match_base, m_match_alt;

    static const lowest_bit_256 low_bit_map;
};

template < class Block, class Allocator, class Alphabet >
const lowest_bit_256 recombine_bitset< Block, Allocator, Alphabet >::low_bit_map;
#endif  // RECOMBINE_BITSET_HPP_
