#ifndef CLOTHO_EVENT_IDER_H_
#define CLOTHO_EVENT_IDER_H_

#include "../ider.h"
#include "../event/clotho_event.h"

template <>
struct IDer< ClothoEvent > {
    typedef event_type_t id_type_t;
    inline id_type_t operator()( const ClothoEvent & ce ) const {
        return ce.getEventType();
    }

    inline id_type_t operator()( const ClothoEvent * ce ) const {
        return ce->getEventType();
    }
};

#endif  // CLOTHO_EVENT_IDER_H_
