#ifndef INDIVIDUAL_INITIALIZER_HPP_
#define INDIVIDUAL_INITIALIZER_HPP_

#include "../initializer.hpp"

namespace initializer {

class IndividualInitializer {
public:
    template < typename IND >
    static void init( IND * ind ) {
        typedef typename IND::inherit_event_t ievent_t;
        typedef typename IND::gamete_t gamete_t;

        ClothoObject * co = ind->getClothoObject();
        ClothoEvent::vtime_t ctime = co->getCurrentTime();

        system_id id = co->getSystemID();

        ClothoObject::event_id_t n_eid = co->getNextEventID(2);
        gamete_t * ngamete = new gamete_t();
        //ievent_t * ie = ievent_t::getOrCreate();
        ievent_t * ie = new ievent_t( ctime, ctime, id, id, n_eid++, ngamete, 0 );
        co->sendEvent( ie );

        //ie = ievent_t::getOrCreate();
        ngamete = new gamete_t();
        ie = new ievent_t( ctime, ctime, id, id, n_eid, ngamete, 1 );
        co->sendEvent( ie );
    }

protected:
};

}   // namespace initializer

#endif  // INDIVIDUAL_INITIALIZER_HPP_
