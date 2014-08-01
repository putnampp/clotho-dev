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

#include "adjacency_iterator.hpp"
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

    typedef unsigned long                       block_type;
    typedef boost::dynamic_bitset< block_type > bitset_type;
    typedef bitset_type                         edge_set_type;

    typedef AdjacencyIterator< active_database_type, edge_set_type > adjacency_iterator;

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
    size_t   block_count();

    size_t  size();
    size_t  active_size();

    bool isLocus( locus_t & l ) const;

    locus_iterator   operator[]( index_type idx );
    inline index_type operator[]( const locus_iterator & sym ) {
        return sym->second.second;
    }

    adjacency_iterator begin( edge_set_type * es );
    adjacency_iterator end( edge_set_type * es );

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

    virtual ~AlleleAlphabet();
protected:
    alleles_type    m_alleles;
    variant_db_t    m_db;

    active_database_type    m_active;

    bitset_type     m_free_list, m_free_intersect, m_free_union;
};

#undef BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#endif  // ALLELE_ALPHABET_H_
