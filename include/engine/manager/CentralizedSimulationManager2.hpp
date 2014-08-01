#ifndef CENTRALIZEDSIMULATIONMANAGER2_HPP_
#define CENTRALIZEDSIMULATIONMANAGER2_HPP_

#include "simulation_manager_impl.hpp"
#include "application.h"

#include "common_engine_strings.h"

#include "simulation_stats.h"
#include <map>
#include <deque>

using std::deque;
using std::map;
using std::pair;
using std::make_pair;
using std::swap;

template < class E, class O >
class CentralizedSimulationManager : public SimulationManager< E, O > {
public:
    typedef typename SimulationManager< E, O >::object_t object_t;
    typedef typename SimulationManager< E, O >::vtime_t vtime_t;
    typedef typename SimulationManager< E, O >::event_t event_t;

    typedef pair< vtime_t, size_t > pair_time_offset;
    typedef pair< object_t *, pair_time_offset > pair_object_timestamp;
    typedef vector< pair_object_timestamp > object_handle_map_t;
    typedef vector< object_t * > object_group_t;
    typedef pair< vtime_t, object_group_t * > concurrent_group_t;
    typedef map< event::vtime_t, object_group_t * > ordered_object_exe_t;
    typedef typename ordered_object_exe_t::iterator _iterator;

    CentralizedSimulationManager( shared_ptr< application >, system_id::manager_id_t id = 0 );
    CentralizedSimulationManager( shared_ptr< application >, shared_ptr< SimulationStats >,  system_id::manager_id_t id = 0 );

    virtual const vtime_t & getSimulationTime() const;
    virtual bool  isSimulationComplete() const;

    virtual const system_id getNextObjectID();

    virtual void registerObject( object_t * obj );
    virtual void unregisterObject( object_t * obj );

    virtual size_t getObjectCount() const;
    virtual object_t * getObject( const system_id & id ) const;

    virtual void initialize();
    virtual void simulate( const vtime_t & until );
    virtual void finalize();

    virtual void routeEvent( const event_t * evt );
    virtual void notifyNextEvent( const system_id & obj, const vtime_t & t );

    virtual ~CentralizedSimulationManager();
protected:

    virtual concurrent_group_t getNextObjectGroup();

    virtual bool setSimulationTime( const vtime_t & t );
    virtual void setSimulateUntil( const vtime_t & t );

    void moveObject( object_t * obj, vtime_t t );

    shared_ptr< application >   m_app;

    vtime_t    m_sim_time;
    vtime_t    m_sim_until;
    bool m_sim_complete;

    object_handle_map_t m_objects;
    ordered_object_exe_t m_ordered_objs;

    unsigned int    m_nPendingEvents, m_nProcessedEvents;
    unsigned int    m_nRegisteredObjs;
    unsigned int    m_nUnregisterCalls;

    shared_ptr< SimulationStats > m_stats;
};

//
// Implementation
//

template < class E, class O >
CentralizedSimulationManager<E, O>::CentralizedSimulationManager( shared_ptr< application > app, system_id::manager_id_t id ) :
    SimulationManager<E,O>(id),
    m_app( app ),
    m_sim_time( SystemClock::ZERO ),
    m_sim_until( SystemClock::POSITIVE_INFINITY ),
    m_sim_complete(false),
    m_nPendingEvents(0),
    m_nProcessedEvents(0),
    m_nRegisteredObjs(0),
    m_nUnregisterCalls(0),
    m_stats( new SimulationStats() ) {
}

template < class E, class O >
CentralizedSimulationManager<E,O>::CentralizedSimulationManager( shared_ptr< application > app, shared_ptr< SimulationStats > stats, system_id::manager_id_t id ) :
    SimulationManager<E,O>(id),
    m_app( app ),
    m_sim_time( SystemClock::ZERO ),
    m_sim_until( SystemClock::POSITIVE_INFINITY ),
    m_sim_complete(false),
    m_nPendingEvents(0),
    m_nProcessedEvents(0),
    m_nRegisteredObjs(0),
    m_nUnregisterCalls(0),
    m_stats( stats ) {
}

template < class E, class O >
CentralizedSimulationManager<E, O>::~CentralizedSimulationManager() {
    cout << m_nUnregisterCalls << " objects unregistered BEFORE destruction" << endl;
    unsigned int nUnfinalized = 0;
    while( !m_objects.empty() ) {
        object_t * tmp = m_objects.back().first;
        m_objects.pop_back();
        if( tmp ) {
            ++nUnfinalized;
            tmp->finalize();
            delete tmp;
        }
    }
    cout << nUnfinalized << " objects were finalized AFTER simulation finalization." << endl;
    m_ordered_objs.clear();
}

