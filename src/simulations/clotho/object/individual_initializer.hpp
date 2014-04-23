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

        ievent_t * ie = new ievent_t( ind->getCurrentTime(), ind->getCurrentTime(), ind, ind, ind->getNextEventID(), new gamete_t(), 0 );

        ind->sendEvent( ie );

        ie = new ievent_t( ind->getCurrentTime(), ind->getCurrentTime(), ind, ind, ind->getNextEventID(), new gamete_t(), 1 );

        ind->sendEvent( ie );
    }

protected:
};

}   // namespace initializer

#endif  // INDIVIDUAL_INITIALIZER_HPP_
