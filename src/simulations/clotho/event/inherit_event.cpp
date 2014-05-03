#include "inherit_event.h"

InheritEvent::InheritEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid,
                Sex parent,
                gamete * z ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_parent_sex( parent ),
    m_gamete( z )
{}

InheritEvent::InheritEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, 
                Sex parent,
                gamete * z ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_parent_sex( parent ),
    m_gamete( z )
{}

Sex InheritEvent::getParentSex() const {
    return m_parent_sex;
}

gamete * InheritEvent::getGamete() const {
    return m_gamete;
}
