#ifndef THREADED_CENTRALIZED_SIMULATION_MANAGER_HPP_
#define THREADED_CENTRALIZED_SIMULATION_MANAGER_HPP_

#include "CentralizedSimulationManager.hpp"
#include "synchronizer.h"

#include "ObjectWorkerThread.h"
#include "common_thread.h"

template < class ES >
class ThreadedCentralizedSimulationManager : public CentralizedSimulationManager<ES>, public synchronizer {
public:
    typedef ObjectWorkerThread< typename CentralizedSimulationManager<ES>::object_group_t > worker_thread_t;

    ThreadedCentralizedSimulationManager( shared_ptr< application >, unsigned int, system_id::manager_id_t id = 0 );
    ThreadedCentralizedSimulationManager( shared_ptr< application >, shared_ptr< SimulationStats >, unsigned int,  system_id::manager_id_t id = 0 );

    const system_id getNextObjectID();

//    virtual void registerObject( object * obj );

    virtual void initialize();
    virtual void simulate( const event::vtime_t & until );
//    virtual void finalize();

    virtual void routeEvent( const event * evt );
    virtual void notifyNextEvent( const system_id & obj, const event::vtime_t & t );

    virtual void sync();

    virtual ~ThreadedCentralizedSimulationManager();

protected:
    void clearBufferedEvents();

    unsigned int m_nThreads;
    unsigned int m_tid;

    worker_thread_t **  m_workers;

    pthread_barrier_t   m_syncpoint;
    pthread_mutex_t     m_mutObjID;

    vector< const event * > ** m_buffers;
};

//
// Implementation
//

template < class ES >
ThreadedCentralizedSimulationManager< ES >::ThreadedCentralizedSimulationManager( shared_ptr< application > app, unsigned int nThreads, system_id::manager_id_t id ) :
    CentralizedSimulationManager< ES >( app, id ),
    m_nThreads(nThreads),
    m_tid( nThreads ),
    m_workers( new worker_thread_t[ nThreads ] ),
    m_buffers( new vector< const event * >[ nThreads ] ) {
    pthread_barrier_init( &m_syncpoint, NULL, m_nThreads + 1 );
    pthread_mutex_init( &m_mutObjID, NULL );

    pthread_once( &common_key_once, make_thread_key );
    pthread_setspecific( common_thread_key, (void *) &m_tid );
}

template < class ES >
ThreadedCentralizedSimulationManager< ES >::ThreadedCentralizedSimulationManager( shared_ptr< application > app, shared_ptr< SimulationStats > stats, unsigned int nThreads, system_id::manager_id_t id ) :
    CentralizedSimulationManager< ES >( app, stats, id ),
    m_nThreads(nThreads),
    m_tid( nThreads ),
    m_workers( new worker_thread_t *[ nThreads ] ),
    m_buffers( new vector< const event *> *[ nThreads ] ) {
    pthread_barrier_init( &m_syncpoint, NULL, m_nThreads + 1 );
    pthread_mutex_init( &m_mutObjID, NULL );

    pthread_once( &common_key_once, make_thread_key );
    pthread_setspecific( common_thread_key, (void *) &m_tid );
}

template < class ES >
ThreadedCentralizedSimulationManager<ES>::~ThreadedCentralizedSimulationManager() {
    for( unsigned int i = 0; i < m_nThreads; ++i ) {
        delete m_workers[i];
    }

    delete [] m_workers;

    pthread_barrier_destroy( &m_syncpoint );

    pthread_once( &common_destroy_once, destroy_thread_key );
}

template < class ES >
const system_id ThreadedCentralizedSimulationManager<ES>::getNextObjectID() {
    pthread_mutex_lock( &m_mutObjID );

    system_id nid( this->getManagerID(), CentralizedSimulationManager<ES>::m_objects.size() );
    CentralizedSimulationManager<ES>::m_objects.push_back( make_pair( (object *)NULL, make_pair( SystemClock::POSITIVE_INFINITY, 0 ) ) );

    pthread_mutex_unlock( &m_mutObjID );
    return nid;
}

template < class ES >
void ThreadedCentralizedSimulationManager<ES>::initialize( ) {
    CentralizedSimulationManager<ES>::m_stats->startPhase( INIT_PHASE_K );

    for( unsigned int i = 0; i < m_nThreads; ++i ) {
        m_workers[i] = new worker_thread_t( this, i, m_nThreads );
        m_buffers[i] = new vector< const event * >();
    }

    sync();

    CentralizedSimulationManager<ES>::m_app->initialize();
    CentralizedSimulationManager<ES>::m_stats->stopPhase( INIT_PHASE_K );
}

