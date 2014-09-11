#include "genetics/sorted_allele_alphabet2.h"
#include <iostream>

#include <cassert>

#include <sstream>
#include <algorithm>

#include "utility/bitset_ops.hpp"

// OFFSET_BIT_MASK => bit_[n] = 1
#define OFFSET_BIT_MASK( n ) (1UL << n)

// LOW_BIT_MASK => bits_{[0, n]} = 1
#define LOW_BIT_MASK( n ) ((1UL << (n + 1) ) - 1)

const typename SortedAlleleAlphabet2::block_type  SortedAlleleAlphabet2::low_order_bit_masks[ bits_per_range ] = {
    LOW_BIT_MASK( 0 ), LOW_BIT_MASK( 1 ), LOW_BIT_MASK( 2 ), LOW_BIT_MASK( 3 ),
    LOW_BIT_MASK( 4 ), LOW_BIT_MASK( 5 ), LOW_BIT_MASK( 6 ), LOW_BIT_MASK( 7 ),
    LOW_BIT_MASK( 8 ), LOW_BIT_MASK( 9 ), LOW_BIT_MASK( 10 ), LOW_BIT_MASK( 11 ),
    LOW_BIT_MASK( 12 ), LOW_BIT_MASK( 13 ), LOW_BIT_MASK( 14 ), LOW_BIT_MASK( 15 ),
    LOW_BIT_MASK( 16 ), LOW_BIT_MASK( 17 ), LOW_BIT_MASK( 18 ), LOW_BIT_MASK( 19 ),
    LOW_BIT_MASK( 20 ), LOW_BIT_MASK( 21 ), LOW_BIT_MASK( 22 ), LOW_BIT_MASK( 23 ),
    LOW_BIT_MASK( 24 ), LOW_BIT_MASK( 25 ), LOW_BIT_MASK( 26 ), LOW_BIT_MASK( 27 ),
    LOW_BIT_MASK( 28 ), LOW_BIT_MASK( 29 ), LOW_BIT_MASK( 30 ), LOW_BIT_MASK( 31 ),
    LOW_BIT_MASK( 32 ), LOW_BIT_MASK( 33 ), LOW_BIT_MASK( 34 ), LOW_BIT_MASK( 35 ),
    LOW_BIT_MASK( 36 ), LOW_BIT_MASK( 37 ), LOW_BIT_MASK( 38 ), LOW_BIT_MASK( 39 ),
    LOW_BIT_MASK( 40 ), LOW_BIT_MASK( 41 ), LOW_BIT_MASK( 42 ), LOW_BIT_MASK( 43 ),
    LOW_BIT_MASK( 44 ), LOW_BIT_MASK( 45 ), LOW_BIT_MASK( 46 ), LOW_BIT_MASK( 47 ),
    LOW_BIT_MASK( 48 ), LOW_BIT_MASK( 49 ), LOW_BIT_MASK( 50 ), LOW_BIT_MASK( 51 ),
    LOW_BIT_MASK( 52 ), LOW_BIT_MASK( 53 ), LOW_BIT_MASK( 54 ), LOW_BIT_MASK( 55 ),
    LOW_BIT_MASK( 56 ), LOW_BIT_MASK( 57 ), LOW_BIT_MASK( 58 ), LOW_BIT_MASK( 59 ),
    LOW_BIT_MASK( 60 ), LOW_BIT_MASK( 61 ), LOW_BIT_MASK( 62 ), (block_type)-1
};

