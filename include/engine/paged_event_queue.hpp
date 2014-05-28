#ifndef PAGED_EVENT_STACK_HPP_
#define PAGED_EVENT_STACK_HPP_

#include <iostream>

#include "event_page_manager.hpp"
#include <utility>

template < class EVT, class OBJ >
class PagedEventQueue {
public:
    typedef EventPageManager< EVT, OBJ > event_page_manager_t;
    typedef typename event_page_manager_t::event_page_t event_page_t;
    typedef typename event_page_t::iterator iterator;

    typedef EVT event_t;
    typedef OBJ object_t;

    PagedEventQueue( event_page_manager_t * epm ) : 
        m_page_manager( epm ),
        m_read( NULL ),
        m_tail( NULL ),
        m_next_evt( NULL ),
        m_read_it(NULL),
        m_nSize(0)
    {
        m_read = m_page_manager->getOrCreate();
        m_tail = m_page_manager->getOrCreate();
        m_read->setNextPage( m_tail );

        m_read_it = m_read->end();
    }

    void append( event_t * evt, const object_t & obj ) {
        if( m_tail->isFull() ) {
            //std::cout << "Page full " << m_tail->getEventCount() << " allocating new page " << std::endl;
            event_page_t * tmp = m_page_manager->getOrCreate();
            m_tail->setNextPage(tmp);
            m_tail = tmp;
        }

        assert( m_tail->addEvent( evt, obj ) );

        ++m_nSize;
    }

    event_t * peek() {
        if( !updateReadPage() ) {
            return NULL;
        }
        
        return m_next_evt;
    }

    event_t * next( ) {
        if( !updateReadPage() ) {
            std::cout << "No more events" << std::endl;
            return NULL;
        }
        
        event_t * tmp = m_next_evt;

        ++m_read_it;
        --m_nSize;

        return tmp;
    }

    size_t size() const {
        return m_nSize;
    }

    void reset() {
        while( m_read != NULL ) {
            event_page_t * tmp = m_read;
            m_read = m_read->getNextPage();

            m_page_manager->release( tmp );
        }

        m_read = m_page_manager->getOrCreate();
        m_tail = m_page_manager->getOrCreate();
        m_read->setNextPage( m_tail );

        m_read_it = m_read->end();

        m_nSize = 0;
    }

    virtual ~PagedEventQueue() {
        event_page_t * page = m_read;
        while( page != NULL ) {
            event_page_t * tmp = page;
            page = page->getNextPage();

            tmp->reset();

            m_page_manager->release(tmp);
        }
        m_read = NULL;
    }
protected:
    bool updateReadPage() {
        assert( m_read != NULL );
        if( m_read_it == m_read->end() ) {
            if( m_read->getNextPage() == m_tail && m_tail->isEmpty() ) {
                std::cout << "next page is tail. and tail is empty" << std::endl;
                return false;
            }

            event_page_t * tmp = m_read;
            m_read = m_read->getNextPage();

            if( m_read != NULL ) {
                m_read_it = m_read->begin();
                if( m_read == m_tail ) {
                    // there always needs to be a writeable page in the tail position
                    // tmp was the last read page
                    // m_read is also the tail of the list
                    assert( m_read->getNextPage() == NULL );
                    m_read->setNextPage(tmp);
                    m_tail = tmp;
                    m_tail->reset();
                } else {
                    // there was a pending page in between read and tail
                    // just release tmp
                    m_page_manager->release( tmp );
                }
            } else {
                assert( false ); // this shouldn't happen
            }
        }
        
        if( m_read_it == m_read->end() ) {
            return false;
        }

        m_next_evt = m_read_it.getEvent();
        return true;
    }
    
    event_page_manager_t    * m_page_manager;
    event_page_t            * m_read, * m_tail;
    event_t                 * m_next_evt;

    iterator  m_read_it;

    size_t  m_nSize;
};

#endif  // PAGED_EVENT_STACK_HPP_
