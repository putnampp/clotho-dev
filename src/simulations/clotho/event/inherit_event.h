#ifndef INHERIT_EVENT_H_
#define INHERIT_EVENT_H_

#include "clotho_event.h"

extern const string INHERIT_EVENT_K;

class InheritEvent : public ClothoEvent {
public:
    InheritEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex parent, AlleleGroupPtr alleles );
    InheritEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex parent, AlleleGroupPtr alleles );

    inline const event_type_t & getEventType() const {
        return INHERIT_EVENT_K;
    }

    Sex getParentSex() const;
    AlleleGroupPtr getAlleles() const;

    virtual ~InheritEvent() {}

protected:
    Sex             m_parent_sex;
    AlleleGroupPtr  m_alleles;
};

#endif  // INHERIT_EVENT_H_
