#ifndef MATURITY_EVENT_H_
#define MATURITY_EVENT_H_

#include "clotho_event.h"

extern const string MATURITY_EVENT_K;

class MaturityEvent : public ClothoEvent {
public:
    MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, const system_id & obj, Sex s );
    MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, const system_id & obj, Sex s );

    virtual const event_type_t & getEventType() const;

    system_id getMatureObject() const;
    Sex getSex() const;

    virtual ~MaturityEvent() {}

protected:
    system_id m_mature_obj;
    Sex m_sex;
};

#endif  // MATURITY_EVENT_H_
