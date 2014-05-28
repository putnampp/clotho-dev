#ifndef EVENT_PAGE_2_HPP_
#define EVENT_PAGE_2_HPP_

#include "memory_page.h"

#include "event_page_walker.2.hpp"

#include <ostream>
#include <iostream>
#include <cstring>

#include <boost/pool/object_pool.hpp>

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

    typedef boost::object_pool< EventPage< EVT, OBJ > > manager_t;

    struct event_node {
        event_t *   p_event;
        event_node * next;
        event_node( event_t * e, event_node * n = NULL ) : p_event(e), next(n) {}
    };

    struct object_node {
        object_t object_id;
        event_node events;
        object_node( object_t id, event_t * e, event_node * n = NULL ) : object_id(id), events(e,n) {}
    };

    struct event_page_header : public memory_page_header {
        EventPage< EVT, OBJ > * next;
        size_t      m_nObjects;
        size_t      m_nEvents;

        event_page_header( EventPage<EVT,OBJ> * n = NULL,  char * s = NULL, char * e = NULL ) : 
            memory_page_header(s, e),
            next(n),
            m_nObjects(0),
            m_nEvents(0)
        {}
    };

    enum { BLOCK_SIZE = PAGE_SIZE - sizeof( event_page_header ) - sizeof( void * ) };   // additional pointer for template overhead

    typedef EventPageWalker<  EventPage< EVT, OBJ > > iterator;
    friend iterator;

    typedef std::pair< iterator, iterator > iterator_range;

    EventPage( EventPage< EVT, OBJ > * n = NULL ) : m_header( n, block, block + BLOCK_SIZE ) {}

    size_t getFreeSpace() const {
        return (m_header.free_end - m_header.free_start);
    }

    bool isFull() const {
        static const size_t buffer_zone_size = sizeof(object_node) + sizeof( event_node );
        return getFreeSpace() <= buffer_zone_size;
    }

    bool isEmpty() const {
        return (m_header.free_start == block);
    }

    bool addEvent( event_t * e, const object_t & object_id ) {
        if( isFull() ) { 
            return false;  // page full
        }

        object_node * obj = findObject( object_id );

        if( obj == NULL ) {
            // object does not exist on current page
            char * n_free_end = m_header.free_end - sizeof(object_node);

            if( m_header.free_start >= n_free_end ) return false;   // insufficient space

            obj = reinterpret_cast< object_node * >(n_free_end);
            obj->object_id = object_id;
            obj->events.p_event = e;
            obj->events.next = NULL;
    
            ++m_header.m_nObjects;

            m_header.free_end = n_free_end;
        } else if( m_header.free_start + sizeof(event_node) < m_header.free_end ) {
            assert( object_id == obj->object_id );

            event_node * tmp = reinterpret_cast< event_node * >(m_header.free_start);
            tmp->p_event = e;
            tmp->next = obj->events.next;
            obj->events.next = tmp;

            m_header.free_start += sizeof(event_node);
        } else {
            // insufficient space
            return false;
        }

        ++m_header.m_nEvents;

        return true;
    }

    iterator begin() {
        if( head_object() == end_object() ) return iterator( NULL );
        return iterator( this );
    }

    iterator end() {
        return iterator(NULL);
    }

    iterator getEventsByObjectIndex( size_t idx ) {
        if( idx < getObjectCount() ) {
            object_node * s = head_object();
            while( idx-- ) { s++; }

            object_node * e = s;
            ++e;
            return iterator(this, s, e);
        }

        return iterator(NULL);
    }

    unsigned int getObjectCount() const {
        return m_header.m_nObjects;
    }

    unsigned int getEventCount() const {
        return m_header.m_nEvents;
    }

    static void * operator new( size_t s ) {
        void * res = m_pool.malloc();
        return res;
    }

    static void operator delete( void * ptr ) {
        ((EventPage< EVT, OBJ> * ) ptr)->reset();
        m_pool.free((EventPage< EVT, OBJ> * ) ptr);
    }

    void reset() {
        m_header.next = NULL;
        m_header.free_start = block;
        m_header.free_end = block + BLOCK_SIZE;
        m_header.m_nObjects = 0;
        m_header.m_nEvents = 0;
    }

    void dump( std::ostream & out ) {
        out << "Object Node Size: " << sizeof(object_node) << "\n";
        out << "Event Node Size: " << sizeof(event_node) << "\n";

        out << "Object Count: " << m_header.m_nObjects << "\n";
        out << "Event Count: " << m_header.m_nEvents << "\n";

        out << "Head Object: " << head_object() << "\n";
        out << "End Object: " << end_object() << "\n";

        size_t dist = end_object() - head_object();
        out << "Calculated Object count: " << ( dist ) << "\n";

        iterator s = begin();
        iterator e = end();
        if( s != e ) {
            do { 
                if( s.HasObjectChanged() ) {
                    out << "\n";
                    out << s.getObjectID();
                    out << "\n";
                }
                out << *(s.getEvent()) << ",";
            } while( ++s != e );
            out << "\n";
        } else {
            out << "Empty Event Page\n";
        }
    }

    EventPage< EVT, OBJ > * getNextPage() {
        return m_header.next;
    }

    void setNextPage( EventPage< EVT, OBJ > * n ) {
        m_header.next = n;
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
        return reinterpret_cast< object_node * >(m_header.free_end );  // free space ends at the start of object list
    }

    object_node * end_object() {
        return reinterpret_cast< object_node * >( block + BLOCK_SIZE );
    }

    //memory_page m_page;
    //size_t      m_nObjects;
    //size_t      m_nEvents;

    event_page_header m_header;
    char          block[ BLOCK_SIZE ];

    static manager_t m_pool;
};

template < class EVT, class OBJ >
typename EventPage< EVT, OBJ >::manager_t EventPage< EVT, OBJ >::m_pool;
#endif  // EVENT_PAGE_2_HPP_
