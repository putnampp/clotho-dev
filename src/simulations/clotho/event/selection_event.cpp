#include "selection_event.h"

SelectionEvent::object_manager_t SelectionEvent::m_pool;

SelectionEvent::SelectionEvent( const vtime_t & tSent, const vtime_t & tRecv,
                                const system_id & sender, const system_id & receiver,
                                event_id_t eid, Sex s, const system_id & pending_offspring ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_sex(s),
    m_offspring( pending_offspring ) {
}

SelectionEvent::SelectionEvent( const vtime_t & tSent, const vtime_t & tRecv,
                                const system_object * sender, const system_object * receiver,
                                event_id_t eid, Sex s, const system_id & pending_offspring ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_sex(s),
    m_offspring( pending_offspring ) {
}

Sex SelectionEvent::getSex() const {
    return m_sex;
}

system_id SelectionEvent::getOffspring() const {
    return m_offspring;
}
