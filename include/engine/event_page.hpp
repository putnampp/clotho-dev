#ifndef EVENT_PAGE_HPP_
#define EVENT_PAGE_HPP_

#include "memory_page.h"

#include <ostream>

template < class EVT, class OBJ >
class EventPage {
public:
    typedef EVT event_t;
    typedef OBJ object_t;

    struct event_node {
        event_t *   p_event;
//        offset_t    next;
        event_node * next;
    };

    struct object_node {
        object_t object_id;
//        offset_t event_offset;
        event_node * enode;
    };

    typedef std::pair< object_node *, event_node * > next_event_t;

    EventPage() {}

    size_t getFreeSpace() const {
        return (m_page.header.free_end - m_page.header.free_start);
    }

    bool isFull() const {
        static const size_t buffer_zone_size = sizeof(object_node) + sizeof( event_node );
        return getFreeSpace() < buffer_zone_size;
    }

    bool addEvent( event_t * e, const object_t & object_id ) {
        if( isFull() ) return false;  // page full

        char * n_free_start = m_page.header.free_start;
        char * n_free_end = m_page.header.free_end - sizeof( event_node );

        object_node * obj = findObject( object_id );

        if( obj == NULL ) {
            // object does not exist on current page
            n_free_start += sizeof( object_node );
        }

        bool res = (n_free_start < n_free_end); // is there sufficient space to schedule event?
        if( res ) {
            // sufficient space to add event to page
            
            event_node * nevt = reinterpret_cast< event_node * >(n_free_end);
            nevt->p_event = e;

            if( obj == NULL ) {
                obj = reinterpret_cast< object_node * >(m_page.header.free_start);

                obj->object_id = object_id;

                nevt->next = NULL;
            } else {
                // object already exists
                //
                nevt->next = obj->enode;
            }
            obj->enode = nevt;
            
            m_page.header.free_start = n_free_start;
            m_page.header.free_end = n_free_end;
        }

        return res;
    }

    void reset() {
        m_page.clear();
    }

    void dump( std::ostream & out ) {
        out << "Object Node Size: " << sizeof(object_node) << "\n";
        out << "Event Node Size: " << sizeof(event_node) << "\n";

        if( m_page.header.free_start != m_page.block ) {
            object_node * onode = reinterpret_cast< object_node * >( m_page.block );
            void * stop = m_page.header.free_start;
            do {
                out << onode->object_id << ":\n";

                if( onode->enode != NULL ) {
                    event_node * enode = onode->enode;
                    do {
                        out << ((unsigned long) enode) << ":" << *(enode->p_event) << ",";
                        enode = enode->next;
                    } while( enode != NULL );
                    out << "\n";
                } else {
                    out << "Has no events\n";
                }
                onode++;
            } while( onode < stop );
        } else {
            out << "Page is empty\n";
        }
    }

protected:
    object_node * findObject( const object_t & object_id ) {
        if( m_page.header.free_start != m_page.block ) {
            object_node * obj = reinterpret_cast< object_node *>(m_page.block);
            object_node * stop = reinterpret_cast< object_node *>( m_page.header.free_start);
            do {
                if( obj->object_id == object_id ) {
                    // consider swapping obj with start of the object node list
                    return obj;
                }
                obj++;
            } while( obj < stop );

        }
        return NULL;
    }

    memory_page m_page;

    std::pair< object_node *, event_node * > m_cur_pair;
};
#endif  // EVENT_PAGE_HPP_
