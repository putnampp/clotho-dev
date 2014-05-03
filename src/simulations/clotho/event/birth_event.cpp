#include "birth_event.h"

BirthEvent::BirthEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_sex(s)
{}

BirthEvent::BirthEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_sex(s)
{}

Sex BirthEvent::getSex() const {
    return m_sex;
}
