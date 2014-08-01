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

    struct _node {
        union d {
            struct o {
                object_ptr  object;
                size_t      queue;
                o() : object(NULL), queue(UNKNOWN_INDEX) {}
            } _object;
            struct {
                vtime_t     timestamp;
                size_t      head;
            } _ltsf;

            d( ) : _object() {}

        } _data;
        size_t  next, prev;
        _node() :
            _data(),
            next( UNKNOWN_INDEX ),
            prev( UNKNOWN_INDEX ) {
        }
    };

    ltsf_queue( ) :
        m_head(NULL),
//        m_available_node( UNKNOWN_INDEX ),
        //m_unset_object(UNKNOWN_INDEX),
        m_root( UNKNOWN_INDEX ),
        //m_available( UNKNOWN_INDEX ),
        m_nEnqueued(0) {
    }

    object_ptr getPoolObject( size_t pool_idx ) {
        assert( pool_idx < m_nodes.size() );
        return m_nodes[ pool_idx ]._data._object.object;
    }

    bool enqueue( object_ptr o, vtime_t t ) {
        size_t croot = m_root;

        size_t q_idx = findLTSFNode( t );
        size_t o_idx = getObjectNode( o );

        //updateObject( o_idx, t );

        insert_object_to_ltsf_node( o_idx, q_idx );

        assert( m_nodes[q_idx]._data._ltsf.head == o_idx );

        ++m_nEnqueued;

        m_head = m_nodes[ m_nodes[ m_root ]._data._ltsf.head ]._data._object.object;
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
        if( res != NULL ) {
            _node & rnode = m_nodes[ m_root ];

            size_t o_idx = rnode._data._ltsf.head;
            _node & n = m_nodes[o_idx];

            std::cout   << rnode._data._ltsf.timestamp
                        << "," << m_root
                        << "," << rnode._data._ltsf.head
                        << "," << (long)rnode.next
                        << "," << (long)n.next
                        << "\n";

            rnode._data._ltsf.head = n.next;
            if( n.next == UNKNOWN_INDEX ) {
                remove_ltsf_node( m_root );
            }

            n._data._object.queue = UNKNOWN_INDEX;

            // add the node to the unused list
            n.next = m_available_node;
            m_available_node = o_idx;

            n.prev = UNKNOWN_INDEX;
            --m_nEnqueued;

            if( m_root != UNKNOWN_INDEX ) {
                m_head = m_nodes[m_nodes[m_root]._data._ltsf.head]._data._object.object;
            } else {
                m_head = NULL;
            }
        }
        return res;
    }

    /*    object_ptr getNextObject( vtime_t t ) {
            if( m_root != UNKNOWN_INDEX ) {
                ltsf_node & rnode = m_nodes[m_root];

                if( rnode.timestamp == t ) {
                    _object & n = m_objects[rnode.head];

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
                _node & tmp = m_nodes[tmp_idx];
                o << "LTSF Node: " << tmp._data._ltsf.timestamp << "\n";

                size_t obj_idx = tmp._data._ltsf.head;
                if( obj_idx != UNKNOWN_INDEX ) {
                    unsigned int n = 0;
                    do {
                        _node & qobj = m_nodes[obj_idx];
                        o << "\t" << ++n << ": " << obj_idx << "\n";
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
//        return ((m_root != UNKNOWN_INDEX) ? m_nodes[m_root].timestamp : -1);
        return ((m_root != UNKNOWN_INDEX ) ? m_nodes[m_root]._data._ltsf.timestamp : -1 );
    }

//    size_t getLTSFNodeCount() const {
//        return m_nodes.size();
//    }

    size_t size() const {
        return m_nEnqueued;
    }

    virtual ~ltsf_queue() {
//        m_objects.clear();
//        m_nodes.clear();
    }

    void updateObject( size_t o_idx, vtime_t t ) {
        _node & obj = m_nodes[o_idx];

        size_t q = obj._data._object.queue;
        if( q != UNKNOWN_INDEX ) {
            if( m_nodes[q]._data._ltsf.timestamp <= t ) return;    // object is already in this queue at an earlier time

            remove_object_from_ltsf_queue( obj );
            assert( obj._data._object.queue == UNKNOWN_INDEX );
        }

        /*        size_t p = UNKNOWN_INDEX;
                q = m_root;

                while( q != UNKNOWN_INDEX ) {
                    _node & qnode = m_nodes[q];
                    vtime_t ts = qnode._data._ltsf.timestamp;

                    if( ts < t ) {
                        p = q;
                        q = qnode.next;
                    } else if( ts == t ) {
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
                }*/

        insert_object_to_ltsf_node( o_idx, q );

        assert( m_nodes[q]._data._ltsf.head == o_idx );
    }

