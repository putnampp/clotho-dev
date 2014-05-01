#ifndef LTSF_POOL_HPP_
#define LTSF_POOL_HPP_

#include <type_traits>
#include <deque>

#include "poolable.h"

template < class T, class OBJ, class E = void >
class ltsf_pool;

template < class T, class OBJ >
class ltsf_pool< T, OBJ, 
    typename std::enable_if< std::is_base_of< poolable, OBJ >::value >::type > {
public:
    typedef T   vtime_t;
    typedef OBJ object_t;
    typedef object_t * obj_pointer;

    static const size_t UNKNOWN_INDEX = -1;

    struct ltsf_node;

    struct object_node {
        size_t  node_idx;
        obj_pointer object;
        ltsf_node   * queue;
        object_node * next, * prev;

        object_node( size_t idx ) : node_idx(idx), object( NULL ), queue(NULL), next(NULL ), prev(NULL) {}
        object_node( size_t idx, obj_pointer o, ltsf_node * q, object_node * n = NULL, object_node * p = NULL) : node_idx(idx), object(o), queue(q), next(n), prev(p) {}
    };

    struct ltsf_node {
        vtime_t timestamp;

        object_node * head;
        ltsf_node * next, * prev;

        ltsf_node( ) : timestamp(0), head(NULL), next(NULL), prev(NULL) {}
        ltsf_node( vtime_t t, object_node * h, ltsf_node * n = NULL, ltsf_node * p = NULL ) : timestamp(t), head(h), next(n), prev(p) {}

        void init( vtime_t t, object_node * h, ltsf_node * n = NULL, ltsf_node * p = NULL ) {
            timestamp = t;
            head = h;
            next = n;
            prev = p;
        }
    };

    ltsf_pool() : m_unset_object(NULL), m_root( NULL ), m_available(NULL) {}

    size_t getNextPoolIndex() {
        return UNKNOWN_INDEX;
    }

    obj_pointer getPoolObject( size_t pool_idx ) {
        assert( pool_idx < m_objects.size() );

        return m_objects[ pool_idx ].object;
    }

    void setPoolObject( obj_pointer p ) {
        size_t p_idx = p->getPoolIndex();
        assert( p_idx == UNKNOWN_INDEX );

        object_node * t = getObjectNode( );

        t->object = p;
        p->setPoolIndex( t->node_idx );
    }

    bool unsetPoolObject( obj_pointer p ) {
        size_t p_idx = p->getPoolIndex();
        if( p_idx == UNKNOWN_INDEX ) return false;

        object_node * obj = &m_objects[ p_idx ];

        obj->object = NULL;

        if( obj->queue != NULL ) {
            remove_object_from_ltsf_queue( obj );
        } else {
            obj->prev = NULL;
        }

        obj->next = m_unset_object;
        m_unset_object = obj;

        p->setPoolIndex( UNKNOWN_INDEX );
        return true;
    }

    void updateObject( obj_pointer o, vtime_t t ) {
        assert( o != NULL );
        updateObject( o->getPoolIndex(), t );
    }

    void updateObject( size_t pool_idx, vtime_t t ) {
        assert( t != (vtime_t) -1 );
        assert( pool_idx != UNKNOWN_INDEX && pool_idx < m_objects.size() );
        object_node * obj = &m_objects[pool_idx];
        updateObject( obj, t );
    }

    obj_pointer getNextObject( vtime_t t ) {
        if( m_root != NULL && m_root->timestamp == t ) {
            object_node * n = m_root->head;

            m_root->head = n->next;
            if( m_root->head == NULL ) {
//                std::cout << "Got last object from " << m_root->timestamp << " @ " << t << std::endl;
                remove_ltsf_node( m_root );
            }

            n->queue = NULL;
            n->next = NULL;
            n->prev = NULL;
            return n->object;
        }
        return NULL;
    }

    void dump( std::ostream & o ) {
        ltsf_node * tmp = m_root;
        if( tmp != NULL ) {
            do {
                o << "LTSF Node: " << tmp->timestamp << "\n";
                object_node * qobj = tmp->head;

                if( qobj != NULL ) {
                    unsigned int n = 0;
                    do {
                        o << "\t" << ++n << ": " << qobj->node_idx << "\n";
                        qobj = qobj->next;
                    } while( qobj != NULL );
                } else {
                    o << "\tContains no objects. THIS SHOULD NEVER HAPPEN\n";
                    break;
                }
                tmp = tmp->next;
            } while( tmp != NULL );
        } else {
            o << "LTSF Queue empty\n";
        }
    }

    vtime_t peekNextTimestamp() const {
        return ((m_root != NULL) ? m_root->timestamp : -1);
    }

    size_t getLTSFNodeCount() const {
        return m_nodes.size();
    }

    virtual ~ltsf_pool() {
        m_objects.clear();
        m_nodes.clear();
    }
protected:
    void updateObject( object_node * obj, vtime_t t ) {
        ltsf_node * q = obj->queue;
        if( q != NULL ) {
            if( q->timestamp <= t ) return;    // object is already in this queue at an earlier time

            remove_object_from_ltsf_queue( obj );
            assert( obj->queue == NULL );
        }

        ltsf_node * p = NULL;
        q = m_root;

        while( q != NULL ) {
            if( q->timestamp < t ) {
                p = q;
                q = q->next;
            } else if( q->timestamp == t ) {
                break;
            } else {
                // ltsf pool does not have node for
                // current timestamp t, but
                // it should be inserted after the
                // current p
                //
                q = NULL;
                break;
            }
        }
        
        if( q == NULL ) {
            // there are currently no queues
//            std::cout << "Adding new LTSF Node to queue " << t << std::endl;
            q = getLTSFNode();

            q->timestamp = t;
            insert_ltsf_node_after(q, p);
        }

//        std::cout << "Inserting " << obj->node_idx << " into " << q->timestamp << std::endl;
        insert_object_to_ltsf_node( obj, q );

        assert( q->head == obj );
    }

    object_node * getObjectNode( ) {
        object_node * n = NULL;
        if( m_unset_object != NULL ) {
            n = m_unset_object;
            m_unset_object = m_unset_object->next;
        } else {
            m_objects.push_back( object_node( m_objects.size() ));
            n = &m_objects.back();
        }
        return n;
    }

    ltsf_node * getLTSFNode() {
        ltsf_node * q = NULL;
        if( m_available != NULL ) {
            // reuse existing node
            q = m_available;
            m_available = m_available->next;
        } else {
            m_nodes.push_back( ltsf_node() );
            q = &m_nodes.back();
        }
        return q;
    }

    void remove_object_from_ltsf_queue( object_node * obj ) {
        // object is current in another queue
        // remove it from its current queue
        //
        if( obj->next != NULL ) {
            obj->next->prev = obj->prev;
        }

        if( obj->prev != NULL ) {
            obj->prev->next = obj->next;
        } else {
            // head of ltsf queue
            obj->queue->head = obj->next;

            //if( obj->queue->head == NULL )
            //    remove_ltsf_node( obj->queue );
        }

        obj->next = NULL;
        obj->prev = NULL;
        obj->queue = NULL;
    }

    void insert_object_to_ltsf_node( object_node * o, ltsf_node * q ) {
        o->next = q->head;

        if( q->head != NULL  ) {
            o->prev = q->head->prev;
            q->head->prev = o;
        } else {
            o->prev = NULL;
        }

        q->head = o;
        o->queue = q;
    };

    void insert_ltsf_node_after( ltsf_node * nn, ltsf_node * p ) {
        nn->prev = p;

        if( p == NULL ) {
            // insert nn at root of ltsf queue
//            std::cout << "Inserting LTSF Node at root" << std::endl;
            nn->next = m_root;
            m_root = nn;
        } else {
//            std::cout << "Inserting LTSF Node after " << p->timestamp << std::endl;
            nn->next = p->next;
            p->next = nn;
        }
        if( nn->next != NULL ) {
            assert( nn->next->prev == p );
            nn->next->prev = nn;
        }
    }

    void remove_ltsf_node( ltsf_node * n ) {
//        std::cout << "Removing LTSF node " << n->timestamp << std::endl;
        if( n->next ) {
            n->next->prev = n->prev;
        }

        if( n->prev ) {
            n->prev->next = n->next;
        } else {
            m_root = n->next;
        }

        n->prev = NULL;
        n->next = m_available;
        m_available = n;
    }

    std::deque< object_node > m_objects;
    std::deque< ltsf_node > m_nodes;

    object_node * m_unset_object;
    ltsf_node * m_root;
    ltsf_node * m_available;
};

#endif  // LTSF_POOL_HPP_
