#ifndef PAGED_EVENT_STACK_HPP_
#define PAGED_EVENT_STACK_HPP_

#include <iostream>

//#include "event_page_manager.hpp"
#include "event_page.h"
#include <utility>

template < class EVT, class OBJ >
class PagedEventQueue {
public:
    typedef EventPage< EVT, OBJ > event_page_t;
    typedef typename event_page_t::iterator iterator;

    typedef EVT event_t;
    typedef OBJ object_t;

    PagedEventQueue( ) : 
        m_read( NULL ),
        m_tail( NULL ),
        m_nSize( 0 )
    { }

    void append( event_t * evt, const object_t & obj ) {
        if( m_tail == NULL ) {
            m_tail = new event_page_t();
        } else if( m_tail->isFull() ) {
            //std::cout << "Page full " << m_tail->getEventCount() << " allocating new page " << std::endl;
            event_page_t * tmp = new event_page_t();
            m_tail->setNextPage(tmp);
            if( m_read == NULL ) {
                m_read = m_tail;
            }
            m_tail = tmp;
            assert( m_tail->addEvent( evt, obj ) );
        } else if( !m_tail->addEvent( evt, obj ) ) {
            event_page_t * tmp = new event_page_t();
            m_tail->setNextPage(tmp);
            if( m_read == NULL ) {
                m_read = m_tail;
            }
            m_tail = tmp;
            assert( m_tail->addEvent( evt, obj ) );
        }
        ++m_nSize;
    }

    event_page_t * getReadPage() {
        if( m_read == NULL ) {
            if( m_tail == NULL ) {
                return NULL;
            }

            event_page_t * tmp = m_tail;
            m_tail = NULL;

            m_nSize -= tmp->getEventCount();
            return tmp;
        }

        event_page_t * tmp = m_read;

        if( m_read == m_tail ) {
            m_tail = NULL;
        }
        m_read = m_read->getNextPage();

        m_nSize -= tmp->getEventCount();
        return tmp;
    }

    size_t size() const {
        return m_nSize;
    }

    void reset() {
        while( m_read != NULL ) {
            event_page_t * tmp = m_read;
            m_read = m_read->getNextPage();

            if( m_read == NULL ) {
                assert( tmp == m_tail );
                m_tail = NULL;
            }
            delete tmp;
        }

        if( m_tail != NULL ) {
            delete m_tail;
        }
        m_nSize = 0;
    }

    virtual ~PagedEventQueue() {
        reset();
    }
protected:
    event_page_t            * m_read, * m_tail;
    size_t                  m_nSize;
};

#endif  // PAGED_EVENT_STACK_HPP_
