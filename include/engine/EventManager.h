#ifndef GENERIC_EVENT_MANAGER_H_
#define GENERIC_EVENT_MANAGER_H_

#include "event_set.h"
#include "event_manager.h"
#include <algorithm>

#include <list>

#include "event_functor.h"

using std::list;
using std::upper_bound;

template < class ES = std::list< const event * > >
class EventManager : virtual public event_manager {
public:
    typedef ES event_set_t;
    typedef typename event_set_t::iterator iterator;

    EventManager() :
        m_events( new event_set_t() ),
        m_processed( new list< const event * >() )
    {}

    void insertEvent( const event * evt ) {
        iterator pos = upper_bound( m_events->begin(), m_events->end(), evt, m_order);
        
        m_events->insert( pos, evt );
    }

    const event * getEvent() {
        const event * t = m_events->front();
        m_events->pop_front();
        m_processed->push_back( t );
        return t;
    }

    const event * peekEvent() const {
        if( m_events->empty() ) return NULL;
        return m_events->front();
    }

    size_t pendingEventCount() const {
        return m_events->size();
    }

    size_t processedEventCount() const {
        return m_processed->size();
    }

    virtual ~EventManager() {
        while( pendingEventCount() > 0 ) {
            const event * t = getEvent();
            delete t;
        }
        delete m_events;

        while( !m_processed->empty() ) {
            const event * t = m_processed->front();
            m_processed->pop_front();
            delete t;
        }
        delete m_processed;
    }

protected:
    event_set_t *   m_events;
    ltsf_event_order m_order;

    list< const event * > * m_processed;
};

#endif  // GENERIC_EVENT_MANAGER_H_
