#include "mate_event.h"

const string MATE_EVENT_K = "mate_event";

MateEvent::MateEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, const system_id & offspring ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_offspring( offspring )
{}

MateEvent::MateEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, const system_object * offspring ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_offspring( offspring->getSystemID() )
{}

const event_type_t & MateEvent::getEventType() const {
    return MATE_EVENT_K;
}

const system_id & MateEvent::getOffspringID() const {
    return m_offspring;
}
