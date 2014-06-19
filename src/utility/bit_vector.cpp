#include "utility/bit_vector.h"

#include <cassert>
#include <cstdlib>

bit_vector::bit_block_t bit_vector::m_masks[ bit_vector::BPB ] = {
0x0000000000000001,
0x0000000000000002,
0x0000000000000004,
0x0000000000000008,
0x0000000000000010,
0x0000000000000020,
0x0000000000000040,
0x0000000000000080,
0x0000000000000100,
0x0000000000000200,
0x0000000000000400,
0x0000000000000800,
0x0000000000001000,
0x0000000000002000,
0x0000000000004000,
0x0000000000008000,
0x0000000000010000,
0x0000000000020000,
0x0000000000040000,
0x0000000000080000,
0x0000000000100000,
0x0000000000200000,
0x0000000000400000,
0x0000000000800000,
0x0000000001000000,
0x0000000002000000,
0x0000000004000000,
0x0000000008000000,
0x0000000010000000,
0x0000000020000000,
0x0000000040000000,
0x0000000080000000,
0x0000000100000000,
0x0000000200000000,
0x0000000400000000,
0x0000000800000000,
0x0000001000000000,
0x0000002000000000,
0x0000004000000000,
0x0000008000000000,
0x0000010000000000,
0x0000020000000000,
0x0000040000000000,
0x0000080000000000,
0x0000100000000000,
0x0000200000000000,
0x0000400000000000,
0x0000800000000000,
0x0001000000000000,
0x0002000000000000,
0x0004000000000000,
0x0008000000000000,
0x0010000000000000,
0x0020000000000000,
0x0040000000000000,
0x0080000000000000,
0x0100000000000000,
0x0200000000000080,
0x0400000000000000,
0x0800000000000000,
0x1000000000000000,
0x2000000000000000,
0x4000000000000000,
0x8000000000000000
};

unsigned char bit_vector::m_hweight[256] = {
0, 1, 1, 2, 1, 2, 2, 3,
1, 2, 2, 3, 2, 3, 3, 4,
1, 2, 2, 3, 2, 3, 3, 4,
2, 3, 3, 4, 3, 4, 4, 5,
1, 2, 2, 3, 2, 3, 3, 4,
2, 3, 3, 4, 3, 4, 4, 5,
2, 3, 3, 4, 3, 4, 4, 5,
3, 4, 4, 5, 4, 5, 5, 6,
1, 2, 2, 3, 2, 3, 3, 4,
2, 3, 3, 4, 3, 4, 4, 5,
2, 3, 3, 4, 3, 4, 4, 5,
3, 4, 4, 5, 4, 5, 5, 6,
2, 3, 3, 4, 3, 4, 4, 5,
3, 4, 4, 5, 4, 5, 5, 6,
3, 4, 4, 5, 4, 5, 5, 6,
4, 5, 5, 6, 5, 6, 6, 7,
1, 2, 2, 3, 2, 3, 3, 4,
2, 3, 3, 4, 3, 4, 4, 5,
2, 3, 3, 4, 3, 4, 4, 5,
3, 4, 4, 5, 4, 5, 5, 6,
2, 3, 3, 4, 3, 4, 4, 5,
3, 4, 4, 5, 4, 5, 5, 6,
3, 4, 4, 5, 4, 5, 5, 6,
4, 5, 5, 6, 5, 6, 6, 7,
2, 3, 3, 4, 3, 4, 4, 5,
3, 4, 4, 5, 4, 5, 5, 6,
3, 4, 4, 5, 4, 5, 5, 6,
4, 5, 5, 6, 5, 6, 6, 7,
3, 4, 4, 5, 4, 5, 5, 6,
4, 5, 5, 6, 5, 6, 6, 7,
4, 5, 5, 6, 5, 6, 6, 7,
5, 6, 6, 7, 6, 7, 7, 8 };

struct block_intersect {
    inline bit_vector::bit_block_t operator()( bit_vector::bit_block_t b0, bit_vector::bit_block_t b1 ) {
        return (b0 & b1);
    }
};

struct block_union {
    inline bit_vector::bit_block_t operator()( bit_vector::bit_block_t b0, bit_vector::bit_block_t b1 ) {
        return (b0 | b1);
    }
};

struct block_difference {
    inline bit_vector::bit_block_t operator()( bit_vector::bit_block_t b0, bit_vector::bit_block_t b1 ) {
        return (b0 ^ b1);
    }
};

template<>
void bit_vector::tail_operator< block_intersect >( bit_block_t * p, bit_block_t * e, block_intersect & oper ) const {
    if( p < e ) { memset(p, 0, (e - p) * sizeof( bit_block_t ) ); }
}

bit_vector::bit_vector( size_t nBlocks ) : 
//    m_blocks( nBlocks ),
    m_bits(nBlocks, 0),
    m_weight(0),
    m_bDirty( false )
{
//    if( m_blocks == 0 ) {
//        m_blocks = 1;
//    }
//    if( m_blocks > 1 ) {
//        m_bits.ptr = new bit_block_t[ m_blocks ];
//    }
}

