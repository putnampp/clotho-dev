#include "human_zygote.h"

#include <cassert>
#include <cstring>

const zygote::zygote_source_t HumanZygote::FROM_MOTHER = 0;
const zygote::zygote_source_t HumanZygote::FROM_FATHER = 1;

const zygote::zygote_type_t HumanZygote::X_TYPE = 0;
const zygote::zygote_type_t HumanZygote::Y_TYPE = 1;

HumanZygote::HumanZygote( zygote::zygote_source_t s, zygote::zygote_type_t t, size_t n) : 
    zygote(s , t), 
    m_nAlleles( n ),
    m_alleles( new unsigned char[ m_nAlleles ] )
{
    memset( m_alleles, 0, sizeof( unsigned char ) * m_nAlleles );
}

HumanZygote::HumanZygote( const HumanZygote & z ) : 
    zygote( z ),
    m_nAlleles( z.m_nAlleles ),
    m_alleles( new unsigned char[ m_nAlleles ] )
{
    memcpy( z.m_alleles, m_alleles, sizeof( unsigned char ) * m_nAlleles );
}

zygote * HumanZygote::clone() const {
    HumanZygote * hz = new HumanZygote( *this );
    return hz;
}

chromosome * HumanZygote::getChromosome( chromosome::chrom_id_t id ) {
    return NULL;
}

void HumanZygote::setChromosome( chromosome * c ) {
    assert(false);
}

HumanZygote::~HumanZygote() {
    if( m_alleles != NULL ) 
        delete [] m_alleles;
}
