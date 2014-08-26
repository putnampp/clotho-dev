#include "genetics/sorted_allele_alphabet.h"
#include <iostream>

#include <cassert>

#include <sstream>
#include <algorithm>

#include "utility/bitset_ops.hpp"

SortedAlleleAlphabet::SortedAlleleAlphabet()  {}

SortedAlleleAlphabet::index_type     SortedAlleleAlphabet::getSymbol( const locus_t & l, const allele_t & a, bool createNew ) {
    index_type lower = 0, upper = 0;
    index_type idx = findIndex( l, createNew, lower, upper );

    if( idx == npos ) {
        return npos;
    }

    assert( idx < m_db.size() );

    m_db[idx] = std::make_pair( l, a );

//    index_type i = lower;
//    locus_t tmp_l = (( (lower - 1) % bits_per_range == 0 ) ? 0.0 : m_db[ lower - 1].first);
//    while( i < idx ) {
//        m_db[i++].first = tmp_l;
//    }
//    ++i;
//    while( i < upper ) {
//        m_db[i++].first = l;
//    }

    return idx;
}

void SortedAlleleAlphabet::updateFreeSymbols( const bitset_type & fs ) {
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
            (*int_it++) &= (*first);
            (*un_it++) |= (*first++);
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

void SortedAlleleAlphabet::setState() {
    m_free_list = m_free_intersect | ~m_free_union;
    m_free_mask = ~m_free_list;

    m_free_ranges.clear();
    buildFreeRanges();
}

void SortedAlleleAlphabet::resetFreeSymbols() {
    if( m_free_union.size() < m_db.size() ) {
        m_free_union.resize( m_db.size(), false);
        m_free_intersect.resize( m_db.size(), false);
    }
    m_free_union.reset();
    m_free_intersect.reset().flip();

    m_free_ranges.clear();
}

size_t SortedAlleleAlphabet::block_count() {
    return m_free_list.num_blocks();
}

SortedAlleleAlphabet::locus_iterator SortedAlleleAlphabet::operator[]( index_type idx ) {
    if( idx >= m_db.size() ) {
        std::cerr << "requesting an index: " << idx << " [" << m_db.size() << "]" << std::endl;
    }
    assert( idx < m_db.size() );
    return m_db.begin() + idx;
}

size_t SortedAlleleAlphabet::size() {
    return m_db.size();
}

size_t SortedAlleleAlphabet::active_size() {
    return m_db.size() - m_free_list.count();
}

bool SortedAlleleAlphabet::isLocus( locus_t & l ) const {
    //clocus_iterator res = m_db.find(l);
    //return res != m_db.end();
    return false;
}

//SortedAlleleAlphabet::adjacency_iterator SortedAlleleAlphabet::begin( edge_set_type * es ) {
//    return adjacency_iterator( &m_active, es, 0 );
//}

//SortedAlleleAlphabet::adjacency_iterator SortedAlleleAlphabet::end( edge_set_type * es ) {
//    return adjacency_iterator( &m_active, es );
//}
//

typename SortedAlleleAlphabet::free_ranges::const_iterator SortedAlleleAlphabet::findFreeRangeContaining( const locus_t & l ) {
//    free_range_type fr = std::make_pair( std::make_pair( l, l ), std::make_pair(0,0));
//    return m_free_ranges.find( fr );
//
//    SortedAlleleAlphabet::free_ranges::iterator it = m_free_ranges.begin();
//
//    while( it != m_free_ranges.end() ) {
//        if( it->first.first <= l && l <= it->first.second ) {
//            break;
//        }
//        ++it;
//    }

    double block_offset = l;
    size_t s = m_db.size() / bits_per_range;
    free_ranges::const_iterator it = m_free_ranges.end();
    while( s-- ) {
        it = m_free_ranges.find( block_offset );
        if( it != m_free_ranges.end() ) {
            break;
        }

        block_offset += 1.0;
    }

    return it;
}

SortedAlleleAlphabet::index_type SortedAlleleAlphabet::findIndex( const locus_t & l, bool createNew, index_type & lower, index_type & upper ) {
//    if( m_db.size() > 0 ) {
//        std::cerr << "Attempting to find Index for " << l << " in:\n" << m_free_ranges << std::endl;
//    }
    index_type pos = npos;

    free_ranges::const_iterator it = findFreeRangeContaining( l );

    locus_t loc_lo = 0.0, loc_hi = 1.0;
    index_type idx_lo = 0, idx_hi = m_db.size();

    if( it != m_free_ranges.end() ) {

        loc_lo = it->first.lower();
        loc_hi = it->first.upper();

        idx_lo = it->second.begin()->lower();
        idx_hi = it->second.begin()->upper();

        m_free_ranges -= *it;
    } else if( !createNew ) {
        // there are no free indices available for locus
        // and we are not supposed to add one
        return npos;
    } else {
        idx_lo = m_db.size();
        idx_hi = idx_lo + bits_per_range;

        loc_lo = (locus_t)(idx_lo / bits_per_range);
        loc_hi = loc_lo + 1.0;

        m_db.reserve( idx_hi );
        for( locus_t i = 0.0; i < 1.0; i += locus_offset) {
            m_db.push_back( std::make_pair( i, allele_t()) );
        }
        assert( m_db.size() == idx_hi);
    }

    index_type roffset = (idx_lo / bits_per_range);
    size_t idx_count = idx_hi - idx_lo;
    pos = idx_lo;

    locus_t _l = l + (locus_t)(roffset);

    if( idx_count > 1 ) {
        locus_t dist_to_lo = (_l - loc_lo),
                lrange = (loc_hi - loc_lo);
        index_type offset = ((dist_to_lo/lrange) * (idx_count));

        pos += offset;
    }

    if( pos - idx_lo != 0 ) {
        locus_range_type lo_val( loc_lo, _l, bound_type::open() );
        index_interval_type lo_range;
        lo_range.insert( index_range_type( idx_lo, pos, bound_type::right_open()));
        m_free_ranges += std::make_pair( lo_val, lo_range );
    }

    if( pos + 1 != idx_hi ) {
        locus_range_type hi_val( _l, loc_hi, bound_type::open() );
        index_interval_type hi_range;
        hi_range.insert( index_range_type( pos + 1, idx_hi, bound_type::right_open()));

        m_free_ranges += std::make_pair( hi_val, hi_range );
    }

    lower = idx_lo;
    upper = idx_hi;

    return pos;
}

void SortedAlleleAlphabet::buildFreeRanges( index_type pos ) {
    assert( pos == npos || pos < m_free_list.size() );

    if( m_free_list.empty() ) return;

    if( pos == npos ) {
        pos = m_free_list.find_first();
    }


    while( pos != npos ) {
        index_type next_pos = m_free_mask.find_next(pos);

        if( next_pos == npos ) {
            next_pos = m_db.size();
        }

        index_type lrange = (pos / bits_per_range);
        index_type hrange = (next_pos / bits_per_range);

        locus_t lo = ((pos > 0) ? m_db[pos - 1].first: 0.0) + (locus_t)(lrange);
        locus_t hi = ((next_pos == m_db.size()) ? 1.0 : m_db[ next_pos ].first) + (locus_t)(hrange);

        if( lrange != hrange ) {
            index_interval_type lo_int;
            ++lrange;
            lo_int.insert( index_range_type(pos, lrange * bits_per_range, bound_type::right_open()));
            m_free_ranges += std::make_pair( locus_range_type(lo, (locus_t)lrange, bound_type::open()), lo_int);
            lo = (locus_t)lrange;

            while( lrange < hrange ) {
                index_interval_type _int;
                unsigned int lbound = (lrange++) * bits_per_range;
                _int.insert( index_range_type( lbound, lbound + bits_per_range, bound_type::right_open()) );
                locus_t tmp_hi = lo + 1.0;
                m_free_ranges += std::make_pair( locus_range_type( lo, tmp_hi, bound_type::open()), _int);
                lo = tmp_hi;
            }
            if( (lrange * bits_per_range) < next_pos ) {
                index_interval_type _int;
                unsigned int lbound = (lrange++) * bits_per_range;
                _int.insert( index_range_type( lbound, next_pos, bound_type::right_open()) );
                m_free_ranges += std::make_pair( locus_range_type( lo, hi, bound_type::open()), _int);
            }
        } else {
            index_interval_type _int;
            _int.insert( index_range_type( pos, next_pos, bound_type::right_open()) );
            m_free_ranges += std::make_pair( locus_range_type( lo, hi, bound_type::open()), _int);
        }

        pos = m_free_list.find_next( next_pos );
    }
}

void SortedAlleleAlphabet::logState( boost::property_tree::ptree & log ) {
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
}

SortedAlleleAlphabet::~SortedAlleleAlphabet() {}