const typename SortedAlleleAlphabet2::block_type  SortedAlleleAlphabet2::bit_position_masks[ bits_per_range ] = {
    OFFSET_BIT_MASK( 0 ), OFFSET_BIT_MASK( 1 ), OFFSET_BIT_MASK( 2 ), OFFSET_BIT_MASK( 3 ),
    OFFSET_BIT_MASK( 4 ), OFFSET_BIT_MASK( 5 ), OFFSET_BIT_MASK( 6 ), OFFSET_BIT_MASK( 7 ),
    OFFSET_BIT_MASK( 8 ), OFFSET_BIT_MASK( 9 ), OFFSET_BIT_MASK( 10 ), OFFSET_BIT_MASK( 11 ),
    OFFSET_BIT_MASK( 12 ), OFFSET_BIT_MASK( 13 ), OFFSET_BIT_MASK( 14 ), OFFSET_BIT_MASK( 15 ),
    OFFSET_BIT_MASK( 16 ), OFFSET_BIT_MASK( 17 ), OFFSET_BIT_MASK( 18 ), OFFSET_BIT_MASK( 19 ),
    OFFSET_BIT_MASK( 20 ), OFFSET_BIT_MASK( 21 ), OFFSET_BIT_MASK( 22 ), OFFSET_BIT_MASK( 23 ),
    OFFSET_BIT_MASK( 24 ), OFFSET_BIT_MASK( 25 ), OFFSET_BIT_MASK( 26 ), OFFSET_BIT_MASK( 27 ),
    OFFSET_BIT_MASK( 28 ), OFFSET_BIT_MASK( 29 ), OFFSET_BIT_MASK( 30 ), OFFSET_BIT_MASK( 31 ),
    OFFSET_BIT_MASK( 32 ), OFFSET_BIT_MASK( 33 ), OFFSET_BIT_MASK( 34 ), OFFSET_BIT_MASK( 35 ),
    OFFSET_BIT_MASK( 36 ), OFFSET_BIT_MASK( 37 ), OFFSET_BIT_MASK( 38 ), OFFSET_BIT_MASK( 39 ),
    OFFSET_BIT_MASK( 40 ), OFFSET_BIT_MASK( 41 ), OFFSET_BIT_MASK( 42 ), OFFSET_BIT_MASK( 43 ),
    OFFSET_BIT_MASK( 44 ), OFFSET_BIT_MASK( 45 ), OFFSET_BIT_MASK( 46 ), OFFSET_BIT_MASK( 47 ),
    OFFSET_BIT_MASK( 48 ), OFFSET_BIT_MASK( 49 ), OFFSET_BIT_MASK( 50 ), OFFSET_BIT_MASK( 51 ),
    OFFSET_BIT_MASK( 52 ), OFFSET_BIT_MASK( 53 ), OFFSET_BIT_MASK( 54 ), OFFSET_BIT_MASK( 55 ),
    OFFSET_BIT_MASK( 56 ), OFFSET_BIT_MASK( 57 ), OFFSET_BIT_MASK( 58 ), OFFSET_BIT_MASK( 59 ),
    OFFSET_BIT_MASK( 60 ), OFFSET_BIT_MASK( 61 ), OFFSET_BIT_MASK( 62 ), OFFSET_BIT_MASK( 63 )
};

SortedAlleleAlphabet2::SortedAlleleAlphabet2()  {}

SortedAlleleAlphabet2::index_type     SortedAlleleAlphabet2::getSymbol( const locus_t & l, const allele_t & a, bool allowDuplicates ) {
    index_type idx = find( l );

    if( idx == npos ) {
        // locus does not currently exist
        idx = findFreeRangeContaining( l );
    } else {
//        std::cerr << "Locus already exists in database: " << l << " at " << idx << std::endl;
        if( !m_free_list[ idx ] ) {
            // locus exists, AND is still active (not at a free position)
            //a
//            std::cerr << "Locus is still active" << std::endl;
            if( !allowDuplicates ) {
                // and duplicates are not allowed to exist
//                std::cerr << "Not creating a new index" << std::endl;
                return idx;
            }
            idx = findFreeRangeContaining( l );
        }
    }

    updateSymbolAt(idx, l, a );

    return idx;
}

void SortedAlleleAlphabet2::updateFreeSymbols( const bitset_type & fs ) {
    // boost::dynamic_bitset returns # of bits as size
    assert(m_free_intersect.size() == m_free_union.size());

    typedef std::vector< typename bitset_type::block_type, typename bitset_type::allocator_type > buffer_type;

    typedef typename buffer_type::iterator iterator;
    typedef typename buffer_type::const_iterator citerator;

    if( fs.size() == 0 ) {
        m_free_intersect.reset();
    } else if( fs.size() < m_free_intersect.size() ) {
        citerator first = fs.m_bits.begin(), last = fs.m_bits.end();

        iterator int_it = m_free_intersect.m_bits.begin(), int_last = m_free_intersect.m_bits.end(),
                 un_it = m_free_union.m_bits.begin();
        while( first != last ) {
            block_type t = (*first++);
            (*int_it++) &= t;
            (*un_it++) |= t;
        }
        while( int_it != int_last ) {
            (*int_it++) = 0;
        }

    } else {
        if( fs.size() > m_free_intersect.size() ) {
            m_free_intersect.resize( fs.size(), false);
            m_free_union.resize( fs.size(), false );
        }

        m_free_intersect &= fs;
        m_free_union |= fs;
    }
}

