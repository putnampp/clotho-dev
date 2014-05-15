#ifndef EVENT_PAGE_HPP_
#define EVENT_PAGE_HPP_

#include "memory_page.h"

#include <ostream>

// BLOCK_SIZE == 4084 (4096-12) => Max unique objects == 127 / page

template < class EVT, class OBJ >
class EventPage {
public:
    typedef EVT event_t;
    typedef OBJ object_t;

    struct event_node {
        event_t *       p_event;
        event_node *    next;
        event_node( event_t * e = NULL, event_node * n = NULL ) : p_event(e), next(n) {}
    };

    struct object_node {
        object_t        object_id;
        event_node *    enode;
    };

    typedef std::pair< object_node *, event_node * > next_event_t;

    EventPage() {}

    unsigned short getFreeSpace() const {
        return (m_page.header.free_end - m_page.header.free_start);
    }

    bool isFull() const {
        static const offset_t buffer_zone_size = sizeof(object_node) + sizeof( event_node );
        return getFreeSpace() < buffer_zone_size;
    }

    bool addEvent( const event_t * e, const object_t & object_id ) {
        offset_t n_free_start = m_page.header.free_start;
        offset_t n_free_end = m_page.header.free_end - sizeof( event_node );

        if( n_free_start >= n_free_end ) return false;  // page full

        object_node * obj = findObject( object_id );

        if( obj == NULL ) {
            // object does not exist on current page
            n_free_start += sizeof( object_node );
        }

        bool res = (n_free_start < n_free_end); // is there sufficient space to schedule event?
        if( res ) {
            // sufficient space to add event to page
            
            event_node * nevt = reinterpret_cast< event_node * >(&m_page.block[ n_free_end ]);
            nevt->p_event = e;

            if( obj == NULL ) {
                obj = reinterpret_cast< object_node * >(&m_page.block[m_page.header.free_start]);

                obj->object_id = object_id;
                m_page.header.free_start = n_free_start;

                nevt->next = BLOCK_SIZE;
            } else {
                // object already exists
                //
                nevt->next = obj->event_offset;
            }
            obj->event_offset = n_free_end;
            
            m_page.header.free_start = n_free_start;
            m_page.header.free_end = n_free_end;
        }

        return res;
    }

    void reset() {
        m_page.header.free_start = 0;
        m_page.header.free_end = BLOCK_SIZE;
    }

    void dump( std::ostream & out ) {}

protected:
    object_node * findObject( const object_t & object_id ) {
        if( m_page.header.free_start != 0 ) {
            object_node * fstart = reinterpret_cast< object_node *>( &m_page.block[m_page.header.free_start]);
            object_node * obj = reinterpret_cast< object_node *>(m_page.block);
            do {
                if( obj->object_id == object_id ) {
                    // consider swapping obj with start of the object node list
                    return obj;
                }
                obj++;
            } while( obj < fstart );

        }
        return NULL;
    }

    memory_page m_page;

    std::pair< object_node *, event_node * > m_cur_pair;
};
#endif  // EVENT_PAGE_HPP_
