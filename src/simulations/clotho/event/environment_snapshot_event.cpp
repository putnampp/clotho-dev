#include "environment_snapshot_event.h"

const event_type_t   ENVIRONMENT_SNAPSHOT_EVENT_K = "environment_snapshot_event";

EnvironmentSnapshotEvent::EnvironmentSnapshotEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid,
                const EnvironmentSnapshot & es
                 ):
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_snapshot( es )
{}

EnvironmentSnapshotEvent::EnvironmentSnapshotEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, 
                const EnvironmentSnapshot & es ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_snapshot( es )
{}

const EnvironmentSnapshot & EnvironmentSnapshotEvent::getSnapshot() const {
    return m_snapshot;
}