template < class E, class O >
const system_id CentralizedSimulationManager<E, O>::getNextObjectID() {
    system_id nid( this->getManagerID(), m_objects.size() );
    m_objects.push_back( make_pair( (object_t *)NULL, make_pair(SystemClock::POSITIVE_INFINITY, 0 ) ) );
    return nid;
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::registerObject( object_t * obj ) {
    if( obj == NULL ) return;

    assert( obj->getSystemID() != this->m_id );

    if( m_objects[ obj->getObjectID() ].first == NULL ) {
        m_objects[ obj->getObjectID() ].first = obj;
        ++m_nRegisteredObjs;
    } else {
        moveObject( obj, SystemClock::POSITIVE_INFINITY );
    }
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::unregisterObject( object_t * obj ) {
    ++m_nUnregisterCalls;
    if( obj == NULL ) return;

    if( m_objects[ obj->getObjectID() ].first != NULL ) {
        m_nPendingEvents += obj->pendingEventCount(obj->getSystemID());
        m_nProcessedEvents += obj->processedEventCount(obj->getSystemID());

        if( m_objects[ obj->getObjectID() ].second.first != SystemClock::POSITIVE_INFINITY ) {
            moveObject( obj, SystemClock::POSITIVE_INFINITY );
        }

        m_objects[ obj->getObjectID() ].first = NULL;
        --m_nRegisteredObjs;
    }
}

template < class E, class O >
void CentralizedSimulationManager<E, O>::moveObject( object_t * obj, vtime_t newT ) {
    system_id::object_id_t o_id = obj->getObjectID();

    pair_time_offset pto = m_objects[ o_id ].second;

    if( pto.first != SystemClock::POSITIVE_INFINITY ) {
        // objects with time set to POSITIVE_INFINITY are not "queued"
        _iterator it = m_ordered_objs.find( pto.first );

        if( it != m_ordered_objs.end() ) {
            object_group_t * g = it->second;

//            assert( pto.second < g->size() &&  g->at( pto.second ) == obj );

            if( g->back() != obj ) {
                // object is not at the end of vector
                // swap with object at the end of the vector
                // and update that objects offset value to
                // be that of the current obj
                system_id::object_id_t other = g->back()->getObjectID();

//                (*g)[ pto.second ] = g->back();
//                g->back() = obj;
                swap( g->at(pto.second), g->back() );

                assert( g->back() == obj );

                m_objects[ other ].second.second = pto.second;
            }
            g->pop_back();
        }
    }

    m_objects[ o_id ].second.first = newT;
    if( newT != SystemClock::POSITIVE_INFINITY ) {
        _iterator it = m_ordered_objs.find( newT );
        if( it == m_ordered_objs.end() ) {
            // no other objects have events scheduled at newT
            pair< _iterator, bool > res = m_ordered_objs.insert( make_pair(newT, new object_group_t() ) );
            assert( res.second );
            it = res.first;

//            it->second->reserve( m_nRegisteredObjs );
        }

        m_objects[ o_id ].second.second = it->second->size();
        it->second->push_back( obj );
    }
}

template< class E, class O >
size_t CentralizedSimulationManager<E, O>::getObjectCount() const {
    return m_nRegisteredObjs;
}


template< class E, class O >
typename CentralizedSimulationManager<E, O>::object_t * CentralizedSimulationManager<E, O>::getObject( const system_id & id ) const {
    return m_objects[ id.getObjectID() ].first;
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::routeEvent( const event_t * evt ) {
    assert( m_objects.size() > evt->getReceiver().getObjectID() );
    m_objects[ evt->getReceiver().getObjectID() ].first->receiveEvent( evt );
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::notifyNextEvent( const system_id & obj, const vtime_t & t ) {
    moveObject( m_objects[ obj.getObjectID() ].first, t );
}

template< class E, class O >
typename CentralizedSimulationManager<E, O>::concurrent_group_t CentralizedSimulationManager<E, O>::getNextObjectGroup() {
    concurrent_group_t ot = *m_ordered_objs.begin();
    m_ordered_objs.erase( m_ordered_objs.begin() );

    return ot;
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::initialize() {
    m_stats->startPhase( INIT_PHASE_K );
    m_app->initialize();
    m_stats->stopPhase( INIT_PHASE_K );
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::simulate( const vtime_t & until ) {
    setSimulateUntil( until );

    m_stats->startPhase( SIMULATE_PHASE_K );
    while(! m_ordered_objs.empty() ) {
        concurrent_group_t ot = getNextObjectGroup();

        if( ot.first == SystemClock::POSITIVE_INFINITY || setSimulationTime( ot.first ) ) {
            break;
        }

//        for( object_group_t::iterator it = ot.second->begin(); it != ot.second->end(); it++ ) {
        while( !ot.second->empty() ) {
            object_t * obj = ot.second->back();
            ot.second->pop_back();

            m_objects[ obj->getObjectID() ].second.first = SystemClock::POSITIVE_INFINITY;
            obj->updateLocalTime(ot.first);
            obj->process();

            const event * e = obj->peekEvent( obj->getSystemID() );
            if( e != NULL ) {
                moveObject( obj, e->getReceived() );
            } else {
                moveObject( obj, SystemClock::POSITIVE_INFINITY );
            }
        }

        delete ot.second;
    }

    m_stats->stopPhase( SIMULATE_PHASE_K );

    cout << "End simulation time: " <<  getSimulationTime() << endl;
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::finalize() {
    m_stats->startPhase( FINALIZE_PHASE_K );

    m_app->finalize();

    m_stats->stopPhase( FINALIZE_PHASE_K );

    m_stats->setProcessedEvents( m_nProcessedEvents );
    m_stats->setPendingEvents( m_nPendingEvents );

}

template< class E, class O >
bool CentralizedSimulationManager<E, O>::isSimulationComplete() const {
    return m_sim_complete;
}

template< class E, class O >
bool CentralizedSimulationManager<E, O>::setSimulationTime( const vtime_t & t ) {
    assert( m_sim_time <= t );
    if( m_sim_time != t ) {
        m_sim_time = t;
        m_sim_complete = (m_sim_time >= m_sim_until);
    }

    return m_sim_complete;
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::setSimulateUntil( const vtime_t & t ) {
    m_sim_until = t;
}

template< class E, class O >
const typename CentralizedSimulationManager<E,O>::vtime_t & CentralizedSimulationManager<E, O>::getSimulationTime() const {
    return m_sim_time;
}
#endif  // CENTRALIZEDSIMULATIONMANAGER2_HPP_
