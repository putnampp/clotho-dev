#include "human_genome.h"

HumanGenome::HumanGenome() :
    m_mother( new HumanGamete(HumanGamete::FROM_MOTHER, HumanGamete::X_TYPE, 1) ),
    m_father( new HumanGamete(HumanGamete::FROM_FATHER, HumanGamete::X_TYPE, 1) ),
    m_sex( FEMALE )
{}

HumanGenome::HumanGenome( HumanGamete * hz0, HumanGamete * hz1 ) {
    if( hz0 == NULL ) {
        m_sex = UNASSIGNED;
        if( hz1 == NULL || hz1->getSource() == HumanGamete::FROM_FATHER ) {
            m_mother = hz0;
            m_father = hz1;
        } else {
            assert( hz1->getSource() == HumanGamete::X_TYPE );
            m_mother = hz1;
            m_father = hz0;
        }
        return;
    } else if( hz1 == NULL ) {
        m_sex = UNASSIGNED;
        if( hz0->getSource() == HumanGamete::FROM_MOTHER ) {
            assert( hz0->getType() == HumanGamete::X_TYPE );
            m_mother = hz0;
            m_father = hz1;
        } else {
            m_mother = hz1;
            m_father = hz0;
        }
        return;
    } else if( hz0->getSource() == HumanGamete::FROM_MOTHER ) {
        assert( hz0->getType() == HumanGamete::X_TYPE);
        assert( hz1->getSource() == HumanGamete::FROM_FATHER);
        m_mother = hz0;
        m_father = hz1;
    } else if( hz1->getSource() == HumanGamete::FROM_MOTHER ) {
        assert( hz1->getType() == HumanGamete::X_TYPE);
        assert( hz0->getSource() == HumanGamete::FROM_FATHER);
        m_mother = hz1;
        m_father = hz0;
    } else {
        // either received two female or two male gametes
        // this should never occur
        assert( false );
    }

    m_sex = ((m_father->getType() == HumanGamete::Y_TYPE) ? MALE : FEMALE );
}

HumanGenome::HumanGenome( const HumanGenome & g ) :
    m_mother( dynamic_cast< HumanGamete * >(g.m_mother->clone()) ),
    m_father( dynamic_cast< HumanGamete * >(g.m_father->clone()) ),
    m_sex( g.m_sex )
{ }

genome * HumanGenome::clone() const {
    return new HumanGenome( *this );
}

Sex HumanGenome::getSex() const {
    return m_sex;
}

gamete * HumanGenome::getGamete( gamete::gamete_source_t id ) const {
/*    if( id == 0 ) {
        cout << "Maternal gamete";
    } else {
        cout << "Paternal gamete";
    }*/
    return ( (id) ? m_father : m_mother );
}

gamete * HumanGenome::getGamete( Sex s ) const {
    return ( (s == FEMALE) ? m_mother : m_father );
}

void HumanGenome::setGamete( gamete * z ) {
    HumanGamete * hz = dynamic_cast< HumanGamete * >( z );
    assert( hz != NULL );
    setGamete( hz );
}

void HumanGenome::setGamete( HumanGamete * hz ) {
    if( hz->getSource() == HumanGamete::FROM_MOTHER ) {
        assert( hz->getType() == HumanGamete::X_TYPE);
        if( m_mother ) {
            delete m_mother;
        }
        m_mother = hz;
    } else if( hz->getSource() == HumanGamete::FROM_FATHER ) {
        if( m_father ) {
            delete m_father;
        }
        m_father = hz;
        m_sex = ((m_father->getType() == HumanGamete::Y_TYPE) ? MALE : FEMALE);
    } else {
        assert(false);
    }
}

HumanGenome::~HumanGenome() {
    if(m_mother) delete m_mother;
    if(m_father) delete m_father;

    //while( !m_old_gametes.empty() ) {
    //    HumanGamete * hz = *m_old_gametes.begin();
    //    m_old_gametes.erase( m_old_gametes.begin() );
    //    if( hz != NULL )    // double delete occurring; why is human gamete existing in multiple genomes?
    //        delete hz;
    //}
}
