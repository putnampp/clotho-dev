#include "birth_event.h"

const string BIRTH_EVENT_K = "birth_event";

BirthEvent::BirthEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid )
{}

BirthEvent::BirthEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid )
{}

const event_type_t & BirthEvent::getEventType() const {
    return BIRTH_EVENT_K;
}