void SortedAlleleAlphabet2::setState() {
    m_free_list = m_free_intersect | ~m_free_union;
    m_free_mask = ~m_free_list;

    buildFreeRanges();
}

void SortedAlleleAlphabet2::resetFreeSymbols() {
    if( m_free_union.size() < m_db.size() ) {
        m_free_union.resize( m_db.size(), false);
        m_free_intersect.resize( m_db.size(), false);
        m_free_ranges.resize( m_db.size(), false );
    }

    m_free_union.reset();
    m_free_intersect.reset().flip();
}

size_t SortedAlleleAlphabet2::block_count() {
    return m_free_list.num_blocks();
}

SortedAlleleAlphabet2::locus_iterator SortedAlleleAlphabet2::operator[]( index_type idx ) {
//    if( idx >= m_db.size() ) {
//        std::cerr << "requesting an index: " << idx << " [" << m_db.size() << "]" << std::endl;
//    }
    assert( idx < m_db.size() );
    return m_db.begin() + idx;
}

size_t SortedAlleleAlphabet2::size() {
    return m_db.size();
}

size_t SortedAlleleAlphabet2::active_size() {
    return m_db.size() - m_free_list.count();
}

bool SortedAlleleAlphabet2::isLocus( locus_t & l ) const {
    //clocus_iterator res = m_db.find(l);
    //return res != m_db.end();
    return false;
}

typename SortedAlleleAlphabet2::index_type SortedAlleleAlphabet2::appendSymbol( const locus_t & l, const allele_t & a ) {
    index_type idx = m_db.size() + (l * bits_per_range);

//    std::cerr << "Appending Symbol at " << idx << std::endl;

    unsigned int i = bits_per_range;

    while( i-- ) {
        m_db.push_back( std::make_pair( 0, a ) );
    }

    m_db[ idx ] = std::make_pair(l,a);

    assert( m_free_list.size() + bits_per_range == m_db.size() );

    m_free_list.resize( m_db.size(), true );

    buildFreeRanges();

    return idx;
}

void SortedAlleleAlphabet2::updateSymbolAt( index_type & idx, const locus_t & l, const allele_t & a ) {
    if( idx == npos ) {
        idx = appendSymbol( l, a );
        m_lookup_locus.insert( std::make_pair( l, idx ));
    } else {
        //assert( idx < m_db.size() );
        if( idx >= m_db.size() ) {
//            std::cerr << "Index out of range: " << idx << " [" << m_db.size() << "]" << std::endl;
            assert(false);
        }

        if( l != m_db[idx].first ) {
            m_lookup_locus.erase( m_db[idx].first );
            m_lookup_locus.insert( std::make_pair( l, idx ));
            m_db[ idx ] = std::make_pair( l, a );
        } else {
            assert( m_lookup_locus[l] == idx );
            m_db[idx].second = a;
        }
    }

    updateFreeIndex( idx, false );
}

