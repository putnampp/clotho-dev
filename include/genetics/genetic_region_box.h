#ifndef GENETIC_REGION_H_
#define GENETIC_REGION_H_

#include "genetic_mapping.h"

#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>

namespace bg = boost::geometry;

class genetic_region : public genetic_mapping {
public:
    typedef pos_t  coord_t;
    typedef bg::model::point< coord_t, 2, bg::cs::cartesian > point_t;
    typedef bg::model::box< point_t > region_t;

    genetic_region( const chromosome_t & c, pos_t s );
    genetic_region( const chromosome_t & c, pos_t s, pos_t e );

    region_t getRegion() const;

    virtual bool operator==( const genetic_region & rhs ) const;

    virtual ~genetic_region();
protected:
    region_t    m_region;
};
#endif  // GENETIC_REGION_H_

