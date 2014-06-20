#include "genetics/population_alphabet.h"

#include <cassert>

PopulationAlphabet::symbol_t PopulationAlphabet::operator[]( size_t idx ) {
    assert( idx < m_alphabet.size() );
    return m_alphabet[idx];
}

PopulationAlphabet::allele_t * PopulationAlphabet::getAllele( size_t idx ) {
    assert( idx < m_alphabet.size() );
    return m_db.getAlleleBySymbol( m_alphabet[idx] );
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