void SortedAlleleAlphabet2::updateFreeIndex( index_type idx, bool isFree ) {
    index_type range_idx = idx / bits_per_range, range_offset = idx % bits_per_range;
    index_type path = range_idx + m_free_ranges.num_blocks();

    block_type offset_mask = bit_position_masks[ range_offset ];
    block_type offset_set = ((isFree) ? offset_mask : 0);

    offset_mask = ~offset_mask;

//    std::cerr << "Updating free index: " << idx << " [" << range_idx << ", " << path << "]" << std::endl;

    m_free_list.m_bits[ range_idx ] &= offset_mask;
    m_free_list.m_bits[ range_idx ] |= offset_set;


//    std::cerr << "Setting c0 to " << range_idx << " (free list)" << std::endl;
    block_type c0 = m_free_list.m_bits[ range_idx ],
               c1 = 0 ;

    range_idx += ((range_idx & 1)? -1 : 1);
    if( range_idx < m_free_list.num_blocks() ) {
//        std::cerr << "Setting c1 to " << range_idx << " (free list)" << std::endl;
        c1 = m_free_list.m_bits[ range_idx ];
    }

    if( path == 1 ) {
        m_free_ranges.m_bits[ 0 ] = (c0 | c1);
        return;
    }

    while( true ) {
        index_type p = (((path & 1) ? ((path - 2)/ 2) : ((path - 1) / 2)));

        path >>= 1;

//        std::cerr << "Updating free range idx: " << p << std::endl;
        m_free_ranges.m_bits[ p ] = ( c0 | c1 );

        if( !p ) break;

//        std::cerr << "Setting c0 to " << p << " (free range)" << std::endl;
        c0 = m_free_ranges.m_bits[p];
        p += ((p & 1) ? 1 : -1);
//        std::cerr << "Setting c1 to " << p << " (free range)" << std::endl;
        c1 = m_free_ranges.m_bits[p];
    }
}

typename SortedAlleleAlphabet2::index_type SortedAlleleAlphabet2::findFreeRangeContaining( const locus_t & l ) {
    index_type _offset = l * bits_per_range;
    block_type offset_mask = bit_position_masks[ _offset ];

    if( !m_free_ranges.empty() && (m_free_ranges.m_bits[ 0 ] & offset_mask) ) {
        if( m_free_list.num_blocks() == 1 ) {
            assert( m_free_list[ _offset ] );
            return _offset;
        }

        size_t path = 0, level = 2;
        while( level < m_free_ranges.num_blocks() ) {
            size_t p = (2 * path) + 1;

            if( !(m_free_ranges.m_bits[ p ] & offset_mask) ) {
                ++p;
                ++level;
                if( !(m_free_ranges.m_bits[ p ] & offset_mask ) ) {
//                    std::cerr << "No free positions in range blocks: " << (p - 1)  << " or " << p << " [" << std::hex << offset_mask << "]" << std::endl;
//                    std::cerr << "Free Ranges [" << m_free_ranges.size() << "]:\n" << m_free_ranges << std::endl;
//                    std::cerr << "Free List [" << m_free_list.size() << "]:\n" << m_free_list << std::endl;
                    assert(false);
                }
            }

            path = p;
//            std::cerr << "Next Path: " << path << std::endl;
            level <<= 1;
        }
        size_t range_idx = (level - m_free_ranges.num_blocks());

//        std::cerr << "Checking free list: " << (range_idx) << " [" << m_free_list.num_blocks() << "]" << std::endl;
        assert( (range_idx ) < m_free_list.num_blocks() );
        if( m_free_list.m_bits[ range_idx ] & offset_mask ) {
            _offset += (range_idx ) * bits_per_range;
//            std::cerr << "Returning free index: " << _offset << " = (" << range_idx << ") * " << bits_per_range << std::endl;
        } else {
            ++range_idx;
//            std::cerr << "Checking free list: " << range_idx << " [" << m_free_list.num_blocks() << "]" << std::endl;
            assert( range_idx < m_free_list.num_blocks() );
            assert( m_free_list.m_bits[ range_idx ] & offset_mask );
            _offset += range_idx * bits_per_range;
//            std::cerr << "Returning free index: " << _offset << " = (" << range_idx << ") * " << bits_per_range << std::endl;
        }
    } else {
        _offset = npos;
    }

    return _offset;
}

