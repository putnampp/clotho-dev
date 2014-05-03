#include "maturity_event.h"

MaturityEvent::MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, const system_id & obj, Sex s ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_mature_obj( obj ),
    m_sex(s)
{}

MaturityEvent::MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, const system_id & obj, Sex s ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_mature_obj(obj),
    m_sex(s)
{}

system_id MaturityEvent::getMatureObject() const {
    return m_mature_obj;
}

Sex MaturityEvent::getSex() const {
    return m_sex;
}
