#ifndef CENTRALIZEDSIMULATIONMANAGERPAGED_HPP_
#define CENTRALIZEDSIMULATIONMANAGERPAGED_HPP_

#include "simulation_manager_impl.hpp"
#include "application.h"

#include "common_engine_strings.h"

#include "simulation_stats.h"
#include <map>

#include "paged_event_queue.hpp"

//using std::deque;
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

    typedef std::pair< object_t *, bool > object_pool_pair_t;
    typedef vector< object_pool_pair_t > object_list_t;

    typedef PagedEventQueue< event_t, system_id > paged_event_pool_t;
    typedef typename paged_event_pool_t::event_page_manager_t page_manager_t;

    typedef std::map< vtime_t, paged_event_pool_t * > ltsf_pool_t;

    CentralizedSimulationManager( shared_ptr< application >, system_id::manager_id_t id = 0 );
    CentralizedSimulationManager( shared_ptr< application >, shared_ptr< SimulationStats >,  system_id::manager_id_t id = 0 );

    virtual const vtime_t & getSimulationTime() const;
    virtual bool  isSimulationComplete() const;

    virtual object_t * getSimulationObject();

    virtual void registerObject( object_t * obj );
    virtual void registerObject( size_t idx );

    virtual void unregisterObject( object_t * obj );
    virtual void unregisterObject( size_t idx );

    virtual size_t getObjectCount() const;
    virtual object_t * getObject( const system_id & id ) const;

    virtual void initialize();
    virtual void simulate( const vtime_t & until );
    virtual void finalize();

    virtual void routeEvent( /*const*/ event_t * evt );
    virtual void routeEvent( const system_id & id, event_t * evt );
    virtual void routeEvent( event_t * evt, const system_id & id, const vtime_t & t );
    virtual void notifyNextEvent( const system_id & obj, const vtime_t & t );

    virtual ~CentralizedSimulationManager();
protected:

    virtual bool setSimulationTime( const vtime_t & t );
    virtual void setSimulateUntil( const vtime_t & t );

    shared_ptr< application >   m_app;

    vtime_t    m_sim_time;
    vtime_t    m_sim_until;
    bool m_sim_complete;

    object_list_t   m_objects;
    ltsf_pool_t     m_pooled_events;

    unsigned int    m_nPendingEvents, m_nProcessedEvents;
    unsigned int    m_nRegisteredObjs;
    unsigned int    m_nUnregisterCalls;

    shared_ptr< SimulationStats > m_stats;
    page_manager_t  m_page_manager;
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
        //object_t * tmp = m_objects.back();
        object_pool_pair_t tmp = m_objects.back();
        m_objects.pop_back();
        if( tmp.first ) {
            //if( tmp.second != ltsf_pool_t::UNKNOWN_INDEX ) {
            if( tmp.second ) {
                ++nUnfinalized;
                tmp.first->finalize();
            }
            delete tmp.first;
        }
    }
    cout << nUnfinalized << " objects were finalized AFTER simulation finalization." << endl;
}

//template < class E, class O >
//const system_id CentralizedSimulationManager<E, O>::getNextObjectID() {
//    system_id nid( this->getManagerID(), m_objects.size() );
//    m_objects.push_back( (object_t *)NULL );
//    return nid;
//}

