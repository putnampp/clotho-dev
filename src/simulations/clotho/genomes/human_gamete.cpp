#include "human_gamete.h"

#include <cassert>
#include <cstring>

const gamete::gamete_source_t HumanGamete::FROM_MOTHER = 0;
const gamete::gamete_source_t HumanGamete::FROM_FATHER = 1;

const gamete::gamete_type_t HumanGamete::X_TYPE = 0;
const gamete::gamete_type_t HumanGamete::Y_TYPE = 1;

HumanGamete::HumanGamete( gamete::gamete_source_t s, gamete::gamete_type_t t, size_t n) :
    gamete(s , t),
    m_nAlleles( n ),
    m_alleles( new unsigned char[ m_nAlleles ] ) {
    memset( m_alleles, 0, sizeof( unsigned char ) * m_nAlleles );
}

HumanGamete::HumanGamete( const HumanGamete & z ) :
    gamete( z ),
    m_nAlleles( z.m_nAlleles ),
    m_alleles( new unsigned char[ m_nAlleles ] ) {
    memcpy( z.m_alleles, m_alleles, sizeof( unsigned char ) * m_nAlleles );
}

gamete * HumanGamete::clone() const {
    HumanGamete * hz = new HumanGamete( *this );
    return hz;
}

chromosome * HumanGamete::getChromosome( chromosome::chrom_id_t id ) {
    return NULL;
}

void HumanGamete::setChromosome( chromosome * c ) {
    assert(false);
}

void HumanGamete::print( std::ostream & o ) {
    o << "{ ";

    if( getSource() == FROM_MOTHER ) {
        o << "Maternal";
    } else if( getSource() == FROM_FATHER ) {
        o << "Paternal";
    }

    o << ", ";

    if( getType() == X_TYPE ) {
        o << "X";
    } else if( getType() == Y_TYPE ) {
        o << "Y";
    }

    o << "}";
}

HumanGamete::~HumanGamete() {
    if( m_alleles != NULL )
        delete [] m_alleles;
}
