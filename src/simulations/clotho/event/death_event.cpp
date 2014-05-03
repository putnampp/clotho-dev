#include "death_event.h"

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
