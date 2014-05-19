#ifndef EVENT_PAGE_MANAGER_H_
#define EVENT_PAGE_MANAGER_H_

#include "event_page.hpp"

template < class EVT, class OBJ >
class EventPageManager {
public:
    typedef EventPage< EVT, OBJ > event_page_t;

    EventPageManager( ) : m_free_list( NULL ) {}

    event_page_t * getOrCreate() {
        event_page_t * t = NULL;
        if( m_free_list == NULL ) {
            t = new event_page_t();
        } else {
            t = m_free_list;
            m_free_list = m_free_list->getNextPage();
            t->reset();
        }
        return t;
    }

    void release( event_page_t * ep ) {
        ep->setNextPage( m_free_list );
        m_free_list = ep;
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
    event_page_t *    m_free_list;
};

#endif  // EVENT_PAGE_MANAGER_H_
