#ifndef GENERIC_EVENT_MANAGER_H_
#define GENERIC_EVENT_MANAGER_H_

#include "event_set.h"
#include "event_manager.h"
#include <algorithm>

#include <list>
#include <vector>
#include <set>

#include "event_functor.h"
#include "FSBAllocator.hh"

using std::list;
using std::vector;
using std::multiset;
using std::upper_bound;

template < class E >
class EventManager : virtual public event_manager {
public:
    typedef E event_set_t;
    typedef typename event_set_t::iterator iterator;

    EventManager();
    bool insertEvent( const event * );
    const event * getEvent( const system_id & );
    const event * peekEvent(const system_id & ) const;
    size_t pendingEventCount(const system_id & ) const;
    size_t processedEventCount(const system_id & ) const;
    virtual ~EventManager();
};


template < >
class EventManager < std::list< const event *, FSBAllocator< const event * > > > : virtual public event_manager {
public:
    typedef std::list< const event *, FSBAllocator< const event * > > event_set_t;
    typedef typename event_set_t::iterator iterator;

    EventManager() :
        m_events( ),
        m_head( m_events.begin() ),
        m_nPending(0),
        m_nProcessed(0)
    {}

    bool insertEvent( const event * evt ) {
        iterator pos = upper_bound( m_head, m_events.end(), evt, ltsf_event_order());
        
        bool res = false;
        if( pos == m_head ) {
            m_head = m_events.insert( pos, evt );
            res = true;
        } else
            m_events.insert( pos, evt );
        ++m_nPending;
        return res;
    }

    const event * getEvent( const system_id & ) {
        if( m_head == m_events.end() ) return NULL;

        const event * t = *m_head++;
        --m_nPending;
        ++m_nProcessed;
        return t;
    }

    const event * peekEvent(const system_id & ) const {
        return ((m_head == m_events.end() ) ? NULL : *m_head);
    }

    size_t pendingEventCount(const system_id & ) const {
        return m_nPending;
    }

    size_t processedEventCount(const system_id & ) const {
        return m_nProcessed;
    }

    virtual ~EventManager() {
        while( !m_events.empty() ) {
            const event * t = m_events.back();
            m_events.pop_back();
            if( t ) delete t;
        }
    }
protected:
    event_set_t   m_events;

    iterator    m_head;
    size_t      m_nPending, m_nProcessed;
};

template <>
class EventManager< std::vector< const event * > > : virtual public event_manager {
public:
    typedef std::vector< const event * > event_set_t;
    typedef typename event_set_t::iterator iterator;

    EventManager() :
        m_events(),
        m_head(0)
    {}

    bool insertEvent( const event * e ) {
        if( e == NULL ) return false;
        
        iterator s = m_events.begin() + m_head;
        iterator pos = upper_bound( s, m_events.end(), e, ltsf_event_order());

        m_events.insert( pos, e );
        return (*m_events.begin() == e);
    }

    const event * getEvent( const system_id & ) {
        return ((m_head >= m_events.size() ) ? NULL : m_events[m_head++]);
    }

    const event * peekEvent( const system_id & ) const {
        return ((m_head >= m_events.size() ) ? NULL : m_events[m_head]);
    }

    size_t pendingEventCount( const system_id & ) const {
        return m_events.size() - m_head;
    }

    size_t processedEventCount( const system_id & ) const {
        return m_head;
    }

    virtual ~EventManager() {
        while( !m_events.empty() ) {
            const event * t = m_events.back();
            m_events.pop_back();
            if( t ) delete t;
        }
    }

protected:
    event_set_t     m_events;
    size_t      m_head;
};

template <>
class EventManager< std::multiset< const event *, ltsf_event_order/*, FSBAllocator< const event * >*/ > > : virtual public event_manager {
public:
    typedef std::multiset< const event *, ltsf_event_order/*, FSBAllocator< const event * >*/ > event_set_t;
    typedef typename event_set_t::iterator iterator;

    EventManager() :
        m_events(),
        m_processed()
    {}

    bool insertEvent( const event * e ) {
        m_events.insert( e );
        return (*m_events.begin() == e );
    }

    const event * getEvent( const system_id & ) {
        if( m_events.empty() ) return NULL;
        const event * t = *(m_events.begin());
        m_events.erase( m_events.begin() );
        m_processed.push_back(t);
        return t;
    }

    const event * peekEvent( const system_id & ) const {
        if( m_events.empty() ) return NULL;
        return *(m_events.begin());
    }

    size_t pendingEventCount( const system_id & ) const {
        return m_events.size();
    }

    size_t processedEventCount( const system_id & ) const {
        return m_processed.size();
    }

    virtual ~EventManager() {
        while( !m_events.empty() ) {
            const event * t = *(m_events.begin());
            m_events.erase( m_events.begin());
            if(t) delete t;
        }

        while( !m_processed.empty() ) {
            const event * t = m_processed.back();
            m_processed.pop_back();
            if( t ) delete t;
        }
    }
protected:
    event_set_t m_events;

    std::list< const event *, FSBAllocator< const event *> > m_processed;
};


#endif  // GENERIC_EVENT_MANAGER_H_
