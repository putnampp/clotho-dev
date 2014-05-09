#include "variant_base.h"

variant_base::variant_base( const key_t & k, double cSelection, double cDominance, variant_flags f ) :
    genetic_region( k ),
    m_penetrance(0),
    m_coeff_selection( cSelection ),
    m_coeff_dominance( cDominance ),
    m_flags(f)
{}

variant_base::variant_base( const chromosome_t & c, pos_t s, double cSelection, double cDominance, variant_flags f ) : 
    genetic_region(c, s),
    m_penetrance(0),
    m_coeff_selection(cSelection),
    m_coeff_dominance(cDominance),
    m_flags( f ) 
{}

bool variant_base::isNeutral() const { return check_flag( NEUTRAL ); }

unsigned int variant_base::getPenetrance() const { return m_penetrance; }

double variant_base::getSelection() const { return m_coeff_selection; }

double variant_base::getDominance() const { return m_coeff_dominance; }

void variant_base::incrementPenetrance() {
    ++m_penetrance;
}

void variant_base::decrementPenetrance() {
    assert( m_penetrance > 0);
    --m_penetrance;
}

void variant_base::print( std::ostream & out ) {
    out << "[ " <<  m_key 
        << ", " << m_coeff_selection 
        << ", " << m_coeff_dominance
        << ", " << m_penetrance
        << "]\n";
}

variant_base::~variant_base() {}
