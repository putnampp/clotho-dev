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
        event_node( event_t * e = NULL, event_node * n = NULL ) : p_event(e), next(n) {}
        event_node( const event_node & en ) : p_event(en.p_event), next( en.next ) {}

        event_node & operator=( const event_node & en ) {
            p_event = en.p_event;
            next = en.next;
            return *this;
        }
    };

    struct object_node {
        object_t object_id;
        event_node * events;
        object_node(object_t id = object_t(), event_node * n = NULL ) : object_id(id), events(n) {}
        object_node( const object_node & onode ) : object_id( onode.object_id ), events( onode.events ) {}

        object_node & operator=( const object_node & rhs ) {
            object_id = rhs.object_id;
            events = rhs.events;
            return *this;
        }
    };


    enum rb_state { RED, BLACK };

    struct rb_node {
        rb_node * parent, * left, * right;
        object_node onode;
        rb_state state;

        rb_node( rb_node * p = NULL, rb_node * l = NULL, rb_node * r = NULL, object_node o = object_node() ) :
            parent(p),
            left(l),
            right(r),
            onode(o),
            state(RED) {
        }
    };

    struct event_page_header {
        EventPage< EVT, OBJ > * next;
        rb_node     *m_root;
        size_t      m_nObjects, m_nEvents;

        event_page_header( EventPage<EVT,OBJ> * n = NULL, rb_node * r = NULL ) :
            next(n),
            m_root(r),
            m_nObjects(0),
            m_nEvents(0) {
        }
    };

    enum {  HEADER_SIZE = sizeof( event_page_header ) + sizeof( void * ), // additional pointer for virtual table
            BLOCK_SIZE = PAGE_SIZE - HEADER_SIZE,
            EXP_EVENTS_PER_NODE = 3,
            OBJECT_COUNT = (BLOCK_SIZE) / (sizeof(rb_node) + (EXP_EVENTS_PER_NODE - 1) * sizeof(event_node) ),
            EVENT_COUNT = OBJECT_COUNT * (EXP_EVENTS_PER_NODE - 1)
         };

    typedef EventPageWalker<  EventPage< EVT, OBJ > > iterator;
    friend iterator;

    typedef std::pair< iterator, iterator > iterator_range;

    EventPage( EventPage< EVT, OBJ > * n = NULL ) : m_header( n ) {}

    bool hasFreeObjects() {
        return m_header.m_nObjects < OBJECT_COUNT;
    }

    bool isFull() const {
        return (m_header.m_nEvents >= EVENT_COUNT);
    }

    bool isEmpty() const {
        return (m_header.m_nEvents == 0);
    }

    bool addEvent( event_t * e, const object_t & object_id ) {
        if( m_header.m_nEvents >= EVENT_COUNT ) return false;

        event_node * enode = &event_space[ m_header.m_nEvents++ ];
        enode->p_event = e;

        object_node onode( object_id, enode );

        if( !insert( onode ) ) {
            m_header.m_nEvents--;
            return false;
        }
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
            rb_node * s = &node_space[ idx++ ];
            rb_node * e = &node_space[ idx ];
            return iterator(this, s, e);
        }

        return iterator(NULL);
    }

    unsigned int getObjectCount() const {
        return m_header.m_nObjects;
    }

    unsigned int getEventCount() const {
        return m_header.m_nEvents + m_header.m_nObjects;
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
//        m_header.free_start = block;
//        m_header.free_end = block + BLOCK_SIZE;
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

    bool insert( const object_node & onode ) {
        rb_node * y = NULL;
        rb_node * x = m_header.m_root;

        bool add_right = false;
        while( x != NULL ) {
            y = x;
            if( x->onode.object_id == onode.object_id ) {
//                std::cout << "Extending list of: " << onode.object_id << std::endl;
                onode.events->next = x->onode.events;
                x->onode.events = onode.events;
                return true;
            } else if( x->onode.object_id < onode.object_id ) {
//                std::cout << "Traversing From " << x->onode.object_id << " R ";
                add_right = true;
                x = x->right;
//                if( x != NULL ) {
//                    std::cout << x->onode.object_id << std::endl;
//                } else {
//                    std::cout << "NULL" << std::endl;
//                }
            } else {
//                std::cout << "Traversing From " << x->onode.object_id << " L ";
                add_right = false;
                x = x->left;
//                if( x != NULL )
//                    std::cout <<  x->onode.object_id << std::endl;
//                else
//                    std::cout << "NULL" << std::endl;
            }
        }

        if( m_header.m_nObjects >= OBJECT_COUNT ) return false;

        rb_node * z = & node_space[ m_header.m_nObjects++ ];

        z->parent = y;
        z->left = NULL;
        z->right = NULL;
        z->state = RED;
        z->onode = onode;

//        std::cout << "Adding new object: ";
        if( y == NULL ) {
//            std::cout << "{" << z->onode.object_id << ",NULL,NULL}" << std::endl;
            m_header.m_root = z;
        } else if( !add_right ) {
//            std::cout << "{" <<y->onode.object_id << ", ";
//            if( y->right == NULL ) {
//                std::cout << "NULL}" << std::endl;
//            } else {
//                std::cout << y->right->onode.object_id << "}" << std::endl;
 //           }
            y->left = z;
        } else {
//            std::cout << "{" << y->onode.object_id << ", ";
//            if( y->left == NULL ) {
//                std::cout << "NULL"; 
//            } else {
//                std::cout << y->left->onode.object_id;
//            }
//            std::cout << ", " << z->onode.object_id << "}" << std::endl;
            y->right = z;
        }

        balance( z );
        return true;
    }

    inline rb_node * grandparent( rb_node * z ) {
        return (((z == NULL) || (z->parent == NULL )) ? NULL : z->parent->parent);
    }

    inline rb_node * uncle( rb_node * z ) {
        rb_node * g = grandparent(z);
        return ((z == NULL) ? NULL : ((g->left == z->parent) ? g->right :g->left));
    }

    inline rb_node * other_sibling( rb_node * parent, rb_node * sib0 ) {
        return ((parent == NULL) ? NULL : ((parent->left == sib0) ? parent->right : parent->left));
    }

    inline void rotate_right( rb_node * z ) {
        rb_node * t = z->left;
        z->left = t->right;

        if( t->right != NULL ) t->right->parent=z;

        t->parent = z->parent;

        if( z->parent == NULL ) {
            m_header.m_root = t;
        } else if( z == z->parent->left) {
            z->parent->left = t;
        } else {
            z->parent->right = t;
        }

        t->right = z;
        z->parent = t;
    }

    inline void rotate_left( rb_node * z ) {
        rb_node * t = z->right;
        z->right = t->left;

        if( t->left != NULL ) t->left->parent=z;

        t->parent = z->parent;
        if( z->parent == NULL ) {
            m_header.m_root = t;
        } else if( z == z->parent->left ) {
            z->parent->left = t;
        } else {
            z->parent->right = t;
        }
        t->left = z;
        z->parent = t;
    }


    void balance( rb_node * x ) {
        if( x->parent == NULL ) {
            x->state = BLACK;
            return;
        }

        while( (x != m_header.m_root ) && (x->parent->state == RED )) {
            if( x->parent == x->parent->parent->left ) {
                rb_node * y = x->parent->parent->right;
                if( y != NULL && y->state == RED ) {
                    x->parent->state = BLACK;
                    y->state = BLACK;
                    x->parent->parent->state = RED;

                    x = x->parent->parent;
                } else {
                    if ( x == x->parent->right ) {
                        x = x->parent;
                        rotate_left( x );
                    }

                    x->parent->state = BLACK;
                    x->parent->parent->state = RED;

                    rotate_right( x->parent->parent );
                }
            } else {
                rb_node * y = x->parent->parent->left;
                if( y != NULL &&  y->state == RED ) {
                    x->parent->state = BLACK;
                    y->state = BLACK;

                    x->parent->parent->state = RED;
                    x = x->parent->parent;
                } else {
                    if( x == x->parent->left ) {
                        x = x->parent;
                        rotate_right(x);
                    }

                    x->parent->state = BLACK;
                    x->parent->parent->state = RED;
                    rotate_left( x->parent->parent );
                }
            }
        }
        m_header.m_root->state=BLACK;
    }

    /*    void balance( rb_node * z ) {
            if( z->parent == NULL ) {
                std::cout << "setting root to black" << std::endl;
                z->state = BLACK;   // root node is always black
            } else if( z->parent->state != BLACK ) {
                rb_node * g = grandparent(z);
                rb_node * u = other_sibling(g, z->parent);
                if( u != NULL && u->state == RED ) {
                    z->parent->state = BLACK;
                    u->state = BLACK;
                    g->state = RED;

                    balance(g);
                } else {
                    std::cout << "no uncle or uncle properly colored" << std::endl;
                    if( z == z->parent->right && z->parent == g->left ) {
                        rotate_left( z->parent );
                        z = z->left;
                    } else if( z == z->parent->left && z->parent == g->right ) {
                        rotate_right( z->parent );
                        z = z->right;
                    }

                    g = grandparent(z);
                    z->parent->state = BLACK;
                    g->state = RED;

                    if( z == z->parent->left ) {
                        std::cout << "Rotating grandparent right" << std::endl;
                        rotate_right(g);
                    } else {
                        std::cout << "Rotating grandparent left" << std::endl;
                        rotate_left( g );
                    }
                }
            }
        }*/


    /*    object_node * head_object( ) {
            return reinterpret_cast< object_node * >(m_header.free_end );  // free space ends at the start of object list
        }

        object_node * end_object() {
            return reinterpret_cast< object_node * >( block + BLOCK_SIZE );
        }*/

    rb_node * head_object() {
        return node_space;
    }

    rb_node * end_object() {
        return node_space + m_header.m_nObjects;
    }

    event_page_header m_header;
    rb_node         node_space[ OBJECT_COUNT ];
    event_node      event_space[ EVENT_COUNT ];

    static manager_t m_pool;
};

template < class EVT, class OBJ >
typename EventPage< EVT, OBJ >::manager_t EventPage< EVT, OBJ >::m_pool;
#endif  // EVENT_PAGE_2_HPP_
