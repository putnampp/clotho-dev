#include "selection_event.h"

const event_type_t SELECTION_EVENT_K = "selection_event";

SelectionEvent::SelectionEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s, const system_id & pending_offspring ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_sex(s),
    m_offspring( pending_offspring )
{}

SelectionEvent::SelectionEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s, const system_id & pending_offspring ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_sex(s),
    m_offspring( pending_offspring )
{}

const event_type_t & SelectionEvent::getEventType() const {
    return SELECTION_EVENT_K;
}

Sex SelectionEvent::getSex() const {
    return m_sex;
}

system_id SelectionEvent::getOffspring() const {
    return m_offspring;
}
