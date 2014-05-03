#ifndef DEATH_EVENT_H_
#define DEATH_EVENT_H_

#include "../clotho_event.h"

class DeathEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 2;

    DeathEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid );
    DeathEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid );

    inline event_type_t getEventType() const { return TYPE_ID; }

    virtual ~DeathEvent() {}
};

#endif  // DEATH_EVENT_H_
