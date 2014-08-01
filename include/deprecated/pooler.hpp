#ifndef POOLER_HPP_
#define POOLER_HPP_

#include <iostream>

template < class E >
class Pooler {
public:
    typedef E event_t;

    static const size_t UNKNOWN_INDEX = -1;

    struct _node {
        event_t * e;
        size_t next;
        _node( event_t * _e = NULL, size_t n = UNKNOWN_INDEX ) : e(_e), next(n) {}
    };

    Pooler( ) :
        m_unused( UNKNOWN_INDEX ),
        m_free_node( UNKNOWN_INDEX ) {
    }

    event_t * getOrCreate() {
        event_t * res = NULL;
        if( m_unused == UNKNOWN_INDEX ) {
            res = new event_t();
        } else {
            res = remove_unused();
        }

        return res;
    }

    void release( event_t * e ) {
        if( e == NULL ) return;
        remove_free( e );
    }

    void reset() {
        while( !m_nodes.empty() ) {
            event_t * e = m_nodes.back().e;
            m_nodes.pop_back();
            if( e ) delete e;
        }
    }

    virtual ~Pooler() {
        reset();
    }
protected:
    event_t * remove_unused( ) {
        size_t tmp = m_unused;

        _node & n = m_nodes[ tmp ];

        m_unused = n.next;
        n.next = m_free_node;
        m_free_node = tmp;

        return n.e;
    }

    void remove_free( event_t * e ) {
        if( m_free_node == UNKNOWN_INDEX ) {
            // there are no free nodes;
            // add a new one
            m_nodes.push_back( _node( e, m_unused ) );
            m_unused = m_nodes.size() - 1;
        } else {
            size_t tmp = m_free_node;
            _node & n = m_nodes[ tmp ];

            n.e = e;

            m_free_node = n.next;
            n.next = m_unused;
            m_unused = tmp;
        }
    }

    std::vector< _node > m_nodes;
    size_t m_unused;
    size_t m_free_node;
};

#endif  // POOLER_HPP_
