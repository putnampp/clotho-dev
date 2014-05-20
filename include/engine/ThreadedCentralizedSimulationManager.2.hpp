#ifndef THREADED_CENTRALIZED_SIMULATION_MANAGER2_HPP_
#define THREADED_CENTRALIZED_SIMULATION_MANAGER2_HPP_

#include "simulation_manager_impl.hpp"
#include "application.h"

#include "common_engine_strings.h"
#include "simulation_stats.h"

#include "synchronizer.h"

#include "threaded_paged_event_queue.hpp"
#include "PageWorkerThread.h"
#include "common_thread.h"

#include <map>
#include <vector>

using std::map;
using std::vector;
using std::pair;
using std::make_pair;

template < class E, class O >
class ThreadedCentralizedSimulationManager : public SimulationManager<E, O>, public synchronizer {
public:
    typedef typename SimulationManager< E, O >::object_t object_t;
    typedef typename SimulationManager< E, O >::vtime_t vtime_t;
    typedef typename SimulationManager< E, O >::event_t event_t;

    typedef std::pair< object_t *, bool > object_pool_pair_t;
    typedef vector< object_pool_pair_t > object_list_t;

    typedef ThreadedPagedEventQueue< event_t, system_id > paged_event_pool_t;
    typedef PageWorkerThread< event_t, system_id, ThreadedCentralizedSimulationManager<E,O> > worker_thread_t;

    typedef typename paged_event_pool_t::event_page_manager_t page_manager_t;

    typedef std::map< vtime_t, paged_event_pool_t * > ltsf_pool_t;

    typedef typename paged_event_pool_t::event_page_t event_page_t;

    ThreadedCentralizedSimulationManager( shared_ptr< application >, unsigned int threads, system_id::manager_id_t id = 0 );
    ThreadedCentralizedSimulationManager( shared_ptr< application >, shared_ptr< SimulationStats >, unsigned int threads,  system_id::manager_id_t id = 0 );

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

    virtual void routeEvent( event_t * evt );
    virtual void routeEvent( const system_id & id, event_t * evt );
    virtual void routeEvent( event_t * evt, const system_id & id, const vtime_t & t );
    virtual void notifyNextEvent( const system_id & obj, const event::vtime_t & t );

    virtual void sync();

    virtual ~ThreadedCentralizedSimulationManager();

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

    unsigned int m_nThreads;
    unsigned int m_tid;

    worker_thread_t **  m_workers;

    pthread_barrier_t   m_syncpoint;
    pthread_mutex_t     m_mutObjID;
};

//
// Implementation
//

template < class E, class O >
ThreadedCentralizedSimulationManager< E, O >::ThreadedCentralizedSimulationManager( shared_ptr< application > app, unsigned int nThreads, system_id::manager_id_t id ) :
    SimulationManager< E, O >( id ),
    m_app(app),
    m_sim_time( SystemClock::ZERO ),
    m_sim_until( SystemClock::POSITIVE_INFINITY ),
    m_sim_complete( false ),
    m_nPendingEvents(0),
    m_nProcessedEvents(0),
    m_nRegisteredObjs(0),
    m_nUnregisterCalls(0),
    m_stats(),
    m_nThreads(nThreads),
    m_tid( nThreads ),
    m_workers( NULL )
{
    m_workers = new worker_thread_t[ nThreads ];

    pthread_barrier_init( &m_syncpoint, NULL, m_nThreads + 1 );
    pthread_mutex_init( &m_mutObjID, NULL );

    pthread_once( &common_key_once, make_thread_key );
    pthread_setspecific( common_thread_key, (void *) &m_tid );

    m_stats.reset( new SimulationStats() );
}

