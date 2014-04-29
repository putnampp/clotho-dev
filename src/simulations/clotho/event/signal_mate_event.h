#ifndef SIGNAL_MATE_EVENT_H_
#define SIGNAL_MATE_EVENT_H_

#include "../clotho_event.h"

extern const event_type_t SIGNAL_MATE_EVENT_K;

class SignalMateEvent : public ClothoEvent {
public:
    SignalMateEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s, unsigned int idx, const system_id & offspring );
    SignalMateEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s, unsigned int idx, const system_id & offspring );

    inline const event_type_t & getEventType() const {
        return SIGNAL_MATE_EVENT_K;
    }

    Sex getSex() const;
    unsigned int getIndex() const;

    system_id   getOffspring() const;

    virtual ~SignalMateEvent() {}
protected:
    Sex             m_sex;
    unsigned int    m_index;
    system_id       m_offspring;
};

#endif  // SIGNAL_MATE_EVENT_H_
