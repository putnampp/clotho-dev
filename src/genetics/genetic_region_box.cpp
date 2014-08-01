#include "genetic_region.h"

genetic_region::genetic_region( const chromosome_t & c, pos_t s ) : genetic_region(c, s, s) {
}

genetic_region::genetic_region( const chromosome_t & c, pos_t s, pos_t e ) :
    m_region( point_t( genetic_mapping::getChromosomeID(c), s),
              point_t( genetic_mapping::getChromosomeID(c), e ) ) {
}

genetic_region::region_t genetic_region::getRegion() const {
    return m_region;
}

bool genetic_region::operator==( const genetic_region & rhs ) const {
    // note: genetic regions can only occur on the same chromosome

    return (m_region.min_corner().get<0>() == rhs.m_region.min_corner().get<0>())
           && (m_region.min_corner().get<1>() == rhs.m_region.min_corner().get<1>())
//            && (m_region.max_corner().get<0>() == rhs.m_region.max_corner().get<0>() )
           && (m_region.max_corner().get<1>() == rhs.m_region.max_corner().get<1>());
}

genetic_region::~genetic_region() {}
