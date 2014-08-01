#ifndef LTSF_QUEUE_HPP_
#define LTSF_QUEUE_HPP_

#include <type_traits>
#include <deque>

template < class T, class OBJ >
class ltsf_queue {
public:
    typedef T   vtime_t;
    typedef OBJ object_t;
    typedef object_t * obj_pointer;

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

    ltsf_queue() : m_unset_object(NULL), m_root( NULL ), m_available(NULL), m_nEnqueued(0) {}

    bool enqueue( obj_pointer o, vtime_t t ) {

        assert( o != NULL );

        object_node * obj = getObjectNode();
        obj->object = o;

        ltsf_node * p = NULL, * q = m_root;

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

        bool res = false;
        if( q == NULL ) {
            // there is no queue for this timestamp
            q = getLTSFNode();

            q->timestamp = t;
            insert_ltsf_node_after(q, p);
            res = (p == NULL);  // this timestamp is new least timestamp
        }

        insert_object_to_ltsf_node( obj, q );

        assert( q->head == obj );
        ++m_nEnqueued;
        return res;
    }

    obj_pointer peek() const {
        if( m_root == NULL || m_root->head == NULL ) return NULL;
        return m_root->head->object;
    }

    obj_pointer dequeue( ) {
        if( m_root != NULL ) {
            object_node * n = m_root->head;

            m_root->head = n->next;
            if( m_root->head == NULL ) {
                remove_ltsf_node( m_root );
            }

            n->queue = NULL;

            // add the node to the unused list
            n->next = m_unset_object;
            m_unset_object = n;

            n->prev = NULL;
            --m_nEnqueued;
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

    bool empty() const {
        return (m_root == NULL);
    }

    size_t size() const {
        return m_nEnqueued;
    }

    size_t getLTSFNodeCount() const {
        return m_nodes.size();
    }

    virtual ~ltsf_queue() {
        m_objects.clear();
        m_nodes.clear();
    }
protected:
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
            nn->next = m_root;
            m_root = nn;
        } else {
            nn->next = p->next;
            p->next = nn;
        }
        if( nn->next != NULL ) {
            assert( nn->next->prev == p );
            nn->next->prev = nn;
        }
    }

    void remove_ltsf_node( ltsf_node * n ) {
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

    size_t m_nEnqueued;
};

#endif  // LTSF_QUEUE_HPP_
