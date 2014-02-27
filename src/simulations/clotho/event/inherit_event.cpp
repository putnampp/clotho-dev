#include "inherit_event.h"

const string INHERIT_EVENT_K = "inherit_event";

InheritEvent::InheritEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid,
                Sex parent,
                AlleleGroupPtr alleles ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_parent_sex( parent ),
    m_alleles( alleles )
{}

InheritEvent::InheritEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, 
                Sex parent,
                AlleleGroupPtr alleles ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_parent_sex( parent ),
    m_alleles( alleles )
{}

Sex InheritEvent::getParentSex() const {
    return m_parent_sex;
}

AlleleGroupPtr InheritEvent::getAlleles() const {
    return m_alleles;
}
