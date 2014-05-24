#ifndef THREADED_PAGED_EVENT_QUEUE_HPP_
#define THREADED_PAGED_EVENT_QUEUE_HPP_

#include <pthread.h>
#include <iostream>

//#include "event_page_manager.hpp"

#include <utility>
#include <cstring>

#include "event_page.hpp"

template < class EVT, class OBJ >
class ThreadedPagedEventQueue {
public:
//    typedef EventPageManager< EVT, OBJ > event_page_manager_t;
//    typedef typename event_page_manager_t::event_page_t event_page_t;
//
    typedef EventPage< EVT, OBJ > event_page_t;
    typedef typename event_page_t::iterator iterator;

    typedef EVT event_t;
    typedef OBJ object_t;

    //ThreadedPagedEventQueue( event_page_manager_t * epm, unsigned int threads = 1 ) : 
    //    m_page_manager( epm ),
    ThreadedPagedEventQueue( unsigned int threads = 1 ) :
        m_read( NULL ),
        m_tail( NULL ),
        m_nThreads(threads),
        m_thread_write( NULL ),
        m_bMoreEvents(false)
    {
        m_tail = new event_page_t();
        m_read = new event_page_t( m_tail );

        if( m_nThreads == (unsigned int) -1 || m_nThreads  <= 1 ) return;

        m_thread_write = new event_page_t *[ threads ];
        memset( m_thread_write, 0, sizeof( event_page_t * ) * m_nThreads );

        pthread_mutex_init( &m_mutLock, NULL );
    }

    void append( event_t * evt, const object_t & obj, unsigned int thread_idx = 0 ) {
        pthread_mutex_lock( &m_mutLock );

        event_page_t * tmp = m_tail;

        if( m_thread_write != NULL ) {
            tmp = m_thread_write[ thread_idx ];

            if( tmp == NULL || tmp->isFull() ) {
                // the following should be performed in an atomic (locked) block

//                tmp = m_page_manager->getOrCreate();    // allocate a new tail

                tmp = m_tail;

                m_thread_write[ thread_idx ] = m_tail;  // update the threads write page as the old tail
                m_tail = new event_page_t();            // create new tail
                tmp->setNextPage( m_tail );             // link the old to the new

            }
        } else if( tmp->isFull() ) {

            //tmp = m_page_manager->getOrCreate();
            tmp = new event_page_t();
            m_tail->setNextPage(tmp);
            m_tail = tmp;
        }
        
        m_bMoreEvents = true;
        pthread_mutex_unlock( &m_mutLock );

        assert( tmp->addEvent( evt, obj ) );
    }

    event_page_t * getEventPage() {
        pthread_mutex_lock( &m_mutLock );
        event_page_t * t = m_read;
        if( m_read != NULL ) {
            if( m_thread_write != NULL ) {
                unsigned int i = m_nThreads;
                while( i ) {
                    if( m_thread_write[ --i ] == m_read ) {
                        // should be done atomically
                        m_thread_write[i] = NULL;
                        break;
                    }
                }
            }

            m_read = m_read->getNextPage();

            if( m_read == m_tail ) {
                m_tail = new event_page_t();
                m_read->setNextPage(m_tail);
            }
            m_bMoreEvents = m_read != NULL && !m_read->isEmpty();
        } 
        pthread_mutex_unlock( &m_mutLock );
        return t;
    }

    bool hasMoreEvents() const {
        return m_bMoreEvents;
    }

    void reset() {
        pthread_mutex_lock( &m_mutLock );
        while( m_read != NULL ) {
            event_page_t * tmp = m_read;
            m_read = m_read->getNextPage();

            delete tmp;
            //m_page_manager->release( tmp );
        }

        m_tail = new event_page_t();
        m_read = new event_page_t(m_tail);

        m_bMoreEvents = false;
        pthread_mutex_unlock( &m_mutLock );
    }

    virtual ~ThreadedPagedEventQueue() {
        event_page_t * page = m_read;
        while( page != NULL ) {
            event_page_t * tmp = page;
            page = page->getNextPage();

            //m_page_manager->release(tmp);
            delete tmp;
        }
        m_read = NULL;
        m_tail = NULL;
    }

protected:
//    event_page_manager_t    * m_page_manager;
    event_page_t            * m_read, * m_tail;

    size_t                  m_nThreads;
    event_page_t            ** m_thread_write;
    bool                    m_bMoreEvents;
    pthread_mutex_t         m_mutLock;
};

#endif  // THREADED_PAGED_EVENT_QUEUE_HPP_
