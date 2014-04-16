#ifndef VARIANT_MAP_H_
#define VARIANT_MAP_H_

#include <type_traits>
#include <memory>
#include "variant_base.h"

#include <boost/geometry/index/rtree.hpp>

namespace bgi=boost::geometry::index;

template < class VT, class E = void >
class variant_map;

template < class VT >
class variant_map< VT, class = typename std::enable_if< std::is_base_of< variant_base, VT >::value >::type > {
public:
    typedef std::pair< VT::region_t, std::shared_ptr< VT > > value_t;
    typedef bgi::rtree< value_t, bgi::quadratic<16> > rtree_t;

    variant_map();

    bool is_known() const;

protected:
    rtree_t m_variants;
};

#endif  // VARIANT_MAP_H_