protected:

    size_t findLTSFNode( vtime_t t ) {
        size_t p = UNKNOWN_INDEX;
        size_t q = m_root;

        while( q != UNKNOWN_INDEX ) {
            _node & qnode = m_nodes[q];
            vtime_t ts = qnode._data._ltsf.timestamp;

            if( ts < t ) {
                p = q;
                q = qnode.next;
            } else if( ts == t ) {
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

        return q;
    }

    size_t getNode( ) {
        size_t n = UNKNOWN_INDEX;
        if( m_available_node != UNKNOWN_INDEX ) {
            n = m_available_node;
            m_available_node = m_nodes[ m_available_node ].next;
        } else {
            n = m_nodes.size();
            m_nodes.push_back( _node( ));
        }
        return n;
    }

    size_t getObjectNode( object_ptr o ) {
        size_t n = getNode();
        _node & tmp = m_nodes[n];
        tmp._data._object.object = o;
        tmp._data._object.queue = UNKNOWN_INDEX;
        return n;
    }

    size_t getLTSFNode( vtime_t t ) {
        size_t q = getNode();
        _node & tmp = m_nodes[q];
        tmp._data._ltsf.timestamp = t;
        tmp._data._ltsf.head = UNKNOWN_INDEX;
        return q;
    }

    void remove_object_from_ltsf_queue( _node & obj ) {
        // object is current in another queue
        // remove it from its current queue
        //
        if( obj.next != UNKNOWN_INDEX ) {
            m_nodes[obj.next].prev = obj.prev;
        }

        if( obj.prev != UNKNOWN_INDEX ) {
            m_nodes[obj.prev].next = obj.next;
        } else {
            // head of ltsf queue
            m_nodes[obj._data._object.queue]._data._ltsf.head = obj.next;
        }

        obj.next = UNKNOWN_INDEX;
        obj.prev = UNKNOWN_INDEX;
    }

    void insert_object_to_ltsf_node( size_t o_idx, size_t q_idx ) {
        _node & o = m_nodes[ o_idx ];
        _node & q = m_nodes[q_idx];
        o.next = q._data._ltsf.head;

        //if( q.head != UNKNOWN_INDEX  ) {
        if( o.next != UNKNOWN_INDEX ) {
            _node & q_head = m_nodes[o.next];
            o.prev = q_head.prev;
            q_head.prev = o_idx;
        } else {
            o.prev = UNKNOWN_INDEX;
        }

        q._data._ltsf.head = o_idx;
        o._data._object.queue = q_idx;
    };

    void insert_ltsf_node_after( size_t nn, size_t p ) {
        assert( nn != UNKNOWN_INDEX );

        _node & nnode = m_nodes[nn];
        nnode.prev = p;

        if( p == UNKNOWN_INDEX ) {
            // insert nn at root of ltsf queue
            nnode.next = m_root;
            m_root = nn;
        } else {
            _node & pnode = m_nodes[p];
            nnode.next = pnode.next;
            pnode.next = nn;
        }
        if( nnode.next != UNKNOWN_INDEX ) {
            assert( m_nodes[nnode.next].prev == p );
            m_nodes[nnode.next].prev = nn;
        }
    }

    void remove_ltsf_node( size_t n_idx ) {
        _node & n = m_nodes[n_idx];
        if( n.next != UNKNOWN_INDEX ) {
            m_nodes[n.next].prev = n.prev;
        }

        if( n.prev != UNKNOWN_INDEX) {
            m_nodes[n.prev].next = n.next;
        } else {
            m_root = n.next;
        }

        n.prev = UNKNOWN_INDEX;
        n.next = m_available_node;
        m_available_node = n_idx;
    }

    object_ptr m_head;
    size_t m_root;
    size_t m_nEnqueued;

    static std::vector< _node > m_nodes;
    static size_t m_available_node;
};

template< class T, class OBJ >
std::vector< typename ltsf_queue<T,OBJ>::_node > ltsf_queue<T,OBJ>::m_nodes;

template< class T, class OBJ >
size_t ltsf_queue<T,OBJ>::m_available_node = ltsf_queue<T,OBJ>::UNKNOWN_INDEX;

#endif  // LTSF_QUEUE_HPP_
