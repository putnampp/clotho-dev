#include "mate_select_event.h"

const event_type_t MATE_SELECT_EVENT_K = "mate_select_event";

MateSelectEvent::MateSelectEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid )
{}

MateSelectEvent::MateSelectEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid )
{}

const event_type_t & MateSelectEvent::getEventType() const {
    return MATE_SELECT_EVENT_K;
}
