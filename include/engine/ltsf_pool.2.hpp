#ifndef LTSF_POOL2_HPP_
#define LTSF_POOL2_HPP_

#include <vector>

template < class T, class OBJ, class E = void >
class ltsf_pool;

template < class T >
class ltsf_pool< T, unsigned int, void> {
public:
    typedef T   vtime_t;
    typedef unsigned int object_t;

    static const size_t UNKNOWN_INDEX = -1;

    struct _node {
        union d {
            struct o {
                object_t  object;
                size_t      queue;
                o() : object((object_t)UNKNOWN_INDEX), queue(UNKNOWN_INDEX) {}
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

    ltsf_pool() :
        m_unset_node(UNKNOWN_INDEX),
        m_root( UNKNOWN_INDEX )/*,
        m_available( UNKNOWN_INDEX ) */
    {}

    object_t getPoolObject( size_t pool_idx ) {
        assert( pool_idx < m_nodes.size() );
        return m_nodes[ pool_idx ]._data._object.object;
    }

    size_t setPoolObject( object_t p ) {
        size_t idx = getObjectNode(p);
        return idx;
    }

    bool unsetPoolObject( size_t p_idx ) {
        _node & obj = m_nodes[ p_idx ];

        obj._data._object.object = (object_t) UNKNOWN_INDEX;

        if( obj._data._object.queue != UNKNOWN_INDEX ) {
            remove_object_from_ltsf_queue( obj );
        } else {
            obj.prev = UNKNOWN_INDEX;
        }

        obj.next = m_unset_node;
        m_unset_node = p_idx;

        return true;
    }

    object_t getNextObject( vtime_t t ) {
        if( m_root != UNKNOWN_INDEX ) {
            _node & rnode = m_nodes[m_root];

            if( rnode._data._ltsf.timestamp == t ) {
                _node & n = m_nodes[rnode._data._ltsf.head];

                rnode._data._ltsf.head = n.next;
                if( n.next == UNKNOWN_INDEX ) {
                    remove_ltsf_node( m_root );
                }

                n._data._object.queue = UNKNOWN_INDEX;
                n.next = UNKNOWN_INDEX;
                n.prev = UNKNOWN_INDEX;
                return n._data._object.object;
            }
        }
        return (object_t)UNKNOWN_INDEX;
    }

    vtime_t peekTime() const {
        return m_active_timestamp;
    }

    object_t dequeue( const vtime_t & t ) {
        if( m_active_timestamp != t ) {
            _node & rnode = m_nodes[ m_root ];

            size_t o_idx = rnode._data._ltsf.head;
            _node & n = m_nodes[ o_idx ];

            rnode._data._ltsf.head = n.next;
            if( n.next == UNKNOWN_INDEX ) {
                remove_ltsf_node( m_root );
                if( m_root != UNKNOWN_INDEX ) {
                    m_active_timestamp = m_nodes[m_root]._data._ltsf.timestamp;
                } else {
                    m_active_timestamp = (vtime_t) -1;
                }
            }

            n._data._object.queue = UNKNOWN_INDEX;
            n.next = UNKNOWN_INDEX;
            n.prev = UNKNOWN_INDEX;

            return n._data._object.object;
        }
        return (object_t)UNKNOWN_INDEX;
    }

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
        return ((m_root != UNKNOWN_INDEX) ? m_nodes[m_root]._data._ltsf.timestamp : -1);
    }

    void updateObject( size_t o_idx, vtime_t t ) {
        _node & obj = m_nodes[o_idx];

        size_t q = obj._data._object.queue;
        if( q != UNKNOWN_INDEX ) {
            if( m_nodes[q]._data._ltsf.timestamp <= t ) return;    // object is already in this queue at an earlier time

            remove_object_from_ltsf_queue( obj );
            assert( obj._data._object.queue == UNKNOWN_INDEX );
        }

        size_t p = UNKNOWN_INDEX;
        size_t croot = m_root;
        q = croot;

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

        insert_object_to_ltsf_node( o_idx, q );

        assert( m_nodes[q]._data._ltsf.head == o_idx );
        if( croot != m_root ) {
            // root changed as a result of this update
            m_active_timestamp = m_nodes[ m_root ]._data._ltsf.timestamp;
        }
    }

    virtual ~ltsf_pool() {
        m_nodes.clear();
    }
protected:
    size_t getNode() {
        size_t n = UNKNOWN_INDEX;
        if( m_unset_node != UNKNOWN_INDEX ) {
            n = m_unset_node;
            m_unset_node = m_nodes[m_unset_node].next;
        } else {
            n = m_nodes.size();
            m_nodes.push_back( _node( ));
        }
        return n;
    }

    size_t getObjectNode( object_t o ) {
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

        obj._data._object.queue = UNKNOWN_INDEX;
        obj.next = UNKNOWN_INDEX;
        obj.prev = UNKNOWN_INDEX;
    }

    void insert_object_to_ltsf_node( size_t o_idx, size_t q_idx ) {
        _node & o = m_nodes[o_idx];
        _node & q = m_nodes[q_idx];
        o.next = q._data._ltsf.head;

        if( o.next != UNKNOWN_INDEX  ) {
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
        n.next = m_unset_node;
        m_unset_node = n_idx;
    }

    std::vector< _node > m_nodes;

    vtime_t m_active_timestamp;
    size_t m_unset_node;
    size_t m_root;
};

#endif  // LTSF_POOL2_HPP_
