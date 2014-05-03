#include "signal_mate_event.h"

SignalMateEvent::SignalMateEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s, unsigned int idx, const system_id & offspring ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_sex( s ),
    m_index( idx ),
    m_offspring( offspring )
{}

SignalMateEvent::SignalMateEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s, unsigned int idx, const system_id & offspring ) :
    ClothoEvent( tSent, tRecv, sender, receiver, eid ),
    m_sex( s ),
    m_index( idx ),
    m_offspring( offspring )
{}

Sex SignalMateEvent::getSex() const {
    return m_sex;
}

unsigned int SignalMateEvent::getIndex() const {
    return m_index;
}

system_id SignalMateEvent::getOffspring() const {
    return m_offspring;
}
