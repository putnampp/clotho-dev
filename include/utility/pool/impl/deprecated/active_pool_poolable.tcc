#ifndef ACTIVE_POOL_POOLABLE_TCC_
#define ACTIVE_POOL_POOLABLE_TCC_

#include <type_traits>
#include <vector>
#include "active_poolable.h"

template < class OBJ >
class active_pool< OBJ, 
    typename std::enable_if< std::is_base_of< active_poolable, OBJ >::value >::type > {
public:
    typedef OBJ object_t;
    typedef object_t * object_ptr;

    static const size_t UNKNOWN_INDEX = -1;

    struct queue_node;

    struct object_node {
        size_t  node_idx;
        object_ptr object;
        queue_node   * queue;
        object_node * next, * prev;

        object_node( size_t idx ) : node_idx(idx), object( NULL ), queue(NULL), next(NULL ), prev(NULL) {}
        object_node( size_t idx, object_ptr o, queue_node * q, object_node * n = NULL, object_node * p = NULL) : node_idx(idx), object(o), queue(q), next(n), prev(p) {}
    };

    struct queue_node {
        object_node     * head;
        size_t          size;

        queue_node( ) : head(NULL), size(0) {}
    };

    active_pool() : m_unset_object(NULL)  {}

    size_t getNextPoolIndex() {
        return UNKNOWN_INDEX;
    }

    object_ptr getPoolObject( size_t pool_idx ) const {
        assert( pool_idx < m_objects.size() );

        return m_objects[ pool_idx ]->object;
    }

    void setPoolObject( object_ptr p ) {
        size_t p_idx = p->getActiveIndex();
        assert( p_idx == UNKNOWN_INDEX );

        object_node * t = getObjectNode( );

        t->object = p;
        p->setActiveIndex( t->node_idx );

        updateObject( t, &m_inactive );
    }

    bool unsetPoolObject( object_ptr p ) {
        size_t p_idx = p->getActiveIndex();
        if( p_idx == UNKNOWN_INDEX ) return false;

        object_node * obj = m_objects[ p_idx ];

        obj->object = NULL;

        if( obj->queue != NULL ) {
            remove_object_from_queue( obj );
        } else {
            obj->prev = NULL;
        }

//        obj->next = m_unset_object;
//        m_unset_object = obj;

        insert_object_to_queue( obj, &m_unset_object );
        p->setActiveIndex( UNKNOWN_INDEX );

        return true;
    }

    object_ptr getInactiveObject( ) {
        if( m_inactive.head == NULL ) return NULL;

        object_node * obj = m_inactive.head;

        updateObject( obj, &m_pending );

        return obj->object;
    }

    inline void activateObject( object_ptr o ) {
        assert( o != NULL );
        activateObject( o->getActiveIndex() );
    }

    void activateObject( size_t pool_idx ) {
        updateObject( pool_idx, &m_active );
    }

    inline void pendingObject( object_ptr o ) { 
        assert( o != NULL );
        pendingObject( o->getActiveIndex() );
    }

    void pendingObject( size_t pool_idx ) {
        updateObject( pool_idx, &m_pending );
    }

    inline void inactivateObject( object_ptr o ) {
        assert( o != NULL );
        inactivateObject( o->getActiveIndex() );
    }

    void inactiveObject( size_t pool_idx ) {
        updateObject( pool_idx, &m_inactive );
    }

    void updateObject( size_t pool_idx, queue_node * nQ ) {
        assert( pool_idx != UNKNOWN_INDEX && pool_idx < m_objects.size() );
        object_node * obj = m_objects[pool_idx];
        updateObject( obj, nQ );
    }

    void dump( std::ostream & o ) {
        o << "ACTIVE queue:\n";
        unsigned int n = printQueue( o, m_active.head);
        assert( n == m_active.size );

        o << "PENDING queue:\n";
        n = printQueue(o, m_pending.head );
        assert( n == m_pending.size );

        o << "INACTIVE queue:\n";
        n = printQueue(o, m_inactive.head );
        assert( n == m_inactive.size );
    }

    object_ptr getActiveObjectAt( size_t index ) const {
        object_node * o = m_active.head;

        while( o != NULL && index > 0) { o = o->next; --index; }

        assert( index == 0 );

        return o->object;
    }

    unsigned int printQueue( std::ostream & o, object_node * qobj ) {
        if( qobj == NULL ) {
            o << "\tContains no objects.\n";
            return 0;
        }

        unsigned int n = 0;
        do {
            o << "\t" << ++n << ": " << qobj->node_idx << "\n";
            qobj = qobj->next;
        } while( qobj != NULL );

        return n;
    }

    size_t size() const {
        return m_objects.size();
    }

    size_t getActiveNodeCount() const {
        return m_active.size;
    }

    size_t getInactiveNodeCount() const {
        return m_inactive.size;
    }

    size_t getPendingNodeCount() const {
        return m_pending.size;
    }

    object_ptr activateNextPendingObject() {
        if( m_pending.head == NULL ) return NULL;

        object_node * o = m_pending.head;
        remove_object_from_queue( o );

        updateObject(o, &m_active);

        return o->object;
    }

    object_ptr inactivateNextActiveObject() {
        if( m_active.head == NULL ) return NULL;

        object_node * o = m_active.head;
        remove_object_from_queue( o );

        updateObject(o, &m_inactive);

        return o->object;

    }

    void reset() {
        while(!m_objects.empty() ) {
            object_node * tmp = m_objects.back();
            m_objects.pop_back();

            if(tmp) {
                tmp->object->setActiveIndex( UNKNOWN_INDEX );
                delete tmp;
            }
        }
        m_active.head = NULL;
        m_inactive.head = NULL;
        m_pending.head = NULL;
    }

    virtual ~active_pool() {
        reset();
    }
protected:
    void updateObject( object_node * obj, queue_node * nQ ) {
        if( obj->queue == nQ ) return;

        if( obj->queue != NULL ) {
            remove_object_from_queue( obj );
            assert( obj->queue == NULL );
        }

        insert_object_to_queue( obj, nQ );
        assert( nQ->head == obj );
    }

    object_node * getObjectNode( ) {
        object_node * n = NULL;
        if( m_unset_object != NULL ) {
            n = m_unset_object;
            m_unset_object = m_unset_object->next;
        } else {
            m_objects.push_back( new object_node( m_objects.size() ));
            n = m_objects.back();
        }
        return n;
    }


    void remove_object_from_queue( object_node * obj ) {
        assert( obj->queue != NULL );

        // object is current in another queue
        // remove it from its current queue
        //
        if( obj->next != NULL ) {
            obj->next->prev = obj->prev;
        }

        if( obj->prev != NULL ) {
            obj->prev->next = obj->next;
        } else {
            // head of active queue
            obj->queue->head = obj->next;
        }

        obj->queue->size--;

        obj->next = NULL;
        obj->prev = NULL;
        obj->queue = NULL;
    }

    void insert_object_to_queue( object_node * o, queue_node * q ) {
        o->next = q->head;

        if( q->head != NULL  ) {
            o->prev = q->head->prev;
            q->head->prev = o;
        } else {
            o->prev = NULL;
        }

        q->head = o;
        o->queue = q;
        q->size++;
    };

    std::vector< object_node * > m_objects;

    //object_node * m_unset_object;
    queue_node m_unset_object, m_active, m_inactive, m_pending;
};
#endif  // ACTIVE_POOL_POOLABLE_TCC_
