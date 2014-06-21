#include "genetics/population_alphabet.h"

#include <cassert>

PopulationAlphabet::variant_db_t PopulationAlphabet::m_db;
PopulationAlphabet::bitset_type  PopulationAlphabet::m_free_list;

PopulationAlphabet::index_t     PopulationAlphabet::addSymbol( const locus_t & l, const allele_t & a ) {
    locus_alleles_t res = m_db.left.equal_range( l );
    variant_db_t::iterator it = m_db.end();

    while( res.first != res.second ) {
        if( res.first->second == a ) {
            it = m_db.project_up( res.first );
        } else {
            res.first++;
        }
    }

    if( it == m_db.end() ) {
        std::pair< variant_db_t::iterator, bool > pbres = m_db.push_back( edge_type(l, a) );
        assert( pbres.second );
        it = pbres.first;
    }

    size_t offset = (it - m_db.begin());
    if( m_free_list.size() <= offset ) {
        size_t fspace = m_free_list.find_first();

        if( fspace == npos ) {
            fspace = m_free_list.size();
            m_free_list.push_back(false);
        }

        if( fspace != offset ) {
            variant_db_t::iterator sit = m_db[fspace];
            variant_db_t::iterator nit = it + 1;
            m_db.relocate( sit, it);
            m_db.relocate( nit, sit);
        }
    } else if( m_free_list[offset] ){
        m_free_list[offset] = false;
    }
}

PopulationAlphabet::symbol_t    PopulationAlphabet::getAlternate( const locus_t & l, const allele_t & a ) {

    

}

PopulationAlphabet::symbol_t PopulationAlphabet::operator[]( size_t idx ) {
    assert( idx < m_alphabet->size() );
    return m_alphabet[idx];
}

PopulationAlphabet::allele_t * PopulationAlphabet::getAllele( size_t idx ) {
    assert( idx < m_alphabet->size() );
    return m_db.getAlleleBySymbol( m_alphabet->at(idx) );
}

PopulationAlphabet::locus_t * PopulationAlphabet::getLocus( size_t idx ) {
    assert( idx < m_alphabet.size() );
    return m_db.getLocusBySymbol( m_alphabet[idx] );
}

size_t PopulationAlphabet::indexOf( symbol_t sym ) {
    size_t i = 0;
    for( population_alphabet_t::iterator it = m_alphabet.begin(); it != m_alphabet.end(); it++ ) {
        if( *it == sym ) break;
        ++i;
    }
    return i;
}
