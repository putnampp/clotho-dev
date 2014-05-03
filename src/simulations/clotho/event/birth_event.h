#ifndef BIRTH_EVENT_H_
#define BIRTH_EVENT_H_

#include "../clotho_event.h"

class BirthEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 1;

    BirthEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s = UNKNOWN );
    BirthEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s = UNKNOWN );

    inline event_type_t getEventType() const { return TYPE_ID; }

    Sex getSex() const;

    virtual ~BirthEvent() {}
protected:
    Sex m_sex;
};

#endif  // BIRTH_EVENT_H_
