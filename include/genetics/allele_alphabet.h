#ifndef ALLELE_ALPHABET_H_
#define ALLELE_ALPHABET_H_

#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#include <vector>
#include <map>
#include <set>

#include "locus.h"
#include "allele.h"

#include <boost/dynamic_bitset.hpp>

#include <memory>

//#include "adjacency_iterator.hpp"
#include "symbol_generator.hpp"

class AlleleAlphabet {
public:
    typedef Locus           locus_t;
    typedef allele          allele_t;

//    typedef std::pair< locus_t, allele_t > symbol_type;

    // using set and multimap (log(size) lookup) b/c unordered version
    // will invalid iterators upon rehash (resize)
    typedef std::size_t                         index_type;
    typedef std::set< allele_t >                alleles_type;
    typedef alleles_type::iterator              allele_iterator;

    typedef std::pair< locus_t, std::pair< allele_iterator, index_type> >       symbol_type;
    typedef std::multimap< locus_t, std::pair< allele_iterator, index_type> >   variant_db_t;
    typedef variant_db_t::iterator                                              vertex_type;
    typedef variant_db_t::iterator                                              locus_iterator;
    typedef variant_db_t::const_iterator                                        clocus_iterator;
    typedef std::vector< locus_iterator >                                       active_database_type;
    typedef typename active_database_type::iterator                             active_iterator;

    typedef unsigned long                       block_type;
    typedef boost::dynamic_bitset< block_type > bitset_type;
    typedef bitset_type                         edge_set_type;

    typedef std::pair< vertex_type, vertex_type >   symbol_range;

    template < class RecordIterator, class Result >
    static Result get( RecordIterator it );

//    typedef AdjacencyIterator< active_database_type, edge_set_type > adjacency_iterator;

    typedef std::shared_ptr< AlleleAlphabet >   pointer;

    friend class symbol_generator< locus_t, allele_t, index_type, AlleleAlphabet >;

    AlleleAlphabet();

    static const index_type  npos = bitset_type::npos;

    static pointer getInstance() {
        static pointer instance(new AlleleAlphabet());
        return instance;
    }

    index_type  getSymbol( const locus_t & l, const allele_t & a, bool createNew = true );
    void     updateFreeSymbols( const bitset_type & fs );
    void     resetFreeSymbols( );

    void    setState();
    size_t  block_count();

    size_t  size();
    size_t  active_size();

    bool isLocus( locus_t & l ) const;

    locus_iterator   operator[]( index_type idx );
    inline index_type operator[]( const locus_iterator & sym ) {
        return sym->second.second;
    }

//    adjacency_iterator begin( edge_set_type * es );
//    adjacency_iterator end( edge_set_type * es );

    active_iterator active_begin()  {
        return m_active.begin();
    }
    active_iterator active_end()    {
        return m_active.end();
    }

    alleles_type::iterator end_allele() {
        return m_alleles.end();
    }

    locus_iterator          end_db() {
        return m_db.end();
    }

    size_t  active_count() const {
        return m_active.size() - m_free_list.count();
    }
    size_t  fixed_lost_count() const {
        return m_free_list.count();
    }

    bitset_type * getFreeMask() {
        return &m_free_mask;
    }

    symbol_range findAllSymbols( locus_t min, locus_t max ) {
        vertex_type min_it = m_db.lower_bound( min );
        vertex_type max_it = m_db.upper_bound( max );

        return std::make_pair( min_it, max_it );
    }

    virtual ~AlleleAlphabet();
protected:
    alleles_type    m_alleles;
    variant_db_t    m_db;

    active_database_type    m_active;

    bitset_type     m_free_list, m_free_intersect, m_free_union;
    bitset_type     m_free_mask;
    bitset_type::size_type  m_next_free;
};

#include "accessor.hpp"

namespace accessor {

template <>
typename AlleleAlphabet::allele_t get< typename AlleleAlphabet::locus_iterator, typename AlleleAlphabet::allele_t>( AlleleAlphabet::locus_iterator it ) {
    return *(it->second.first);
}

template <>
typename AlleleAlphabet::allele_t get< typename AlleleAlphabet::active_iterator, typename AlleleAlphabet::allele_t>( AlleleAlphabet::active_iterator it ) {
    return *((*it)->second.first);
}

template <>
typename AlleleAlphabet::locus_t get< typename AlleleAlphabet::active_iterator, typename AlleleAlphabet::locus_t>( AlleleAlphabet::active_iterator it ) {
    return (*it)->first;
}

template <>
typename AlleleAlphabet::locus_t get< typename AlleleAlphabet::locus_iterator, typename AlleleAlphabet::locus_t>( AlleleAlphabet::locus_iterator it ) {
    return it->first;
}

}   // namespace accessor

#undef BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#endif  // ALLELE_ALPHABET_H_
