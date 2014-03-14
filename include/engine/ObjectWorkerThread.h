#ifndef OBJECT_WORKER_THREAD_H_
#define OBJECT_WORKER_THREAD_H_

#include <pthread.h>
#include <cassert>

#include "common_thread.h"
#include "object.h"
#include "event_interface.h"

template < class G >
class ObjectWorkerThread {
public:
    typedef G object_group_t;

    ObjectWorkerThread( synchronizer * sync_point, unsigned int id, unsigned int pool_size );

    void runNextObject( const event::vtime_t & , const object_group_t * objects );

    void stop();
    void force_stop();

    virtual ~ObjectWorkerThread();

protected:
    static void * start( void * arguments );

    void run();

    synchronizer *  m_sync_point;

    unsigned int m_id;
    unsigned int m_pool_size;
    pthread_t *  m_thread;

    pthread_mutex_t m_mutWait;
    pthread_cond_t  m_cond_wait;

    bool m_done, m_waiting, m_sync;

    event::vtime_t              m_cur_time;
    const object_group_t *      m_cur_objs;
};

//
// Implementation
//

template < class G >
void * ObjectWorkerThread<G>::start( void * arguments ) {
    ObjectWorkerThread<G> * owt = static_cast< ObjectWorkerThread<G> *>( arguments );

    if( owt ) {
        owt->run();
    }

    return NULL;
}

template < class G>
ObjectWorkerThread<G>::ObjectWorkerThread( synchronizer * sync_point, unsigned int id, unsigned int pool_size ) :
    m_sync_point( sync_point ),
    m_id( id ),
    m_pool_size( pool_size ),
    m_thread( new pthread_t() ),
    m_done(false), 
    m_waiting(false), 
    m_sync(true),
    m_cur_time( SystemClock::POSITIVE_INFINITY ),
    m_cur_objs(NULL)
{
    pthread_mutex_init( &m_mutWait, NULL );
    pthread_cond_init( &m_cond_wait, NULL );

    int rVal = pthread_create( m_thread, NULL, &start, (void *) this );

    assert( rVal == 0);
}

template< class G > 
ObjectWorkerThread<G>::~ObjectWorkerThread() {
    force_stop();

    void * status;
    int rVal = pthread_join( *m_thread, &status );

    assert( rVal == 0 );

    pthread_cond_destroy( &m_cond_wait );
    pthread_mutex_destroy( &m_mutWait );
}

template < class G >
void ObjectWorkerThread<G>::runNextObject( const event::vtime_t & t, const object_group_t * objs ) {
    pthread_mutex_lock( &m_mutWait );
    m_cur_time = t;
    m_cur_objs = objs;

    pthread_cond_signal( &m_cond_wait );
    pthread_mutex_unlock( &m_mutWait );
}

template < class G >
void ObjectWorkerThread<G>::stop() {
    if( m_done ) return;

    pthread_mutex_lock( &m_mutWait );

    m_done = true;

    m_cur_time = SystemClock::POSITIVE_INFINITY;
    m_cur_objs = NULL;

    pthread_cond_signal( &m_cond_wait );
    pthread_mutex_unlock( &m_mutWait );
}

template < class G >
void ObjectWorkerThread<G>::force_stop() {
    if( m_done ) return;

    pthread_mutex_lock( &m_mutWait );

    m_done = true;
    m_sync = false;

    m_cur_time = SystemClock::POSITIVE_INFINITY;
    m_cur_objs = NULL;
    
    pthread_cond_signal( &m_cond_wait );
    pthread_mutex_unlock( &m_mutWait );
}

template < class G >
void ObjectWorkerThread<G>::run() {
    pthread_once( &common_key_once, make_thread_key );
    pthread_setspecific( common_thread_key, (void *) &m_id );

    pthread_mutex_lock( &m_mutWait );
    m_sync_point->sync();

    while( !m_done ) {
        pthread_cond_wait( &m_cond_wait, &m_mutWait );

        if( m_cur_time != SystemClock::POSITIVE_INFINITY && m_cur_objs ) {

            for( size_t i = m_id; i < m_cur_objs->size(); i += m_pool_size ) {
                object * tmp = (*m_cur_objs)[i];

                tmp->updateLocalTime( m_cur_time );
                tmp->process();
            }

            m_cur_time = SystemClock::POSITIVE_INFINITY;
            m_cur_objs = NULL;
        }

        if( m_sync ) m_sync_point->sync();
    }

    pthread_mutex_unlock( &m_mutWait );

    pthread_exit(NULL);
}

#endif  // OBJECT_WORKER_THREAD_H_
