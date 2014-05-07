#ifndef CENTRALIZEDSIMULATIONMANAGERPOOLED_HPP_
#define CENTRALIZEDSIMULATIONMANAGERPOOLED_HPP_

#include "simulation_manager_impl.hpp"
#include "application.h"

#include "common_engine_strings.h"

#include "simulation_stats.h"
#include <map>
#include <deque>

#include "ltsf_pool.hpp"

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

//    typedef pair< vtime_t, size_t > pair_time_offset;
//    typedef pair< object_t *, pair_time_offset > pair_object_timestamp;
//    typedef vector< pair_object_timestamp > object_handle_map_t;
//    typedef vector< object_t * > object_group_t;
//    typedef pair< vtime_t, object_group_t * > concurrent_group_t;
//    typedef map< event::vtime_t, object_group_t * > ordered_object_exe_t;
//    typedef typename ordered_object_exe_t::iterator _iterator;
    typedef vector< object_t * > object_list_t;
    typedef ltsf_pool< vtime_t, object_t >  ltsf_pool_t;

    CentralizedSimulationManager( shared_ptr< application >, system_id::manager_id_t id = 0 );
    CentralizedSimulationManager( shared_ptr< application >, shared_ptr< SimulationStats >,  system_id::manager_id_t id = 0 );

    virtual const vtime_t & getSimulationTime() const;
    virtual bool  isSimulationComplete() const;

//    virtual const system_id getNextObjectID();
//
    virtual object_t * getSimulationObject();

    virtual void registerObject( object_t * obj );
    virtual void unregisterObject( object_t * obj );

    virtual size_t getObjectCount() const;
    virtual object_t * getObject( const system_id & id ) const;

    virtual void initialize();
    virtual void simulate( const vtime_t & until );
    virtual void finalize();

    virtual void routeEvent( /*const*/ event_t * evt );
    virtual void notifyNextEvent( const system_id & obj, const vtime_t & t );

    virtual ~CentralizedSimulationManager();
protected:

//    virtual concurrent_group_t getNextObjectGroup();

    virtual bool setSimulationTime( const vtime_t & t );
    virtual void setSimulateUntil( const vtime_t & t );

//    void moveObject( object_t * obj, vtime_t t );

    shared_ptr< application >   m_app;

    vtime_t    m_sim_time;
    vtime_t    m_sim_until;
    bool m_sim_complete;

    object_list_t   m_objects;
    ltsf_pool_t     m_pooled_objects;

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
    m_stats( new SimulationStats() )
{}

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
    m_stats( stats )
{}

template < class E, class O >
CentralizedSimulationManager<E, O>::~CentralizedSimulationManager() {
    cout << m_nUnregisterCalls << " objects unregistered BEFORE destruction" << endl;
    unsigned int nUnfinalized = 0;
    while( !m_objects.empty() ) {
        object_t * tmp = m_objects.back();
        m_objects.pop_back();
        if( tmp ) {
            if( tmp->getPoolIndex() != ltsf_pool_t::UNKNOWN_INDEX ) {
                ++nUnfinalized;
                tmp->finalize();
            }
            delete tmp;
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
    object_t * obj = new object_t( this, this->getManagerID(), m_objects.size() );

    m_objects.push_back( obj );
    registerObject( obj );

    return obj;
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::registerObject( object_t * obj ) {
    if( obj == NULL ) return;

    assert( obj->getSystemID() != this->m_id );

    if( m_objects[ obj->getObjectID() ]->getPoolIndex() == ltsf_pool_t::UNKNOWN_INDEX ) {
        assert( m_objects[ obj->getObjectID() ] == obj );
        m_pooled_objects.setPoolObject( obj );
        ++m_nRegisteredObjs;
    }
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::unregisterObject( object_t * obj ) {
    ++m_nUnregisterCalls;
    if( obj == NULL ) return;

    if( m_objects[ obj->getObjectID() ]->getPoolIndex() != ltsf_pool_t::UNKNOWN_INDEX ) {
        m_nPendingEvents += obj->pendingEventCount(obj->getSystemID());
        m_nProcessedEvents += obj->processedEventCount(obj->getSystemID());
        m_pooled_objects.unsetPoolObject(obj);

//        m_objects[ obj->getObjectID() ] = NULL;
        --m_nRegisteredObjs;
    }
}

template< class E, class O >
size_t CentralizedSimulationManager<E, O>::getObjectCount() const {
    return m_nRegisteredObjs;
}


template< class E, class O >
typename CentralizedSimulationManager<E, O>::object_t * CentralizedSimulationManager<E, O>::getObject( const system_id & id ) const {
    return m_objects[ id.getObjectID() ];
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::routeEvent( /*const*/ event_t * evt ) {
    system_id::object_id_t id = evt->getReceiver().getObjectID();

    assert( id < m_objects.size() );
    m_objects[ id ]->receiveEvent( evt );
}

template< class E, class O >
void CentralizedSimulationManager<E, O>::notifyNextEvent( const system_id & obj, const vtime_t & t ) {
    m_pooled_objects.updateObject( m_objects[ obj.getObjectID()], t );
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
    vtime_t timestamp = m_pooled_objects.peekNextTimestamp();
    object_t * obj;

//    m_pooled_objects.dump( std::cout );
    while( timestamp != SystemClock::POSITIVE_INFINITY && !setSimulationTime( timestamp ) ) {
        while( (obj = m_pooled_objects.getNextObject( timestamp ) ) != NULL ) {
        //    std::cout << "Processing concurrent events at " << timestamp << " for " << obj->getSystemID() << " (" << obj->getPoolIndex() << ")" << std::endl;
            obj->updateLocalTime(timestamp);
            obj->process();

            const event * e = obj->peekEvent( obj->getSystemID() );
            if( e != NULL ) {
//                std::cout << "Updating " << obj->getSystemID() << " (" << obj->getPoolIndex() << ") to " << e->getReceived() << std::endl;
                m_pooled_objects.updateObject( obj, e->getReceived() );
            }
        }

        timestamp = m_pooled_objects.peekNextTimestamp();
        //m_pooled_objects.dump( std::cout );
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
#endif  // CENTRALIZEDSIMULATIONMANAGERPOOLED_HPP_
