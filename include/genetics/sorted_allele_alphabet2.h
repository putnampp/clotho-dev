#ifndef SORTED_ALLELE_ALPHABET_2H_
#define SORTED_ALLELE_ALPHABET_2H_

#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#include <vector>
#include <unordered_map>

#include "locus.h"
#include "allele.h"

#include <boost/dynamic_bitset.hpp>
#include <boost/property_tree/ptree.hpp>

//#include <boost/icl/interval.hpp>
//#include <boost/icl/interval_map.hpp>
//#include <boost/icl/interval_set.hpp>

#include <memory>

#include "symbol_generator.hpp"

class SortedAlleleAlphabet2 {
public:
    typedef Locus           locus_t;
    typedef allele          allele_t;

    typedef std::pair< locus_t, allele_t > symbol_type;


    typedef std::vector< symbol_type >          variant_db_t;
    typedef variant_db_t::size_type             index_type;
    typedef variant_db_t::iterator              vertex_type;
    typedef variant_db_t::iterator              locus_iterator;
    typedef variant_db_t::const_iterator        clocus_iterator;
    typedef variant_db_t::iterator              active_iterator;

    typedef std::unordered_map< locus_t, index_type > locus_lookup_map;

    typedef unsigned long                       block_type;
    typedef boost::dynamic_bitset< block_type > bitset_type;
    typedef bitset_type                         edge_set_type;

    typedef std::pair< vertex_type, vertex_type >   symbol_range;

    typedef std::shared_ptr< SortedAlleleAlphabet2 >   pointer;

    friend class symbol_generator< locus_t, allele_t, index_type, SortedAlleleAlphabet2 >;

    struct comp_first {
        bool operator()( const symbol_type & lhs, const symbol_type & rhs ) const {
            return lhs.first <= rhs.first;
        }
    };

    template < class RecordIterator, class Result >
    static Result get( RecordIterator it );

    SortedAlleleAlphabet2();

    static const index_type  npos = bitset_type::npos;
    //static const index_type  bits_per_range = 1024;
    //static const index_type  bits_per_range = 512;
    static const index_type  bits_per_range = 64;
    static const index_type  blocks_per_range = (bits_per_range / (sizeof(block_type)*8));
    constexpr static const double      locus_offset = ((1.0) / (double)(bits_per_range));

    static pointer getInstance() {
        static pointer instance(new SortedAlleleAlphabet2());
        return instance;
    }

    index_type  getSymbol( const locus_t & l, const allele_t & a, bool allowDuplicates = false );

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

    active_iterator active_begin()  {
        return m_db.begin();
    }

    active_iterator active_end()    {
        return m_db.end();
    }

    size_t  active_count() const {
        return m_db.size() - m_free_list.count();
    }
    size_t  fixed_lost_count() const {
        return m_free_list.count();
    }

    size_t database_size() const {
        return m_db.size();
    }

    bool checkFreeStatus( index_type idx ) {
        return !m_free_list[idx];
    }

    bitset_type * getFreeMask() {
        return &m_free_mask;
    }

    virtual ~SortedAlleleAlphabet2();
protected:
    index_type      find( const locus_t & l );

    index_type      appendSymbol( const locus_t & l, const allele_t & a );
    void            updateSymbolAt( index_type & idx, const locus_t & l, const allele_t & a );

    void            updateFreeIndex( index_type idx, bool isFree = false );

    index_type      findFreeRangeContaining( const locus_t & l );
    void            buildFreeRanges( );

    variant_db_t    m_db;
    locus_lookup_map    m_lookup_locus;

    bitset_type     m_free_list, m_free_intersect, m_free_union;
    bitset_type     m_free_mask;
    bitset_type     m_free_ranges;

public:
    static const  block_type  low_order_bit_masks[ bits_per_range ];
    static const  block_type  bit_position_masks[ bits_per_range ];
};

#include "accessor.hpp"

namespace accessor {

template < >
typename SortedAlleleAlphabet2::allele_t get< typename SortedAlleleAlphabet2::active_iterator, typename SortedAlleleAlphabet2::allele_t >( SortedAlleleAlphabet2::active_iterator it ) {
    return it->second;
}

template < >
typename SortedAlleleAlphabet2::locus_t get< typename SortedAlleleAlphabet2::active_iterator, typename SortedAlleleAlphabet2::locus_t >( SortedAlleleAlphabet2::active_iterator it ) {
    return it->first;
}
}   // namesapce accessor

#undef BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#endif  // SORTED_ALLELE_ALPHABET_2H_
