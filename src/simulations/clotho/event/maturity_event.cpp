#include "maturity_event.h"

const string MATURITY_EVENT_K = "maturity_event";

MaturityEvent::MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid )
{}

MaturityEvent::MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid )
{}

const event_type_t & MaturityEvent::getEventType() const {
    return MATURITY_EVENT_K;
}
