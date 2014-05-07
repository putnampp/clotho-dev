#ifndef BIRTH_EVENT_H_
#define BIRTH_EVENT_H_

#include "../clotho_event.h"
#include "../pooler.hpp"

class BirthEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 1;

    friend class Pooler< BirthEvent >;

    static BirthEvent * getOrCreate() {
        return BirthEvent::m_pool->getOrCreate();
    }

    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s = UNKNOWN );
    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s = UNKNOWN );

    inline event_type_t getEventType() const { return TYPE_ID; }

    Sex getSex() const;

    void release();

    virtual ~BirthEvent() {}
protected:
    BirthEvent() {}

    BirthEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s = UNKNOWN );
    BirthEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s = UNKNOWN );

    Sex m_sex;

    static shared_ptr< Pooler< BirthEvent > > m_pool;
};

#endif  // BIRTH_EVENT_H_
