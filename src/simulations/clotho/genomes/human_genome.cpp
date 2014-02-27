#include "human_genome.h"

HumanGenome::HumanGenome() :
    m_mother( new HumanZygote(HumanZygote::FROM_MOTHER, HumanZygote::X_TYPE) ),
    m_father( new HumanZygote(HumanZygote::FROM_FATHER, HumanZygote::X_TYPE) ),
    m_sex( FEMALE )
{}

HumanGenome::HumanGenome( HumanZygote * hz0, HumanZygote * hz1 ) {
    if( hz0->getSource() == HumanZygote::FROM_MOTHER ) {
        assert( hz0->getType() == HumanZygote::X_TYPE);
        assert( hz1->getSource() == HumanZygote::FROM_FATHER);
        m_mother = hz0;
        m_father = hz1;
    } else if( hz1->getSource() == HumanZygote::FROM_MOTHER ) {
        assert( hz1->getType() == HumanZygote::X_TYPE);
        assert( hz0->getSource() == HumanZygote::FROM_FATHER);
        m_mother = hz1;
        m_father = hz0;
    } else {
        // either received two female or two male zygotes
        // this should never occur
        assert( false );
    }

    m_sex = ((m_father->getType() == HumanZygote::Y_TYPE) ? MALE : FEMALE );
}

HumanGenome::HumanGenome( const HumanGenome & g ) :
    m_mother( NULL ),
    m_father( NULL ),
    m_sex( g.m_sex )
{
    setZygote( g.m_mother->clone() );
    setZygote( g.m_father->clone() );
}

genome * HumanGenome::clone() const {
    return new HumanGenome( *this );
}

zygote * HumanGenome::getZygote( zygote::zygote_source_t id ) const {
    return ( (id) ? m_father : m_mother );
}

zygote * HumanGenome::getZygote( Sex s ) const {
    return ( (s == FEMALE) ? m_mother : m_father );
}

void HumanGenome::setZygote( zygote * z ) {
    HumanZygote * hz = dynamic_cast< HumanZygote * >( z );
    assert( hz != NULL );
    setZygote( hz );
}

void HumanGenome::setZygote( HumanZygote * hz ) {
    if( hz->getSource() == HumanZygote::FROM_MOTHER ) {
        assert( hz->getType() == HumanZygote::X_TYPE);
        m_mother = hz;
    } else if( hz->getSource() == HumanZygote::FROM_FATHER ) {
//        assert( z->getType() == HumanZygote::Y_TYPE || z->getType() == HumanZygote::X_TYPE );
        m_father = hz;
        m_sex = ((m_father->getType() == HumanZygote::Y_TYPE) ? MALE : FEMALE);
    } else {
        assert(false);
    }
}

HumanGenome::~HumanGenome() {
    if(m_mother) delete m_mother;
    if(m_father) delete m_father;
}
