#ifndef POOLED_EVENT_SET_H_
#define POOLED_EVENT_SET_H_

#include "engine/simulation_object.h"
#include "engine/simulation_manager_event_manager.h"
#include "engine/event_functor.h"

#include <list>
#include <unordered_map>
#include <iostream>

using std::unordered_map;
using std::list;
using std::pair;
using std::make_pair;

using std::cout;
using std::endl;

struct pooled_event_set {};

template<>
class EventManager< pooled_event_set > : virtual public event_manager {};

template<>
class SimulationObject< pooled_event_set > : 
    virtual public Object,
    virtual public EventManager< pooled_event_set >
{
public:
    SimulationObject( ) : Object( ) {}
    SimulationObject( simulation_manager * manager ) : 
        Object( manager )
    {}

    virtual const event * getEvent( const system_id & ) {
        return m_sim_manager->getEvent( m_id );
    }

    virtual const event * peekEvent( const system_id & ) const {
        return m_sim_manager->peekEvent( m_id );
    }

    virtual size_t  pendingEventCount( const system_id & ) const {
        return m_sim_manager->pendingEventCount( m_id );
    }

    virtual size_t processedEventCount( const system_id & ) const {
        return m_sim_manager->processedEventCount( m_id );
    }

    virtual void sendEvent( const event * e ) {
        //cout << "sendEvent: routing event" << endl;
        m_sim_manager->routeEvent( e );
    }

    virtual ~SimulationObject() {}
protected:
    bool insertEvent( const event * ) { return false; }
    void receiveEvent( const event * ) {}

};

template <>
class SimulationManagerEventManager< pooled_event_set > : virtual public event_manager {
public:
    typedef list< const event * > processed_events_t;
    typedef multiset< const event *, ltsf_event_order > event_set_t;
    typedef pair< event_set_t *, processed_events_t * > pair_object_events_t;
    typedef unordered_map< system_id, pair_object_events_t >   object_event_set_t;
    typedef typename object_event_set_t::iterator _iterator;
    typedef typename object_event_set_t::const_iterator _citerator;

    bool insertEvent( const event * e ) {
    //    cout << "simulation manager insert event: " << endl;
        _iterator it = m_event_set.find( e->getReceiver() );
        if( it == m_event_set.end() ) {
            pair< _iterator, bool > res = m_event_set.insert( make_pair( e->getReceiver(), make_pair(new event_set_t(), new processed_events_t()) ) );
            assert( res.second );
            it = res.first;
        }

        it->second.first->insert( e );
        return (*(it->second.first->begin()) == e);
    }

    const event * getEvent( const system_id & id ) {
        _iterator it = m_event_set.find( id );
        assert( it != m_event_set.end() );

        if( it->second.first->empty() ) return NULL;
    
        const event * t = *(it->second.first->begin());
        it->second.first->erase(it->second.first->begin());
        it->second.second->push_back( t );
        m_processed.push_back(t);
        return t;
    }

    const event * peekEvent( const system_id & id ) const {
        _citerator it = m_event_set.find( id );
        assert( it != m_event_set.end() );

        if( it->second.first->empty() ) return NULL;
        
        return *(it->second.first->begin());
    }

    size_t  pendingEventCount( const system_id & id ) const {
        _citerator it = m_event_set.find( id );
        //assert( it != m_event_set.end() );
        if ( it == m_event_set.end() ) {
        //    cout << id << " never encountered any events" << endl;
            return 0;
        }
        return it->second.first->size();
    }

    size_t processedEventCount( const system_id & id ) const {
        _citerator it = m_event_set.find( id );
//        assert( it != m_event_set.end() );
        if( it == m_event_set.end() ) {
//            cout << id << " never encountered any events" << endl;
            return 0;
        }
        return it->second.second->size();
    }

    virtual ~SimulationManagerEventManager() {
        while(! m_event_set.empty() ) {
            event_set_t * pending = m_event_set.begin()->second.first;
            processed_events_t * processed = m_event_set.begin()->second.second;

            m_event_set.erase( m_event_set.begin() );
            while( !pending->empty() ) {
                const event * t = *(pending->begin());
                pending->erase( pending->begin());

                delete t;
            }
            delete pending;

            // all processed events will be cleaned up later
            processed->clear();
            delete processed;
        }

        while( !m_processed.empty() ) {
            const event * t = m_processed.back();
            m_processed.pop_back();
            delete t;
        }
    }
protected:
    object_event_set_t  m_event_set;
    processed_events_t  m_processed;
};
#endif  // POOLED_EVENT_SET_H_
