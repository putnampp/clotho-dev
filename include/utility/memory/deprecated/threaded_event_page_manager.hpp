#ifndef THREADED_EVENT_PAGE_MANAGER_H_
#define THREADED_EVENT_PAGE_MANAGER_H_

#include <pthread.h>
#include "event_page.hpp"

template < class EVT, class OBJ >
class ThreadedEventPageManager {
public:
    typedef EventPage< EVT, OBJ > event_page_t;

    EventPageManager( ) : m_free_list( NULL ) {
        pthread_mutex_init( &m_mutLock, NULL );
    }

    event_page_t * getOrCreate() {
        event_page_t * t = NULL;
        pthread_mutex_lock( &m_mutLock );
        if( m_free_list == NULL ) {
            t = new event_page_t();
        } else {
            t = m_free_list;
            m_free_list = m_free_list->getNextPage();
            t->reset();
        }
        pthread_mutex_unlock( &m_mutLock );
        return t;
    }

    void release( event_page_t * ep ) {
        pthread_mutex_lock( &m_mutLock );

        ep->setNextPage( m_free_list );
        m_free_list = ep;

        pthread_mutex_unlock( &m_mutLock );
    }

    virtual ~EventPageManager() {
        event_page_t * tmp = m_free_list;
        while( tmp != NULL ) {
            event_page_t * t = tmp;
            tmp = tmp->getNextPage();

            delete t;
        }
    }
protected:
    event_page_t *      m_free_list;
    pthread_mutex_t     m_mutLock;
};

#endif  // THREADED_EVENT_PAGE_MANAGER_H_

