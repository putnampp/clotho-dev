#ifndef LTSF_QUEUE_HPP_
#define LTSF_QUEUE_HPP_

#include <type_traits>
#include <vector>

#include <iostream>

template< class T, class OBJ >
class ltsf_queue {
public:
    typedef T   vtime_t;
    typedef OBJ object_t;
    typedef object_t * object_ptr;

    static const size_t UNKNOWN_INDEX = -1;

    struct ltsf_node;

    struct object_node {
        size_t  node_idx;
        object_ptr object;
        size_t queue;
        size_t next, prev;

        object_node( size_t idx, object_ptr o = NULL, size_t q = UNKNOWN_INDEX, size_t n = UNKNOWN_INDEX, size_t p = UNKNOWN_INDEX) : 
            node_idx(idx), 
            object(o), 
            queue(q), 
            next(n), 
            prev(p) 
        {}
    };

    struct ltsf_node {
        vtime_t timestamp;

        size_t head;
        size_t next, prev;

        ltsf_node( vtime_t t, size_t h = UNKNOWN_INDEX, size_t n = UNKNOWN_INDEX, size_t p = UNKNOWN_INDEX ) : 
            timestamp(t), 
            head(h), 
            next(n), 
            prev(p) 
        {}
    };

    ltsf_queue( ) :
        m_head(NULL),
        m_unset_object(UNKNOWN_INDEX), 
        m_root( UNKNOWN_INDEX ), 
        m_available( UNKNOWN_INDEX ),
        m_nEnqueued(0)
    { }

    object_ptr getPoolObject( size_t pool_idx ) {
        assert( pool_idx < m_objects.size() );
        return m_objects[ pool_idx ].object;
    }

    bool enqueue( object_ptr o, vtime_t t ) {
        size_t o_idx = getObjectNode();
        object_node & onode = m_objects[o_idx];

        onode.object = o;

        size_t croot = m_root;
        updateObject( onode, t );

        ++m_nEnqueued;

        m_head = m_objects[m_nodes[m_root].head].object;
        // return true when enqueued object is inserted
        // at the head of the queue
        return croot != m_root;
    }

    object_ptr peek() const {
        //if( m_root == UNKNOWN_INDEX ) return NULL;

        // if the root node is defined then it must have a head object
        //
        //const ltsf_node & qnode =  m_nodes[m_root];
        //return m_objects[ qnode.head ].object;
        return m_head;
    }

    object_ptr dequeue( ) {
        object_ptr res = m_head;
        if( m_root != UNKNOWN_INDEX ) {
            ltsf_node & rnode = m_nodes[ m_root ];
            size_t o_idx = rnode.head;
            object_node & n = m_objects[o_idx];

            rnode.head = n.next;
            if( rnode.head == UNKNOWN_INDEX ) {
                remove_ltsf_node( m_root );
            }

            n.queue = UNKNOWN_INDEX;

            // add the node to the unused list
            n.next = m_unset_object;
            m_unset_object = n.node_idx;

            n.prev = UNKNOWN_INDEX;
            --m_nEnqueued;

            if( m_root != UNKNOWN_INDEX ) {
                m_head = m_objects[m_nodes[m_root].head].object;
            } else {
                m_head = NULL;
            }
        }
        return res;
    }

    void updateObject( size_t pool_idx, vtime_t t ) {
        assert( t != (vtime_t) -1 );
        assert( pool_idx != UNKNOWN_INDEX && pool_idx < m_objects.size() );
        object_node & obj = m_objects[pool_idx];
        updateObject( obj, t );
    }

/*    object_ptr getNextObject( vtime_t t ) {
        if( m_root != UNKNOWN_INDEX ) {
            ltsf_node & rnode = m_nodes[m_root];

            if( rnode.timestamp == t ) {
                object_node & n = m_objects[rnode.head];
    
                rnode.head = n.next;
                if( rnode.head == UNKNOWN_INDEX ) {
                    remove_ltsf_node( m_root );
                }

                n.queue = UNKNOWN_INDEX;
                n.next = UNKNOWN_INDEX;
                n.prev = UNKNOWN_INDEX;
                return n.object;
            }
        }
        return NULL;
    }*/

    void dump( std::ostream & o ) {
        size_t tmp_idx = m_root;
        if( tmp_idx != UNKNOWN_INDEX ) {
            do {
                ltsf_node & tmp = m_nodes[tmp_idx];
                o << "LTSF Node: " << tmp.timestamp << "\n";

                size_t obj_idx = tmp.head;
                if( obj_idx != UNKNOWN_INDEX ) {
                    unsigned int n = 0;
                    do {
                        object_node & qobj = m_objects[obj_idx];
                        o << "\t" << ++n << ": " << qobj.node_idx << "\n";
                        obj_idx = qobj.next;
                    } while( obj_idx != UNKNOWN_INDEX );
                } else {
                    o << "\tContains no objects. THIS SHOULD NEVER HAPPEN\n";
                    break;
                }
                tmp_idx = tmp.next;
            } while( tmp_idx != UNKNOWN_INDEX );
        } else {
            o << "LTSF Queue empty\n";
        }
    }

