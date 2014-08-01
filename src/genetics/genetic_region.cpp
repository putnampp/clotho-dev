#include "genetics/genetic_region.h"

#include <limits>
#include <cmath>

genetic_region::genetic_region( key_t  k ) :
    m_key( k ) {
}

genetic_region::genetic_region( const chromosome_t & c, pos_t s ) : genetic_region(genetic_mapping::convertToKey< key_t >( c, s)) {
}

genetic_region::key_t genetic_region::getKey() const {
    return m_key;
}

bool genetic_region::operator==( const genetic_region & rhs ) const {
    return std::fabs(m_key - rhs.m_key) <= std::numeric_limits<double>::epsilon();
}

bool genetic_region::operator<( const genetic_region & rhs ) const {
    return m_key < rhs.m_key;
}

genetic_region::~genetic_region() {}
