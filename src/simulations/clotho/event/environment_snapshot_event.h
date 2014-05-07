#ifndef ENVIRONMENT_SNAPSHOT_EVENT_H_
#define ENVIRONMENT_SNAPSHOT_EVENT_H_

#include "../clotho_event.h"
#include "../object/environment_snapshot.h"

class EnvironmentSnapshotEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 10;

    EnvironmentSnapshotEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid,
                const EnvironmentSnapshot & es );
    EnvironmentSnapshotEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, 
                const EnvironmentSnapshot & es );

    inline event_type_t getEventType() const { return TYPE_ID; }

    const EnvironmentSnapshot & getSnapshot() const;

    void release() {}

    virtual ~EnvironmentSnapshotEvent() {}

protected:
    EnvironmentSnapshot m_snapshot;
};

#endif  // ENVIRONMENT_SNAPSHOT_EVENT_H_
