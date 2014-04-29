#ifndef ENVIRONMENT_SNAPSHOT_EVENT_H_
#define ENVIRONMENT_SNAPSHOT_EVENT_H_

#include "../clotho_event.h"
#include "../object/environment_snapshot.h"

extern const event_type_t   ENVIRONMENT_SNAPSHOT_EVENT_K;

class EnvironmentSnapshotEvent : public ClothoEvent {
public:
    EnvironmentSnapshotEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid,
                const EnvironmentSnapshot & es );
    EnvironmentSnapshotEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, 
                const EnvironmentSnapshot & es );

    inline const event_type_t & getEventType() const {
        return ENVIRONMENT_SNAPSHOT_EVENT_K;
    }

    const EnvironmentSnapshot & getSnapshot() const;

    virtual ~EnvironmentSnapshotEvent() {}

protected:
    EnvironmentSnapshot m_snapshot;
};

#endif  // ENVIRONMENT_SNAPSHOT_EVENT_H_