void SortedAlleleAlphabet2::buildFreeRanges(  ) {
    m_free_ranges.reset();

    if( m_db.size() == 0 ) return;


    if( m_free_list.num_blocks() == 0 ) {
        return;
    }

    m_free_list.resize( m_db.size(), false );

    size_t inode_count = 1;
    size_t bcount = (m_db.size() / bits_per_range) + 1;

    while( inode_count < bcount ) {
        inode_count <<= 1;
    }

    m_free_ranges.resize( inode_count * bits_per_range, false );

    if( m_free_ranges.num_blocks() < m_free_list.num_blocks() ) {
//        std::cerr << "Insufficient number of blocks in the free range lookup table: " << m_free_ranges.num_blocks() << " < " << m_free_list.num_blocks() << std::endl;
        assert( false );
    }

    if( m_free_list.num_blocks() == 1 ) {
        m_free_ranges.m_bits[ 0 ] = m_free_list.m_bits[0];
        return;
    } else if( m_free_list.num_blocks() == 2 ) {
        m_free_ranges.m_bits[ 0 ] = (m_free_list.m_bits[0] | m_free_list.m_bits[1]);
        return;
    }

    size_t b_idx = m_free_list.num_blocks() - 1;
    size_t r_idx = ((b_idx + m_free_ranges.num_blocks()) / 2) - 1;

    if( !( b_idx & 1 ) ) {
        // even index; (odd number of blocks)
        m_free_ranges.m_bits[r_idx--] = m_free_list.m_bits[ b_idx-- ];
    }

    while( b_idx < m_free_list.size() ) {
        m_free_ranges.m_bits[ r_idx ] = m_free_list.m_bits[ b_idx-- ];
        m_free_ranges.m_bits[ r_idx-- ] |= m_free_list.m_bits[ b_idx-- ];
    }

    // assumes free_ranges is always a balanced tree
    b_idx = m_free_ranges.num_blocks() - 2;
    while( r_idx < m_free_ranges.size() ) {
        m_free_ranges.m_bits[ r_idx ] = m_free_ranges.m_bits[b_idx--];
        m_free_ranges.m_bits[ r_idx-- ] |= m_free_ranges.m_bits[b_idx--];
    }

//    std::cerr << "Free Ranges [" << m_free_ranges.size() << "]:\n" << m_free_ranges << std::endl;
//    std::cerr << "Free List [" << m_free_list.size() << "]:\n" << m_free_list << std::endl;
}

/*
typename SortedAlleleAlphabet2::index_type SortedAlleleAlphabet2::find( const locus_t & l ) {
    index_type offset = l * bits_per_range;
    locus_iterator it = m_db.begin() + offset;

    bool found = false;
    while( offset < m_db.size() ) {
        if( it->first == l ) {
            found = true;
            break;
        }
        offset += bits_per_range;
        it += bits_per_range;
    }

    if( !found ) {
        offset = npos;
    }

    return offset;
}
*/

typename SortedAlleleAlphabet2::index_type SortedAlleleAlphabet2::find( const locus_t & l ) {
    locus_lookup_map::iterator it = m_lookup_locus.find(l);

    return ((it == m_lookup_locus.end() ) ? npos : it->second);
}

void SortedAlleleAlphabet2::logState( boost::property_tree::ptree & log ) {
    std::string key = "symbol_database";
    log.put( key + ".count", m_db.size() );
    log.put( key + ".active_count", m_db.size() - m_free_list.count());
    log.put( key + ".symbol_per_block", bitset_type::bits_per_block );
    log.put( key + ".max_block_per_region", (m_db.size() / bitset_type::bits_per_block) + 1 );

    std::ostringstream oss;

    for( unsigned int i = 0; i < m_db.size(); ++i ) {
        if( !m_free_list[i] ) {
            oss.str("");
            oss.clear();
            oss << ".symbol." << i;
            log.put( key + oss.str(), m_db[i].first );
        }
    }

    variant_db_t::iterator first = m_db.begin();

    /*
        free_ranges::iterator it = m_free_ranges.begin();
        boost::property_tree::ptree fr;
        while( it != m_free_ranges.end() ) {
            boost::property_tree::ptree p, q, r, s, t;
            p.put("", it->first.lower() );
            q.put("", it->first.upper() );
            r.push_back( std::make_pair("", p));
            r.push_back( std::make_pair("", q));

            index_interval_type::iterator idx_it = it->second.begin();
            while( idx_it != it->second.end() ) {
                boost::property_tree::ptree _p, _q, _s;
                _p.put("", idx_it->lower() );
                _q.put("", idx_it->upper() );

                _s.push_back( std::make_pair( "", _p ) );
                _s.push_back( std::make_pair( "", _q ) );
                s.push_back( std::make_pair("", _s));
                ++idx_it;
            }

            t.push_back( std::make_pair( "", r ) );
            t.push_back( std::make_pair( "", s ) );
            fr.push_back( std::make_pair( "", t ));
            ++it;
        }

        log.add_child( key + ".free_ranges", fr );
    */
}

SortedAlleleAlphabet2::~SortedAlleleAlphabet2() {}
