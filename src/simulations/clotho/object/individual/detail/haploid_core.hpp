#ifndef HAPLOID_CORE_HPP_
#define HAPLOID_CORE_HPP_

#include "object/individual/individual3.hpp"

#define BOILER_PLATE_TEMPLATE template < class G, class S, class PROP >
#define BOILER_PLATE_SPECIALIZED_CLASS TIndividual< G, 1, S, PROP >
#define BOILER_PLATE_STRUCTOR( method ) BOILER_PLATE_TEMPLATE BOILER_PLATE_SPECIALIZED_CLASS::method
#define BOILER_PLATE_METHOD( t, m ) BOILER_PLATE_TEMPLATE t BOILER_PLATE_SPECIALIZED_CLASS::m

namespace initializer {

BOILER_PLATE_TEMPLATE
class InitWorker< objects::individual::BOILER_PLATE_SPECIALIZED_CLASS > {
public:
    static void init(objects::individual::BOILER_PLATE_SPECIALIZED_CLASS * ind ) {

    }
};

}   // namespace initializer


#undef BOILER_PLATE_TEMPLATE
#undef BOILER_PLATE_SPECIALIZED_CLASS
#undef BOILER_PLATE_STRUCTOR
#undef BOILER_PLATE_METHOD


#endif  // HAPLOID_CORE_HPP_