template < class E, class O >
ThreadedCentralizedSimulationManager< E, O >::ThreadedCentralizedSimulationManager( shared_ptr< application > app, shared_ptr< SimulationStats > stats, unsigned int nThreads, system_id::manager_id_t id ) :
    SimulationManager< E, O >( id ), 
    m_app(app),
    m_sim_time( SystemClock::ZERO ),
    m_sim_until( SystemClock::POSITIVE_INFINITY ),
    m_sim_complete( false ),
    m_nPendingEvents(0),
    m_nProcessedEvents(0),
    m_nRegisteredObjs(0),
    m_nUnregisterCalls(0),
    m_stats(stats),
    m_nThreads(nThreads),
    m_tid( nThreads ),
    m_workers( NULL )
{
    m_workers = new worker_thread_t *[ m_nThreads ];

    pthread_barrier_init( &m_syncpoint, NULL, m_nThreads + 1 ); // + 1 for simulation manager thread
    pthread_mutex_init( &m_mutObjID, NULL );

    pthread_once( &common_key_once, make_thread_key );
    pthread_setspecific( common_thread_key, (void *) &m_tid );
}

template < class E, class O >
ThreadedCentralizedSimulationManager<E,O>::~ThreadedCentralizedSimulationManager() {
    for( unsigned int i = 0; i < m_nThreads; ++i ) {
        delete m_workers[i];
    }

    delete [] m_workers;

    pthread_barrier_destroy( &m_syncpoint );

    pthread_once( &common_destroy_once, destroy_thread_key );
}

template < class E, class O >
typename ThreadedCentralizedSimulationManager< E, O>::object_t * ThreadedCentralizedSimulationManager<E, O>::getSimulationObject() {
    system_id::manager_id_t mid = this->getManagerID();
    system_id::object_id_t oid = m_objects.size();

    object_t * obj = new object_t( this, mid, oid );

    //m_objects.push_back( make_pair( obj, ltsf_pool_t::UNKNOWN_INDEX ) );
    m_objects.push_back( make_pair( obj, false ) );
    registerObject( oid );

    return obj;
}

template< class E, class O >
void ThreadedCentralizedSimulationManager<E, O>::registerObject( object_t * obj ) {
    if( obj == NULL ) return;

    system_id::object_id_t oid = obj->getObjectID();
    registerObject( oid );
}

template< class E, class O >
void ThreadedCentralizedSimulationManager<E, O>::registerObject( size_t idx ) {
    if( !m_objects[idx].second ) {
        m_objects[idx].second = true;
        ++m_nRegisteredObjs;
    }
}

template< class E, class O >
void ThreadedCentralizedSimulationManager<E, O>::unregisterObject( object_t * obj ) {
    if( obj == NULL ) return;

    system_id::object_id_t id = obj->getObjectID();
    unregisterObject(id);
}

template < class E, class O >
void ThreadedCentralizedSimulationManager<E, O>::unregisterObject( size_t idx ) {
    ++m_nUnregisterCalls;
    if( m_objects[ idx ].second ) {
        object_pool_pair_t & o_pair = m_objects[idx];
        object_t * obj = o_pair.first;

        system_id id = obj->getSystemID();

        m_nPendingEvents += obj->pendingEventCount(id);
        m_nProcessedEvents += obj->processedEventCount(id);

        o_pair.second = false;
        --m_nRegisteredObjs;
    }
}

template< class E, class O >
size_t ThreadedCentralizedSimulationManager<E, O>::getObjectCount() const {
    return m_nRegisteredObjs;
}

template< class E, class O >
typename ThreadedCentralizedSimulationManager<E, O>::object_t * ThreadedCentralizedSimulationManager<E, O>::getObject( const system_id & id ) const {
    return m_objects[ id.getObjectID() ].first;
}

template < class E, class O >
void ThreadedCentralizedSimulationManager<E,O>::initialize( ) {
    m_stats->startPhase( INIT_PHASE_K );
    
    for( unsigned int i = 0; i < m_nThreads; ++i ) {
        m_workers[i] = new worker_thread_t( this, i, m_nThreads );
    }

    sync();

    m_app->initialize();
    m_stats->stopPhase( INIT_PHASE_K );
}