template < class E, class O >
typename CentralizedSimulationManager< E, O>::object_t * CentralizedSimulationManager<E, O>::getSimulationObject() {
    system_id::manager_id_t mid = this->getManagerID();
    system_id::object_id_t oid = m_objects.size();

    object_t * obj = new object_t( this, mid, oid );

    //m_objects.push_back( make_pair( obj, ltsf_pool_t::UNKNOWN_INDEX ) );
    m_objects.push_back( make_pair( obj, false ) );
    registerObject( oid );

    return obj;
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::registerObject( object_t * obj ) {
    if( obj == NULL ) return;

    system_id::object_id_t oid = obj->getObjectID();
    registerObject( oid );
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::registerObject( size_t idx ) {
//    if( m_objects[idx].second  == ltsf_pool_t::UNKNOWN_INDEX ) {
    if( !m_objects[idx].second ) {
//        size_t pool_idx = m_pooled_objects.setPoolObject( idx );
//        m_objects[idx].second = pool_idx;
        m_objects[idx].second = true;

        ++m_nRegisteredObjs;
    }
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::unregisterObject( object_t * obj ) {
    if( obj == NULL ) return;

    system_id::object_id_t id = obj->getObjectID();
    unregisterObject(id);
}

template < class E, class O >
void CentralizedSimulationManager<E, O>::unregisterObject( size_t idx ) {
    ++m_nUnregisterCalls;
//    if( m_objects[ idx ].second != ltsf_pool_t::UNKNOWN_INDEX ) {
    if( m_objects[ idx ].second ) {
        object_pool_pair_t & o_pair = m_objects[idx];
        object_t * obj = o_pair.first;

        system_id id = obj->getSystemID();

        m_nPendingEvents += obj->pendingEventCount(id);
        m_nProcessedEvents += obj->processedEventCount(id);

//        m_pooled_objects.unsetPoolObject(idx);

        //m_objects[ idx ].first = NULL;
        //m_objects[idx].second = false;
        o_pair.second = false;
        --m_nRegisteredObjs;
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
void CentralizedSimulationManager<E, O>::routeEvent( /*const*/ event_t * evt ) {
    routeEvent( evt, evt->getReceiver(), evt->getReceived() );
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::routeEvent( const system_id & id, event_t * evt ) {
    vtime_t t = evt->getReceived();
    routeEvent( evt, id, t );
}

template < class E, class O >
void CentralizedSimulationManager<E, O>::routeEvent( event_t * evt, const system_id & recv_id, const vtime_t & rt ) {
    typename ltsf_pool_t::iterator it = m_pooled_events.find(rt);
    if( it == m_pooled_events.end() ) {
        paged_event_pool_t * tmp = new paged_event_pool_t( & m_page_manager );
        std::pair< typename ltsf_pool_t::iterator, bool > res = m_pooled_events.insert( std::make_pair( rt, tmp ) );
        assert( res.second );

        it = res.first;
    }
    it->second->append( evt, recv_id );
}


template< class E, class O >
void CentralizedSimulationManager<E, O>::notifyNextEvent( const system_id & obj, const vtime_t & t ) {
    assert(false);
//    m_pooled_objects.updateObject( m_objects[ obj.getObjectID() ].second, t );
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
//    vtime_t timestamp = m_pooled_objects.peekNextTimestamp();
    typename ltsf_pool_t::iterator it = m_pooled_events.begin();
    vtime_t timestamp = (( it == m_pooled_events.end() ) ? SystemClock::POSITIVE_INFINITY : it->first);

    while( timestamp != SystemClock::POSITIVE_INFINITY && !setSimulationTime( timestamp ) ) {
        std::cout << "Simulation time: " << timestamp << std::endl;
        paged_event_pool_t * tmp_evts = it->second;

        object_t * obj = NULL;
        event_t * evt = NULL;
        while( (evt = tmp_evts->next( ) ) != NULL ) {
            if( obj == NULL || obj->getSystemID() != evt->getReceiver() ) {
                system_id::object_id_t oid = evt->getReceiver().getObjectID();
                obj = m_objects[ oid ].first;
                obj->updateLocalTime(timestamp);
                assert( obj->getEventPerformer() != NULL );
            }

            obj->getEventPerformer()->perform_event( evt );
            ++m_nProcessedEvents;
        }

        assert( tmp_evts->size() == 0 );

        m_pooled_events.erase( it );

        it = m_pooled_events.begin();
        timestamp = (( it == m_pooled_events.end() ) ? SystemClock::POSITIVE_INFINITY : it->first );
    }

    m_stats->stopPhase( SIMULATE_PHASE_K );

    cout << "End simulation time: " <<  getSimulationTime() << endl;
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::finalize() {
    m_stats->startPhase( FINALIZE_PHASE_K );

    m_app->finalize();

    while( !m_pooled_events.empty() ) {
        paged_event_pool_t * tmp = m_pooled_events.begin()->second;
        m_pooled_events.erase( m_pooled_events.begin() );

        m_nPendingEvents += tmp->size();
        delete tmp;
    }

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
#endif  // CENTRALIZEDSIMULATIONMANAGERPOOLED_HPP_