template < class ES >
void ThreadedCentralizedSimulationManager<ES>::simulate( const event::vtime_t & until ) {
    CentralizedSimulationManager<ES>::setSimulateUntil( until );

    CentralizedSimulationManager<ES>::m_stats->startPhase( SIMULATE_PHASE_K );
    while(! CentralizedSimulationManager<ES>::m_ordered_objs.empty() ) {
        typename CentralizedSimulationManager<ES>::concurrent_group_t ot = CentralizedSimulationManager<ES>::getNextObjectGroup();

        if( ot.first == SystemClock::POSITIVE_INFINITY || CentralizedSimulationManager<ES>::setSimulationTime( ot.first ) ) {
            break;
        }

        for( typename CentralizedSimulationManager<ES>::object_group_t::iterator it = ot.second->begin(); it != ot.second->end(); it++ ) {
            CentralizedSimulationManager<ES>::m_objects[ (*it)->getObjectID() ].second.first = SystemClock::POSITIVE_INFINITY;
        }

        for( unsigned int i = 0; i < m_nThreads; ++i ) {
            m_workers[i]->runNextObject( ot.first, ot.second );
        }

        sync();

        clearBufferedEvents();

        while( !ot.second->empty() ) {
            object * o = ot.second->back();
            ot.second->pop_back();

            const event * e = o->peekEvent( o->getSystemID() );
            if( e != NULL ) {
                CentralizedSimulationManager<ES>::moveObject( o, e->getReceived() );
            } else {
                CentralizedSimulationManager<ES>::moveObject( o, SystemClock::POSITIVE_INFINITY );
            }
        }

        delete ot.second;
    }

    for( unsigned int i = 0; i < m_nThreads; ++i ) {
        m_workers[i]->stop();
    }

    sync();

    CentralizedSimulationManager<ES>::m_stats->stopPhase( SIMULATE_PHASE_K );
}
/*
template < class ES >
void ThreadedCentralizedSimulationManager<ES>::finalize( ) {
    CentralizedSimulationManager<ES>::m_stats->startPhase( FINALIZE_PHASE_K );

    CentralizedSimulationManager<ES>::m_app->finalize();

    CentralizedSimulationManager<ES>::m_stats->stopPhase( FINALIZE_PHASE_K );

    CentralizedSimulationManager<ES>::m_stats->setProcessedEvents( m_nProcessedEvents );
    CentralizedSimulationManager<ES>::m_stats->setPendingEvents( m_nPendingEvents );
}*/

template < class ES >
void ThreadedCentralizedSimulationManager<ES>::routeEvent( const event * evt ) {
    void * tmp = pthread_getspecific( common_thread_key );
    assert( tmp != NULL );

    unsigned int id = *((unsigned int *) tmp);

    if( id == m_tid ) {
        CentralizedSimulationManager<ES>::m_objects[ evt->getReceiver().getObjectID() ].first->receiveEvent( evt );
    } else {
        m_buffers[id]->push_back( evt );
    }
}

template < class ES >
void ThreadedCentralizedSimulationManager<ES>::sync() {
    pthread_barrier_wait( &m_syncpoint );
}

template < class ES >
void ThreadedCentralizedSimulationManager<ES>::clearBufferedEvents() {
    for( unsigned int i = 0; i < m_nThreads; ++i ) {
        while( !m_buffers[i]->empty() ) {
            const event * t = m_buffers[i]->back();
            m_buffers[i]->pop_back();

            CentralizedSimulationManager<ES>::m_objects[ t->getReceiver().getObjectID() ].first->receiveEvent( t );
        }
    }
}

template < class ES >
void ThreadedCentralizedSimulationManager<ES>::notifyNextEvent( const system_id & obj, const event::vtime_t & t ) {
    unsigned int id = *((unsigned int * ) pthread_getspecific(common_thread_key) );

    if( id == m_tid ) {
        CentralizedSimulationManager<ES>::moveObject( CentralizedSimulationManager<ES>::m_objects[ obj.getObjectID() ].first, t );
    }
}

#endif  // THREADED_CENTRALIZED_SIMULATION_MANAGER_HPP_
