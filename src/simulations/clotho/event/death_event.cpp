#include "death_event.h"

//shared_ptr< Pooler< DeathEvent > > DeathEvent::m_pool( new Pooler< DeathEvent >() );
boost::object_pool< DeathEvent > DeathEvent::m_pool;

DeathEvent::DeathEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid )
{}

DeathEvent::DeathEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid )
{}


void DeathEvent::init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid ) {
    ClothoEvent::init( tSent, tRecv, sender, receiver, eid );
}

void DeathEvent::init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid ) {
    ClothoEvent::init( tSent, tRecv, sender, receiver, eid );
}

//void DeathEvent::release() {
//    m_pool->release(this);
//}
