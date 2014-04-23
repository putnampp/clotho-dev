#ifndef BIRTH_EVENT_H_
#define BIRTH_EVENT_H_

#include "clotho_event.h"

extern const event_type_t BIRTH_EVENT_K;

class BirthEvent : public ClothoEvent {
public:
    BirthEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s = UNKNOWN );
    BirthEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s = UNKNOWN );

    inline const event_type_t & getEventType() const {
        return BIRTH_EVENT_K;
    }

    Sex getSex() const;

    virtual ~BirthEvent() {}
protected:
    Sex m_sex;
};

#endif  // BIRTH_EVENT_H_
