#ifndef PAGE_WORKER_THREAD_H_
#define PAGE_WORKER_THREAD_H_

#include <pthread.h>
#include <cassert>
#include <type_traits>

#include "common_thread.h"
#include "synchronizer.h"
#include "event_page.h"

template < class E, class O, class SM >
class PageWorkerThread {
public:
    typedef O object_id_t;
    typedef E event_t;
    typedef SM simulation_manager_t;

    typedef EventPage< E, O > event_page_t;
   

//    PageWorkerThread( simulation_manager_t * sm, unsigned int id, unsigned int pool_size );
    PageWorkerThread( simulation_manager_t * sm, unsigned int id, unsigned int pool_size );

    void runNextObject( const event::vtime_t & , event_page_t * objects );

    void stop();
    void force_stop();

    size_t getProcessedCount() const { return m_nProcessed; }

    virtual ~PageWorkerThread();

protected:
    static void * start( void * arguments );

    void run();

    simulation_manager_t *  m_sm;

    unsigned int m_id;
    unsigned int m_pool_size;
    pthread_t *  m_thread;

    pthread_mutex_t m_mutWait;
    pthread_cond_t  m_cond_wait;

    bool m_done, m_waiting, m_sync;

    event::vtime_t          m_cur_time;
    event_page_t            * m_cur_objs;

    size_t                  m_nProcessed;
};

//
// Implementation
//

template < class E, class O, class SM >
void * PageWorkerThread<E,O,SM>::start( void * arguments ) {
    PageWorkerThread<E,O,SM> * owt = static_cast< PageWorkerThread<E,O,SM> *>( arguments );

    if( owt ) {
        owt->run();
    }

    return NULL;
}

template < class E, class O, class SM >
PageWorkerThread<E,O,SM>::PageWorkerThread( simulation_manager_t * sm, unsigned int id, unsigned int pool_size ) :
    m_sm( sm ),
    m_id( id ),
    m_pool_size( pool_size ),
    m_thread( new pthread_t() ),
    m_done(false), 
    m_waiting(false), 
    m_sync(true),
    m_cur_time( SystemClock::POSITIVE_INFINITY ),
    m_cur_objs(NULL),
    m_nProcessed(0)
{
    pthread_mutex_init( &m_mutWait, NULL );
    pthread_cond_init( &m_cond_wait, NULL );

    int rVal = pthread_create( m_thread, NULL, &start, (void *) this );

    assert( rVal == 0);
}

template < class E, class O, class SM > 
PageWorkerThread<E,O,SM>::~PageWorkerThread() {
    force_stop();

    void * status;
    int rVal = pthread_join( *m_thread, &status );

    assert( rVal == 0 );

    pthread_cond_destroy( &m_cond_wait );
    pthread_mutex_destroy( &m_mutWait );
}

template < class E, class O, class SM >
void PageWorkerThread<E,O,SM>::runNextObject( const event::vtime_t & t, event_page_t * objs ) {
    pthread_mutex_lock( &m_mutWait );
    m_cur_time = t;
    m_cur_objs = objs;

    pthread_cond_signal( &m_cond_wait );
    pthread_mutex_unlock( &m_mutWait );
}

template < class E, class O, class SM >
void PageWorkerThread<E,O,SM>::stop() {
    if( m_done ) return;

    pthread_mutex_lock( &m_mutWait );

    m_done = true;

    m_cur_time = SystemClock::POSITIVE_INFINITY;
    m_cur_objs = NULL;

    pthread_cond_signal( &m_cond_wait );
    pthread_mutex_unlock( &m_mutWait );
}

template < class E, class O, class SM >
void PageWorkerThread<E,O,SM>::force_stop() {
    if( m_done ) return;

    pthread_mutex_lock( &m_mutWait );

    m_done = true;
    m_sync = false;

    m_cur_time = SystemClock::POSITIVE_INFINITY;
    m_cur_objs = NULL;
    
    pthread_cond_signal( &m_cond_wait );
    pthread_mutex_unlock( &m_mutWait );
}

template < class E, class O, class SM >
void PageWorkerThread<E,O,SM>::run() {
    pthread_once( &common_key_once, make_thread_key );
    pthread_setspecific( common_thread_key, (void *) &m_id );

    pthread_mutex_lock( &m_mutWait );
    m_sm->sync();

    while( !m_done ) {
        pthread_cond_wait( &m_cond_wait, &m_mutWait );

        if( m_cur_time != SystemClock::POSITIVE_INFINITY && m_cur_objs ) {
            for( size_t i = m_id; i < m_cur_objs->getObjectCount(); i += m_pool_size ) {
                typename event_page_t::iterator it = m_cur_objs->getEventsByObjectIndex( i );

                if( it != m_cur_objs->end() ) {
                    object_id_t oid = it.getObjectID();
                    typename SM::object_t * obj = m_sm->getObject( oid );
                    obj->updateLocalTime( m_cur_time );
                    do {
                        event_t * e = it.getEvent();
                        if( e != NULL ) {
                            obj->getEventPerformer()->perform_event(e);
                            ++m_nProcessed;
                            delete e;
                        }
                    } while( ++it != m_cur_objs->end());
                }
            }

            m_cur_time = SystemClock::POSITIVE_INFINITY;
            m_cur_objs = NULL;
        }

        if( m_sync ) m_sm->sync();
    }

    pthread_mutex_unlock( &m_mutWait );

    pthread_exit(NULL);
}

#endif  // PAGE_WORKER_THREAD_H_