bit_vector::bit_vector( const bit_vector & bv )  :
//    m_blocks( bv.m_blocks ),
    m_bits( bv.m_bits ),
    m_weight( bv.m_weight ),
    m_bDirty( bv.m_bDirty )
{
//    if( m_blocks == 0 ) {
//        // this should technically never happen
//        m_blocks = 1;
//    } else if( m_blocks > 1 ) {
//        m_bits.ptr = new bit_block_t[ m_blocks ];
//        memcpy( m_bits.ptr, bv.m_bits.ptr, sizeof( bit_block_t ) * m_blocks );
//    } else {
//        m_bits.b = bv.m_bits.b;
//    }
    
}

void bit_vector::operator=( const bit_vector & bv ) {
//    resize( bv.m_blocks );
//
//    if( m_blocks == 1 ) {
//        m_bits.b = bv.m_bits.b;
//    } else if( bv.m_blocks == 1 ) {
//        m_bits.ptr[0] = bv.m_bits.b;
//    } else {
//        memcpy( m_bits.ptr, bv.m_bits.ptr, sizeof(bit_block_t) * bv.m_blocks );
//    }

    m_bits = bv.m_bits;
    m_weight = bv.m_weight;
    m_bDirty = bv.m_bDirty;
}

bool bit_vector::operator[]( size_t idx ) const {
//    assert( idx < BPB * m_blocks );
//    assert ( idx < BPB * m_bits.size() );
//    return ((( m_blocks > 1 ) ? m_bits.ptr[ idx / BPB ] : m_bits.b) & m_masks[idx & INDEX_MASK]);
    return !!(m_bits[ idx / BPB ] & m_masks[idx & INDEX_MASK]);
}

void bit_vector::reset() {
//    if( m_blocks > 1 ) {
//        memset( m_bits.ptr, 0, sizeof( bit_block_t ) * m_blocks );
//    } else {
//        m_bits.b = 0;
//    }
    m_bits.clear();
}

//void bit_vector::resize( size_t nBlocks ) {
//    if( nBlocks == m_blocks ) return;
//
//    if( nBlocks == 1 || nBlocks == 0 ) {
//        if( m_bits.ptr != NULL ) {
//            bit_block_t val = m_bits.ptr[0];
//            delete [] m_bits.ptr;
//
//            m_bits.b = val;
//        }
//        m_blocks = 1;
//    } else if( m_blocks > 1 ) {
//        // resizing an already allocated array to some size
//        // which is not 1
//        bit_block_t * tmp = (bit_block_t *) realloc( m_bits.ptr, nBlocks * sizeof(bit_block_t) );
//
//        assert( tmp != NULL );
//        m_bits.ptr = tmp;
//        
//        if( nBlocks > m_blocks ) {
//            tmp = m_bits.ptr + m_blocks;
//            memset( tmp, 0, sizeof(bit_block_t) * (nBlocks - m_blocks));
//        }
//        m_blocks = nBlocks;
//    } else {
//        // increasing the number of blocks from 1 to nBlocks
//        bit_block_t * tmp = new bit_block_t[ nBlocks ];
//
//        memset( tmp, 0, sizeof( bit_block_t ) * nBlocks );
//
//        *tmp = m_bits.b;
//        m_bits.ptr = tmp;
//        m_blocks = nBlocks;
//    }
//}
//
void bit_vector::resize( size_t n ) {
    m_bits.resize(n);
}

bit_vector bit_vector::operator|( const bit_vector & rhs ) const {
    return combiner( rhs, block_union() );
}

bit_vector bit_vector::operator&( const bit_vector & rhs ) const {
    return combiner( rhs, block_intersect() );
}

bit_vector bit_vector::operator^( const bit_vector & rhs ) const {
    return combiner( rhs, block_difference() );
}

size_t bit_vector::hamming_weight() {
    if( m_bDirty ) {
        updateWeight();
    }

    return m_weight;
}

bit_vector::iterator bit_vector::begin() {
//    return iterator( ((m_blocks > 1) ? m_bits.ptr : &m_bits.b), hamming_weight());
    return iterator( &m_bits[0], hamming_weight());
}

bit_vector::iterator bit_vector::end() {
    return iterator();
}

bit_vector::block_iterator bit_vector::block_begin() {
    return m_bits.begin();
}

bit_vector::block_iterator bit_vector::block_end() {
    return m_bits.end();
}

void bit_vector::updateWeight() {
//    unsigned char * tmp = reinterpret_cast< unsigned char * > ( ((m_blocks > 1) ? m_bits.ptr : &m_bits.b));
//    unsigned char * tmp_e = tmp + sizeof(bit_block_t) * m_blocks;
    unsigned char * tmp = reinterpret_cast< unsigned char * > ( &m_bits[0] );
    unsigned char * tmp_e = tmp + sizeof(bit_block_t) * m_bits.size();

    m_weight = 0;

    while( tmp < tmp_e ) {
        m_weight += m_hweight[ (int) (*tmp++) ];
    }
    m_bDirty = false;
}

bit_vector::~bit_vector() {
//    if( m_blocks > 1 ) {
//        delete [] m_bits.ptr;
//    }
}
