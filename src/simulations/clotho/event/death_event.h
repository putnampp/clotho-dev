#ifndef DEATH_EVENT_H_
#define DEATH_EVENT_H_

#include "clotho_event.h"

extern const string DEATH_EVENT_K;

class DeathEvent : public ClothoEvent {
public:
    DeathEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid );
    DeathEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid );

    virtual const event_type_t & getEventType() const;

    virtual ~DeathEvent() {}
};

#endif  // DEATH_EVENT_H_
