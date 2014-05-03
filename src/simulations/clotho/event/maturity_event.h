#ifndef MATURITY_EVENT_H_
#define MATURITY_EVENT_H_

#include "../clotho_event.h"

class MaturityEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 5;

    MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, const system_id & obj, Sex s = UNKNOWN );
    MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, const system_id & obj, Sex s = UNKNOWN );

    inline event_type_t getEventType() const { return TYPE_ID; }

    system_id getMatureObject() const;
    Sex getSex() const;

    virtual ~MaturityEvent() {}

protected:
    system_id m_mature_obj;
    Sex m_sex;
};

#endif  // MATURITY_EVENT_H_
