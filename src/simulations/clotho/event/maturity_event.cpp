#include "maturity_event.h"

const string MATURITY_EVENT_K = "maturity_event";

MaturityEvent::MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_sex(s)
{}

MaturityEvent::MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_sex(s)
{}

const event_type_t & MaturityEvent::getEventType() const {
    return MATURITY_EVENT_K;
}

Sex MaturityEvent::getSex() const {
    return m_sex;
}
