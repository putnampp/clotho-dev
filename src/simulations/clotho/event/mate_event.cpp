#include "mate_event.h"

MateEvent::object_manager_t MateEvent::m_pool;

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

void MateEvent::init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, const system_id & offspring ) {
    ClothoEvent::init( tSent, tRecv, sender, receiver, eid );
    m_offspring = offspring;
}

void MateEvent::init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, const system_object * offspring ) {
    ClothoEvent::init( tSent, tRecv, sender, receiver, eid );
    m_offspring = offspring->getSystemID();
}

const system_id & MateEvent::getOffspringID() const {
    return m_offspring;
}

/*
void MateEvent::release() {
    m_pool->release(this);
}*/
