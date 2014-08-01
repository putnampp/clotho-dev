#include "mate_select_event.h"

MateSelectEvent::object_manager_t MateSelectEvent::m_pool;

MateSelectEvent::MateSelectEvent( const vtime_t & tSent, const vtime_t & tRecv,
                                  const system_id & sender, const system_id & receiver,
                                  event_id_t eid ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ) {
}

MateSelectEvent::MateSelectEvent( const vtime_t & tSent, const vtime_t & tRecv,
                                  const system_object * sender, const system_object * receiver,
                                  event_id_t eid ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ) {
}

void MateSelectEvent::init( const vtime_t & tSent, const vtime_t & tRecv,
                            const system_id & sender, const system_id & receiver,
                            event_id_t eid ) {
    ClothoEvent::init( tSent, tRecv, sender, receiver, eid );
}

void MateSelectEvent::init( const vtime_t & tSent, const vtime_t & tRecv,
                            const system_object * sender, const system_object * receiver,
                            event_id_t eid ) {
    ClothoEvent::init( tSent, tRecv, sender, receiver, eid );
}

/*
void MateSelectEvent::release() {
    m_pool->release( this );
}*/
