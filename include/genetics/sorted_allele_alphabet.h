#ifndef SORTED_ALLELE_ALPHABET_H_
#define SORTED_ALLELE_ALPHABET_H_

#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#include <vector>
#include <map>
#include <set>

#include "locus.h"
#include "allele.h"

#include <boost/dynamic_bitset.hpp>
#include <boost/property_tree/ptree.hpp>

#include <boost/icl/interval.hpp>
#include <boost/icl/interval_map.hpp>
#include <boost/icl/interval_set.hpp>

#include <memory>

#include "symbol_generator.hpp"

class SortedAlleleAlphabet {
public:
    typedef Locus           locus_t;
    typedef allele          allele_t;

    typedef std::pair< locus_t, allele_t > symbol_type;

    typedef std::size_t                         index_type;

    typedef std::vector< symbol_type >          variant_db_t;
    typedef variant_db_t::iterator              vertex_type;
    typedef variant_db_t::iterator              locus_iterator;
    typedef variant_db_t::const_iterator        clocus_iterator;
    typedef variant_db_t::iterator              active_iterator;

    typedef boost::icl::interval< locus_t >::type           locus_range_type;
    typedef boost::icl::interval< index_type >::type        index_range_type;
    typedef boost::icl::interval_bounds                     bound_type;

    typedef boost::icl::interval_set< index_type >          index_interval_type;
    typedef boost::icl::interval_map< locus_t, index_interval_type >    free_ranges;

    typedef unsigned long                       block_type;
    typedef boost::dynamic_bitset< block_type > bitset_type;
    typedef bitset_type                         edge_set_type;

    typedef std::pair< vertex_type, vertex_type >   symbol_range;

    typedef std::shared_ptr< SortedAlleleAlphabet >   pointer;

    friend class symbol_generator< locus_t, allele_t, index_type, SortedAlleleAlphabet >;

    struct comp_first {
        bool operator()( const symbol_type & lhs, const symbol_type & rhs ) const {
            return lhs.first <= rhs.first;
        }
    };

    template < class RecordIterator, class Result >
    static Result get( RecordIterator it );

    SortedAlleleAlphabet();

    static const index_type  npos = bitset_type::npos;
    //static const index_type  bits_per_range = 1024;
    //static const index_type  bits_per_range = 512;
    static const index_type  bits_per_range = 64;
    static const index_type  blocks_per_range = (bits_per_range / (sizeof(block_type)*8));
    constexpr static const double      locus_offset = ((1.0) / (double)(bits_per_range));

    static pointer getInstance() {
        static pointer instance(new SortedAlleleAlphabet());
        return instance;
    }

    index_type  getSymbol( const locus_t & l, const allele_t & a, bool createNew = true );

    void     updateFreeSymbols( const bitset_type & fs );
    void     resetFreeSymbols( );

    void    setState();

    void    logState( boost::property_tree::ptree & log );
    size_t  block_count();

    size_t  size();
    size_t  active_size();

    bool isLocus( locus_t & l ) const;

    locus_iterator   operator[]( index_type idx );
    inline index_type operator[]( const locus_iterator & sym ) {
        return sym - m_db.begin();
    }

//    adjacency_iterator begin( edge_set_type * es );
//    adjacency_iterator end( edge_set_type * es );

    active_iterator active_begin()  {
        return m_db.begin();
    }
    active_iterator active_end()    {
        return m_db.end();
    }

    locus_iterator          end_db() {
        return m_db.end();
    }

    size_t  active_count() const {
        return m_db.size() - m_free_list.count();
    }
    size_t  fixed_lost_count() const {
        return m_free_list.count();
    }

    bitset_type * getFreeMask() {
        return &m_free_mask;
    }

//    symbol_range findAllSymbols( locus_t min, locus_t max ) {
//        vertex_type min_it = m_db.lower_bound( min );
//        vertex_type max_it = m_db.upper_bound( max );
//
//        return std::make_pair( min_it, max_it );
//    }

    virtual ~SortedAlleleAlphabet();
protected:

    index_type      findIndex( const locus_t & l, bool createNew, index_type & lower, index_type & upper );
    free_ranges::const_iterator findFreeRangeContaining( const locus_t & l );

    void            buildFreeRanges( index_type pos = npos );

    variant_db_t    m_db;

    free_ranges      m_free_ranges;

    bitset_type     m_free_list, m_free_intersect, m_free_union;
    bitset_type     m_free_mask;
};

#include "accessor.hpp"

namespace accessor {

template < >
typename SortedAlleleAlphabet::allele_t get< typename SortedAlleleAlphabet::active_iterator, typename SortedAlleleAlphabet::allele_t >( SortedAlleleAlphabet::active_iterator it ) {
    return it->second;
}

template < >
typename SortedAlleleAlphabet::locus_t get< typename SortedAlleleAlphabet::active_iterator, typename SortedAlleleAlphabet::locus_t >( SortedAlleleAlphabet::active_iterator it ) {
    return it->first;
}
}   // namesapce accessor

#undef BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#endif  // SORTED_ALLELE_ALPHABET_H_
