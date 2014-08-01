#ifndef CLOTHO_EVENT_H_
#define CLOTHO_EVENT_H_

#include "engine/event_interface.h"
#include "engine/object/system_object.h"
#include "clotho.h"
#include "ider.h"

#include "utility/pool/object_manager.h"

#include <ostream>

#define CEID id
#define CLOTHO_EVENT_ID( x ) static const clotho_event_type CEID = x; \
inline clotho_event_type getEventType() const { return CEID; }

static const unsigned int EVENT_PAGE_SIZE = (1 << 14);

typedef unsigned int event_type_t;
typedef event_type_t clotho_event_type;

class ClothoEvent : public event {
public:

    inline const event_id_t &  getEventID() const {
        return m_eid;
    }
    inline const system_id &   getSender() const {
        return m_sender;
    }
    inline const system_id &   getReceiver() const {
        return m_receiver;
    }

    inline const vtime_t &     getSent() const {
        return m_sent;
    }
    inline const vtime_t &     getReceived() const {
        return m_recv;
    }

    inline bool isReceivedAt( const vtime_t & t ) const {
        return (m_recv == t);
    }

    inline bool operator<( const event * rhs ) const {
        return m_recv < rhs->getReceived();
    }

    inline bool operator<( const ClothoEvent * rhs ) const {
        return m_recv < rhs->m_recv;
    }

    inline bool operator==( const event * rhs ) const {
        return ( m_sender == rhs->getSender() && m_eid == rhs->getEventID() );
    }

    inline bool operator==( const ClothoEvent * rhs ) const {
        return (m_sender == rhs->m_sender && m_eid == rhs->m_eid);
    }

    virtual event_type_t getEventType() const = 0;

    virtual ~ClothoEvent() {}
protected:
    ClothoEvent() {}

    ClothoEvent(  const vtime_t & tSent, const vtime_t & tRecv,
                  const system_id & sender, const system_id & receiver,
                  event_id_t eid ) :
        m_sent( tSent),
        m_recv( tRecv),
        m_sender( sender ),
        m_receiver( receiver ),
        m_eid( eid ) {
    }

    ClothoEvent( const vtime_t & tSent, const vtime_t & tRecv,
                 const system_object * sender, const system_object * receiver, event_id_t eid ) :
        ClothoEvent( tSent, tRecv, sender->getSystemID(), receiver->getSystemID(), eid ) {
    }

    void init(  const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid ) {
        m_sent = tSent;
        m_recv = tRecv;
        m_sender = sender;
        m_receiver = receiver;
        m_eid = eid;
    }

    void init( const vtime_t & tSent, const vtime_t & tRecv,
               const system_object * sender, const system_object * receiver, event_id_t eid ) {
        m_sent = tSent;
        m_recv = tRecv;
        m_sender = sender->getSystemID();
        m_receiver = receiver->getSystemID();
        m_eid = eid;
    }
    vtime_t       m_sent, m_recv;
    system_id     m_sender, m_receiver;
    event_id_t    m_eid;
};

template <>
struct IDer< ClothoEvent > {
    typedef event_type_t id_type_t;
    inline id_type_t operator()( const ClothoEvent & ce ) const {
        return ce.getEventType();
    }

    inline id_type_t operator()( const ClothoEvent * ce ) const {
        return ce->getEventType();
    }
};

std::ostream & operator<<( std::ostream & o, const ClothoEvent & ce );

inline std::ostream & operator<<( std::ostream & o, const ClothoEvent * ce ) {
    o << *ce;
    return o;
}

#endif  // CLOTHO_EVENT_H_
