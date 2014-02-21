#ifndef DEFAULT_EVENT_H_
#define DEFAULT_EVENT_H_

#include "event_interface.h"
#include "orderable.h"

class default_event : virtual public event {
public:
    default_event(  const vtime_t & tSent, const vtime_t & tRecv,
                    const system_id & sender, const system_id & receiver,
                    event_id_t eid ):
                m_sent( tSent),
                m_recv( tRecv),
                m_sender( sender ),
                m_receiver( receiver )
                m_eid( eid )
    {}

    virtual const event_id_t &  getEventID() const { return m_eid; }
    virtual const system_id &   getSender() const { return m_sender; }
    virtual const system_id &   getReceiver() const { return m_receiver; }

    virtual const vtime_t &     getSent() const { return m_sent; }
    virtual const vtime_t &     getReceived() const { return m_recv; }

    virtual bool operator<( const event * rhs ) const {
        return (m_recv < rhs->m_recv) ||
            (m_recv == rhs->m_recv &&
             m_eid < rhs->m_eid );
    }

    virtual bool operator==( const event * rhs ) const {
        return (m_sender == rhs->m_sender 
                && m_eid == rhs->m_eid);
    }

    virtual ~default_event() {}
protected:
    const vtime_t       m_sent, m_recv;
    const system_id     m_sender, m_receiver;
    const event_id_t    m_eid;
};

#endif  // DEFAULT_EVENT_H_