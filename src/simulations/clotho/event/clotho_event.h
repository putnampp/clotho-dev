#ifndef CLOTHO_EVENT_H_
#define CLOTHO_EVENT_H_

#include "engine/default_event.h"
#include "../clotho.h"
#include "../ider.h"

#include <ostream>

typedef std::string event_type_t;

class ClothoEvent : public default_event {
public:
    virtual const event_type_t & getEventType() const = 0;

    virtual ~ClothoEvent() {}
protected:
    ClothoEvent(  const vtime_t & tSent, const vtime_t & tRecv,
                    const system_id & sender, const system_id & receiver,
                    event_id_t eid ) :
        default_event( tSent, tRecv, sender, receiver, eid ) 
    {}

    ClothoEvent( const vtime_t & tSent, const vtime_t & tRecv,
                 const system_object * sender, const system_object * receiver,
                 event_id_t eid ) :
        default_event( tSent, tRecv, sender, receiver, eid )
    {}
};

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

std::ostream & operator<<( std::ostream & o, const ClothoEvent & ce );

inline std::ostream & operator<<( std::ostream & o, const ClothoEvent * ce ) {
    o << *ce;
    return o;
}

#endif  // CLOTHO_EVENT_H_
