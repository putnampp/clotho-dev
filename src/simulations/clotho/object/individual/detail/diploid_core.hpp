#ifndef DIPLOID_CORE_HPP_
#define DIPLOID_CORE_HPP_

#include "object/individual/individual3.hpp"

#define BOILER_PLATE_TEMPLATE template < class G, class S, class PROP >
#define BOILER_PLATE_SPECIALIZED_CLASS TIndividual< G, 2, S, PROP >
#define BOILER_PLATE_STRUCTOR( method ) BOILER_PLATE_TEMPLATE BOILER_PLATE_SPECIALIZED_CLASS::method
#define BOILER_PLATE_METHOD( t, m ) BOILER_PLATE_TEMPLATE t BOILER_PLATE_SPECIALIZED_CLASS::m

#include <iostream>

namespace initializer {

BOILER_PLATE_TEMPLATE
class InitWorker< objects::individual::BOILER_PLATE_SPECIALIZED_CLASS > {
public:
    static void init( objects::individual::BOILER_PLATE_SPECIALIZED_CLASS * ind ) {
        std::cout << "Diploid individual initialized" << std::endl;
////    typedef InheritEvent ievent_t;
////
////    ClothoObject * co = getClothoObject();
////    ClothoEvent::vtime_t ctime = co->getCurrentTime();
////
////    system_id id = co->getSystemID();
////
////    ClothoObject::event_id_t n_eid = co->getNextEventID(2);
////    ievent_t * ie = new ievent_t( ctime, ctime, id, id, n_eid++, gamete_type::EMPTY_GAMETE.copy());
////    co->sendEvent( ie, id, ctime );
////
////    ie = new ievent_t( ctime, ctime, id, id, n_eid, gamete_type::EMPTY_GAMETE.copy());
////    co->sendEvent( ie, id, ctime );
    }
};

}   // namespace ::initializer


#undef BOILER_PLATE_TEMPLATE
#undef BOILER_PLATE_SPECIALIZED_CLASS
#undef BOILER_PLATE_STRUCTOR
#undef BOILER_PLATE_METHOD

#endif  // DIPLOID_CORE_HPP_
