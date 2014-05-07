#include "maturity_event.h"

shared_ptr< Pooler< MaturityEvent > > MaturityEvent::m_pool( new Pooler< MaturityEvent >());

MaturityEvent::MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, const system_id & obj, Sex s ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_mature_obj( obj ),
    m_sex(s)
{}

MaturityEvent::MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, const system_id & obj, Sex s ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_mature_obj(obj),
    m_sex(s)
{}

void MaturityEvent::init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, const system_id & obj, Sex s ) {
    ClothoEvent::init( tSent, tRecv, sender, receiver, eid );
    m_mature_obj = obj;
    m_sex = s;
}

void MaturityEvent::init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, const system_id & obj, Sex s ) {
    ClothoEvent::init( tSent, tRecv, sender, receiver, eid );
    m_mature_obj = obj;
    m_sex = s;
}

system_id MaturityEvent::getMatureObject() const {
    return m_mature_obj;
}

Sex MaturityEvent::getSex() const {
    return m_sex;
}

void MaturityEvent::release() {
    m_pool->release(this);
}
