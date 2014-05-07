#ifndef DEATH_EVENT_H_
#define DEATH_EVENT_H_

#include "../clotho_event.h"
#include "pooler.hpp"

class DeathEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 2;

    friend class Pooler< DeathEvent >;

    static DeathEvent * getOrCreate() {
        return DeathEvent::m_pool->getOrCreate();
    }

    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid );

    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid );

    inline event_type_t getEventType() const { return TYPE_ID; }

    void release();

    virtual ~DeathEvent() {}
protected:
    DeathEvent() {}

    DeathEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid );

    DeathEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid );

    static shared_ptr< Pooler< DeathEvent > > m_pool;
};

#endif  // DEATH_EVENT_H_