    vtime_t peekNextTimestamp() const {
        return ((m_root != UNKNOWN_INDEX) ? m_nodes[m_root].timestamp : -1);
    }

    size_t getLTSFNodeCount() const {
        return m_nodes.size();
    }

    size_t size() const {
        return m_nEnqueued;
    }

    virtual ~ltsf_queue() {
        m_objects.clear();
        m_nodes.clear();
    }
protected:
    void updateObject( object_node & obj, vtime_t t ) {
        size_t q = obj.queue;
        if( q != UNKNOWN_INDEX ) {
            if( m_nodes[q].timestamp <= t ) return;    // object is already in this queue at an earlier time

            remove_object_from_ltsf_queue( obj );
            assert( obj.queue == UNKNOWN_INDEX );
        }

        size_t p = UNKNOWN_INDEX;
        q = m_root;

        while( q != UNKNOWN_INDEX ) {
            ltsf_node & qnode = m_nodes[q];
            if( qnode.timestamp < t ) {
                p = q;
                q = qnode.next;
            } else if( qnode.timestamp == t ) {
                break;
            } else {
                // ltsf pool does not have node for
                // current timestamp t, but
                // it should be inserted after the
                // current p
                //
                q = UNKNOWN_INDEX;
                break;
            }
        }
        
        if( q == UNKNOWN_INDEX ) {
            // there are currently no queues
            q = getLTSFNode( t );

            insert_ltsf_node_after(q, p);
        }

        insert_object_to_ltsf_node( obj, q );

        assert( m_nodes[q].head == obj.node_idx );
    }

    size_t getObjectNode( ) {
        size_t n = UNKNOWN_INDEX;
        if( m_unset_object != UNKNOWN_INDEX ) {
            n = m_unset_object;
            m_unset_object = m_objects[m_unset_object].next;
        } else {
            n = m_objects.size();
            m_objects.push_back( object_node( n ));
        }
        return n;
    }

    size_t getLTSFNode( vtime_t t ) {
        size_t q = UNKNOWN_INDEX;
        if( m_available != UNKNOWN_INDEX ) {
            // reuse existing node
            q = m_available;
            m_available = m_nodes[m_available].next;
            m_nodes[q].timestamp = t;
        } else {
            q = m_nodes.size();
            m_nodes.push_back( ltsf_node(t) );
        }
        return q;
    }

    void remove_object_from_ltsf_queue( object_node & obj ) {
        // object is current in another queue
        // remove it from its current queue
        //
        if( obj.next != UNKNOWN_INDEX ) {
            m_objects[obj.next].prev = obj.prev;
        }

        if( obj.prev != UNKNOWN_INDEX ) {
            m_objects[obj.prev].next = obj.next;
        } else {
            // head of ltsf queue
            m_nodes[obj.queue].head = obj.next;
        }

        obj.next = UNKNOWN_INDEX;
        obj.prev = UNKNOWN_INDEX;
        obj.queue = UNKNOWN_INDEX;
    }

    void insert_object_to_ltsf_node( object_node & o, size_t q_idx ) {
        ltsf_node & q = m_nodes[q_idx];
        o.next = q.head;

        if( q.head != UNKNOWN_INDEX  ) {
            object_node & q_head = m_objects[q.head];
            o.prev = q_head.prev;
            q_head.prev = o.node_idx;
        } else {
            o.prev = UNKNOWN_INDEX;
        }

        q.head = o.node_idx;
        o.queue = q_idx;
    };

    void insert_ltsf_node_after( size_t nn, size_t p ) {
        assert( nn != UNKNOWN_INDEX );

        ltsf_node & nnode = m_nodes[nn];
        nnode.prev = p;

        if( p == UNKNOWN_INDEX ) {
            // insert nn at root of ltsf queue
            nnode.next = m_root;
            m_root = nn;
        } else {
            ltsf_node & pnode = m_nodes[p];
            nnode.next = pnode.next;
            pnode.next = nn;
        }
        if( nnode.next != UNKNOWN_INDEX ) {
            assert( m_nodes[nnode.next].prev == p );
            m_nodes[nnode.next].prev = nn;
        }
    }

    void remove_ltsf_node( size_t n_idx ) {
        ltsf_node & n = m_nodes[n_idx];
        if( n.next != UNKNOWN_INDEX ) {
            m_nodes[n.next].prev = n.prev;
        }

        if( n.prev != UNKNOWN_INDEX) {
            m_nodes[n.prev].next = n.next;
        } else {
            m_root = n.next;
        }

        n.prev = UNKNOWN_INDEX;
        n.next = m_available;
        m_available = n_idx;
    }

    std::vector< object_node > m_objects;
    std::vector< ltsf_node > m_nodes;

    object_ptr m_head;
    size_t m_unset_object;      // object_node index
    size_t m_root, m_available; // ltsf_node index
    size_t m_nEnqueued;
};

#endif  // LTSF_QUEUE_HPP_
