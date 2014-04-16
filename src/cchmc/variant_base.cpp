#include "variant_base.h"

variant_base::variant_base( const chromosome_t & c, pos_t s, double cSelection, double cDominance, variant_flags f ) : 
    genetic_region( c, s, s), 
    m_penetrance(1),
    m_coeff_selection(cSelection),
    m_coeff_dominance(cDominance),
    m_flags( f ) 
{}

variant_base::variant_base( const chromosome_t & c, pos_t s, pos_t e, double cSelection, double cDominance, variant_flags f ) : 
    genetic_region( c, s, e), 
    m_penetrance(1),
    m_coeff_selection(cSelection),
    m_coeff_dominance(cDominance),
    m_flags( f ) 
{}

bool variant_base::isNeutral() const { return check_flag( NEUTRAL ); }

unsigned int variant_base::getPenetrance() const { return m_penetrance; }

double variant_base::getSelection() const { return m_coeff_selection; }

double variant_base::getDominance() const { return m_coeff_dominance; }

void variant_base::incrementPenetrance() {
    if( m_penetrance++ == 0) {
        ++m_reemergence;
    }
}

void variant_base::decrementPenetrance() {
    assert( m_penetrance > 0);
    --m_penetrance;
}

variant_base::~variant_base() {}
