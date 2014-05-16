#ifndef EVENT_PAGE_HPP_
#define EVENT_PAGE_HPP_

#include "memory_page.h"

#include "event_page_walker.hpp"

#include <ostream>
#include <iostream>

// PAGE_SIZE == 4096
// HEADER_SIZE == 24 ( next pointer + free_start pointer + free_end pointer)
// BLOCK_SIZE = PAGE_SIZE - HEADER_SIZE
// sizeof(event_node) = 16
// sizeof(object_node) = 24
//
// E[ events/object ] = 250 => Max 1 object/page;
// ...
// E[ event/object ] = 1 => Max 100 object/page;
template < class EVT, class OBJ >
class EventPage {
public:
    typedef EVT event_t;
    typedef OBJ object_t;

    struct event_node {
        event_t *   p_event;
        event_node * next;
    };

    struct object_node {
        object_t object_id;
        event_node * head, * tail;
    };

    typedef EventPageWalker<  EventPage< EVT, OBJ > > iterator;
    friend iterator;

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

        char * n_free_start = m_page.header.free_start + sizeof(event_node);
        char * n_free_end = m_page.header.free_end;

        object_node * obj = findObject( object_id );

        if( obj == NULL ) {
            // object does not exist on current page
            n_free_end -= sizeof( object_node );
        }

        bool res = (n_free_start < n_free_end); // is there sufficient space to schedule event?
        if( res ) {
            // sufficient space to add event to page
            
            event_node * nevt = reinterpret_cast< event_node * >(n_free_start);
            nevt->p_event = e;
            nevt->next = NULL;

            if( obj == NULL ) {
                obj = reinterpret_cast< object_node * >( n_free_end );
                obj->object_id = object_id;
                obj->head = nevt;

                std::cout << "New Object Node: " << object_id << std::endl;
            } else {
                // object already exists
                //
                //nevt->next = obj->head;
                obj->tail->next = nevt;
            }
            obj->tail = nevt;
            
            m_page.header.free_start = n_free_start;
            m_page.header.free_end = n_free_end;
        }

        return res;
    }

    iterator begin() {
        if( head_object() == end_object() ) return iterator( NULL );
        return iterator( this );
    }
    iterator end() {
        return iterator(NULL);
    }

    void reset() {
        m_page.clear();
    }

    void dump( std::ostream & out ) {
        out << "Object Node Size: " << sizeof(object_node) << "\n";
        out << "Event Node Size: " << sizeof(event_node) << "\n";

        iterator s = begin();
        iterator e = end();
        if( s != e ) {
            do { 
                if( s.HasObjectChanged() ) {
                    out << "\n";
                    out << s.getObjectID();
                    out << "\n";
                }
                out << (s.getEvent()) << ",";
            } while( ++s != e );
            out << "\n";
        } else {
            out << "Empty Event Page\n";
        }
    }

    virtual ~EventPage() { }

protected:

    // returns NULL if object does not exist in object list
    object_node * findObject( const object_t & object_id ) {
        object_node * obj = head_object();
        object_node * e = end_object();
        while ( obj < e ) {
            if( obj->object_id == object_id ) {
                // consider swapping obj with start of the object node list
                // alternatively, splay tree of objects may be advantageous
                // if events/object is small (E[ X < 7 ] => >32 object/page )
                // assuming sequential event generation per object

                break;
            }
            ++obj;
        }

        return (( obj >= e ) ? NULL : obj);
    }

    object_node * head_object( ) {
        return reinterpret_cast< object_node * >(m_page.header.free_end );  // free space ends at the start of object list
    }

    object_node * end_object() {
        return reinterpret_cast< object_node * >( m_page.block + BLOCK_SIZE );
    }

    memory_page m_page;
};
#endif  // EVENT_PAGE_HPP_
