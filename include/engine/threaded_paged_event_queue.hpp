#ifndef THREADED_PAGED_EVENT_QUEUE_HPP_
#define THREADED_PAGED_EVENT_QUEUE_HPP_

#include <iostream>

#include "event_page_manager.hpp"
#include <utility>
#include <cstring>

template < class EVT, class OBJ >
class ThreadedPagedEventQueue {
public:
    typedef EventPageManager< EVT, OBJ > event_page_manager_t;
    typedef typename event_page_manager_t::event_page_t event_page_t;
    typedef typename event_page_t::iterator iterator;

    typedef EVT event_t;
    typedef OBJ object_t;

    ThreadedPagedEventQueue( event_page_manager_t * epm, unsigned int threads = 1 ) : 
        m_page_manager( epm ),
        m_read( NULL ),
        m_tail( NULL ),
        m_nThreads(threads),
        m_thread_write( NULL ),
        m_bMoreEvents(false)
    {
        m_tail = m_page_manager->getOrCreate();
        m_read = m_page_manager->getOrCreate();
        m_read->setNextPage( m_tail );

        if( m_nThreads == (unsigned int) -1 || m_nThreads  <= 1 ) return;

        m_thread_write = new event_page_t *[ threads ];
        memset( m_thread_write, 0, sizeof( event_page_t * ) * m_nThreads );
    }

    void append( event_t * evt, const object_t & obj, unsigned int thread_idx = 0 ) {
        event_page_t * tmp = m_tail;

        if( m_thread_write != NULL ) {
            tmp = m_thread_write[ thread_idx ];

            if( tmp == NULL || tmp->isFull() ) {
                // the following should be performed in an atomic (locked) block
                // pthread_mutex_lock( swapping_write_page )

                tmp = m_page_manager->getOrCreate();    // allocate a new tail

                m_thread_write[ thread_idx ] = m_tail;  // update the threads write page as the old tail
                m_tail->setNextPage( tmp );             // append the new tail to the end of the list
                m_tail = tmp;                           // update the tail
                tmp = m_thread_write[ thread_idx ];     // update tmp to the old tail
                // pthread_mutex_unlock( swapping_write_page )
            }
        } else if( tmp->isFull() ) {
            tmp = m_page_manager->getOrCreate();
            m_tail->setNextPage(tmp);
            m_tail = tmp;
        }
        
        assert( tmp->addEvent( evt, obj ) );
        m_bMoreEvents = true;
    }

    event_page_t * getEventPage() {
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
                m_tail = m_page_manager->getOrCreate();
                m_read->setNextPage(m_tail);
                m_bMoreEvents = !m_read->isEmpty();
            }
        }
        return t;
    }

    bool hasMoreEvents() const {
        return m_bMoreEvents;
    }

    void reset() {
        while( m_read != NULL ) {
            event_page_t * tmp = m_read;
            m_read = m_read->getNextPage();

            m_page_manager->release( tmp );
        }

        m_read = m_page_manager->getOrCreate();
        m_tail = m_page_manager->getOrCreate();
        m_read->setNextPage(m_tail);

        m_bMoreEvents = false;

    }

    virtual ~ThreadedPagedEventQueue() {
        event_page_t * page = m_read;
        while( page != NULL ) {
            event_page_t * tmp = page;
            page = page->getNextPage();

            m_page_manager->release(tmp);
        }
        m_read = NULL;
        m_tail = NULL;
    }
protected:
    event_page_manager_t    * m_page_manager;
    event_page_t            * m_read, * m_tail;

    size_t                  m_nThreads;
    event_page_t            ** m_thread_write;
    bool                    m_bMoreEvents;

};

#endif  // THREADED_PAGED_EVENT_QUEUE_HPP_
