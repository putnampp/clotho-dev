#ifndef SELECTION_EVENT_H_
#define SELECTION_EVENT_H_

#include "../clotho_event.h"

extern const event_type_t SELECTION_EVENT_K;

class SelectionEvent : public ClothoEvent {
public:
    SelectionEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s, const system_id & pending_offspring );
    SelectionEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s, const system_id & pending_offspring );

    virtual const event_type_t & getEventType() const;

    Sex getSex() const;
    system_id getOffspring() const;

    virtual ~SelectionEvent() {}

protected:
    Sex m_sex;
    system_id   m_offspring;
};

#endif  // SELECTION_EVENT_H_
