#ifndef MATURITY_EVENT_H_
#define MATURITY_EVENT_H_

#include "../clotho_event.h"
#include "pooler.hpp"

class MaturityEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 5;

    friend class Pooler< MaturityEvent >;

    static MaturityEvent * getOrCreate() {
        return MaturityEvent::m_pool->getOrCreate();
    }

    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, const system_id & obj, Sex s = UNKNOWN );
    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, const system_id & obj, Sex s = UNKNOWN );

    inline event_type_t getEventType() const { return TYPE_ID; }

    system_id getMatureObject() const;
    Sex getSex() const;

    void release();

    virtual ~MaturityEvent() {}

protected:
    MaturityEvent() {}

    MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, const system_id & obj, Sex s = UNKNOWN );
    MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, const system_id & obj, Sex s = UNKNOWN );
    system_id m_mature_obj;
    Sex m_sex;

    static shared_ptr< Pooler< MaturityEvent > > m_pool;
};

#endif  // MATURITY_EVENT_H_
