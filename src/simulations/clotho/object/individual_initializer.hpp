#ifndef INDIVIDUAL_INITIALIZER_HPP_
#define INDIVIDUAL_INITIALIZER_HPP_

#include "../initializer.hpp"

namespace initializer {

class IndividualInitializer {
public:
    template < typename IND >
    static void init( IND * ind ) {
        typedef typename IND::inherit_event_t ievent_t;
        typedef typename ievent_t::gamete_t gamete_t;

        ClothoObject * co = ind->getClothoObject();
        ClothoEvent::vtime_t ctime = co->getCurrentTime();

        system_id id = co->getSystemID();

        ievent_t * ie = new ievent_t( ctime, ctime, id, id, co->getNextEventID(), new gamete_t(), 0 );
        co->sendEvent( ie );

        ie = new ievent_t( ctime, ctime, id, id, co->getNextEventID(), new gamete_t(), 1 );
        co->sendEvent( ie );
    }

protected:
};

}   // namespace initializer

#endif  // INDIVIDUAL_INITIALIZER_HPP_