template < class E, class O >
void ThreadedCentralizedSimulationManager<E,O>::simulate( const vtime_t & until ) {
    setSimulateUntil( until );
    
    m_stats->startPhase( SIMULATE_PHASE_K );

    typename ltsf_pool_t::iterator it = m_pooled_events.begin();
    vtime_t timestamp = ((it == m_pooled_events.end()) ? SystemClock::POSITIVE_INFINITY : it->first );

    while( timestamp != SystemClock::POSITIVE_INFINITY && !setSimulationTime( timestamp ) ) {
        paged_event_pool_t * tmp_evts = it->second;

        while( tmp_evts->hasMoreEvents() ) {
            event_page_t * ep = tmp_evts->getEventPage();
            for( unsigned int i = 0; i < m_nThreads; ++i ) {
                m_workers[i]->runNextObject( timestamp, ep );
            }

            sync();
        }

        m_pooled_events.erase(it);

        it = m_pooled_events.begin();
        timestamp = ((it == m_pooled_events.end()) ? SystemClock::POSITIVE_INFINITY : it->first );
    }

    for( unsigned int i = 0; i < m_nThreads; ++i ) {
        m_workers[i]->stop();
    }

    sync();

    m_stats->stopPhase( SIMULATE_PHASE_K );
}

template < class E, class O >
void ThreadedCentralizedSimulationManager< E, O >::routeEvent( event_t * evt ) {
    routeEvent( evt, evt->getReceiver(), evt->getReceived() );
}

template < class E, class O >
void ThreadedCentralizedSimulationManager< E, O >::routeEvent( const system_id & id, event_t * evt ) {
    routeEvent( evt, id, evt->getReceived() );
}

template < class E, class O >
void ThreadedCentralizedSimulationManager< E, O >::routeEvent( event_t * evt, const system_id & id, const vtime_t & rt ) {
    unsigned int tid = *((unsigned int * ) pthread_getspecific(common_thread_key) );
    
    typename ltsf_pool_t::iterator it = m_pooled_events.find( rt );
    if( it == m_pooled_events.end() ) {
        // sufficient to only lock this block
        std::pair< typename ltsf_pool_t::iterator, bool > res = m_pooled_events.insert( make_pair( rt, (paged_event_pool_t * )NULL ));
        it = res.first;
        if( res.second ) {
            // delay allocation as long as possible
            it->second = new paged_event_pool_t( &m_page_manager, m_nThreads + 1 );
        }
    }

    it->second->append( evt, id, tid );
}

template < class E, class O >
void ThreadedCentralizedSimulationManager<E,O>::sync() {
    pthread_barrier_wait( &m_syncpoint );
}

template < class E, class O >
void ThreadedCentralizedSimulationManager<E,O>::notifyNextEvent( const system_id & obj, const event::vtime_t & t ) {
    assert(false);
}

template< class E, class O >
void ThreadedCentralizedSimulationManager<E, O>::finalize() {
    m_stats->startPhase( FINALIZE_PHASE_K );

    m_app->finalize();

    while( !m_pooled_events.empty() ) {
        paged_event_pool_t * tmp = m_pooled_events.begin()->second;
        m_pooled_events.erase( m_pooled_events.begin() );

//        m_nPendingEvents += tmp->size();
        delete tmp;
    }

    for( unsigned int i = 0; i < m_nThreads; ++i ) {
        m_nProcessedEvents += m_workers[i]->getProcessedCount();
    }

    m_stats->stopPhase( FINALIZE_PHASE_K );

    m_stats->setProcessedEvents( m_nProcessedEvents );
    m_stats->setPendingEvents( m_nPendingEvents );
}

template< class E, class O >
bool ThreadedCentralizedSimulationManager<E, O>::isSimulationComplete() const {
    return m_sim_complete;
}

template< class E, class O >
bool ThreadedCentralizedSimulationManager<E, O>::setSimulationTime( const vtime_t & t ) {
    assert( m_sim_time <= t );
    if( m_sim_time != t ) {
        m_sim_time = t;
        m_sim_complete = (m_sim_time >= m_sim_until);
    }

    return m_sim_complete;
}

template< class E, class O >
void ThreadedCentralizedSimulationManager<E, O>::setSimulateUntil( const vtime_t & t ) {
    m_sim_until = t;
}

template< class E, class O >
const typename ThreadedCentralizedSimulationManager<E,O>::vtime_t & ThreadedCentralizedSimulationManager<E, O>::getSimulationTime() const {
    return m_sim_time;
}
#endif  // THREADED_CENTRALIZED_SIMULATION_MANAGER2_HPP_
