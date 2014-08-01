#ifndef BIT_VECTOR_H_
#define BIT_VECTOR_H_

#include <cstring>
#include <vector>

#include "bit_iterator.hpp"

class bit_vector {
public:
    typedef unsigned long   bit_block_t;

    //union block_t {
    //    bit_block_t b;
    //    bit_block_t * ptr;
    //    block_t() : b(0) {}
    //};
    //
    typedef std::vector< bit_block_t > block_t;
    typedef typename block_t::iterator block_iterator;

    enum    {   BPB = sizeof( bit_block_t ) * 8, // bits per block
                INDEX_MASK = (BPB - 1 ) // assumes BPB is power of 2
//                INDEX_MASK = 0x000000000000003F  // (0xFFFFFFFFFFFFFFFF >> (BPB - log( BPB = 64 )))
            };

    typedef BitIterator< bit_block_t > iterator;

    bit_vector( size_t block_count = 1 );
    bit_vector( const bit_vector & bv );

    void operator=( const bit_vector & bv );

    inline void operator()( size_t idx, bool state ) {
        (*this)((idx / BPB), (idx % BPB), state);
    }

    inline void operator()( size_t b_idx, size_t offset, bool state ) {
//        if( b_idx >= m_blocks ) {
//            resize( b_idx + 1 );
//        }

        bit_block_t mask = m_masks[offset];
//        bit_block_t * ptr = ((m_blocks > 1) ? &m_bits.ptr[b_idx] : &m_bits.b);
        bit_block_t * ptr = &m_bits[b_idx];
        if( !!(*ptr & mask ) != state ) {
            if( state ) {
                *ptr |= mask;
                ++m_weight;
            } else {
                *ptr &= ~mask;
                --m_weight;
            }
        }
    }

    bool operator[]( size_t idx ) const;

    // union of two vectors
    bit_vector operator|( const bit_vector & bv ) const;

    // intersection of two vectors
    bit_vector operator&( const bit_vector & bv ) const;

    // difference of two vectors
    bit_vector operator^( const bit_vector & bv ) const;

    void reset();
    void resize( size_t nBlocks );

//    inline size_t block_count() const { return m_blocks; }
    inline size_t block_count() const {
        return m_bits.size();
    }

    size_t hamming_weight();

    iterator begin();
    iterator end();

    block_iterator block_begin();
    block_iterator block_end();

    virtual ~bit_vector();
protected:
    template < class OP >
    void tail_operator( bit_block_t * p, bit_block_t * e, OP & oper ) const {
        return;
    }

    template < typename OP >
    bit_vector combiner( const bit_vector & rhs, OP oper ) const {
//        bit_vector res( ((rhs.m_blocks <= m_blocks) ? *this : rhs));
        bit_vector res( ((rhs.m_bits.size() <= m_bits.size()) ? *this : rhs));

//        bit_block_t * pres = ((res.m_blocks > 1) ? res.m_bits.ptr : &res.m_bits.b);
        bit_block_t * pres = &res.m_bits[0];
//        bit_block_t * pres_e = pres + res.m_blocks;
        bit_block_t * pres_e = pres + res.m_bits.size();

//        const bit_block_t * p0 = ((rhs.m_blocks > 1 ) ? rhs.m_bits.ptr : &rhs.m_bits.b);
        const bit_block_t * p0 = &rhs.m_bits[0];
//        const bit_block_t * p0_e = p0 + rhs.m_blocks;
        const bit_block_t * p0_e = p0 + rhs.m_bits.size();

//        if( rhs.m_blocks > m_blocks ) {
        if( rhs.m_bits.size() > m_bits.size()) {
//            p0 = ((m_blocks > 1 ) ? m_bits.ptr : &m_bits.b);
            p0 = &m_bits[0];
//            p0_e = p0 + m_blocks;
            p0_e = p0 + m_bits.size();
        }

        while( p0 < p0_e ) {
            (*pres) = oper((*pres), (*p0++));
            ++pres;
        }

        tail_operator( pres, pres_e, oper );

        res.m_bDirty = true;
        return res;
    }

    void updateWeight();

    block_t         m_bits;
//    size_t          m_blocks;

    size_t          m_weight;
    bool            m_bDirty;

    static bit_block_t      m_masks[ BPB ];
    static unsigned char    m_hweight[ 256 ];
};

#endif  // BIT_VECTOR_H_
