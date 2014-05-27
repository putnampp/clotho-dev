#ifndef MANAGED_PAGE_HPP_
#define MANAGED_PAGE_HPP_

#include <stddef.h>
#include <cassert>
#include <cstring>

template < class N, class O, unsigned int S >
class ManagedPage {
public:
    typedef O object_t;
    typedef N container_node_t;
    typedef unsigned short    offset_t;

    struct index_t {
        offset_t id, fl_offset;
        index_t(offset_t _id = 0, offset_t _off = 0) : id(_id), fl_offset(_off) {}
    };

    struct free_list_t {
        container_node_t * parent;
        offset_t free_count, available;
        free_list_t( container_node_t * p, offset_t c = 0, offset_t _a = -1 ) : parent(p), free_count( c ), available(_a) {}
    };

    struct _node {
        index_t idx;
        object_t obj;
    };

    enum {  PS = S,
            COUNT = ((PS - sizeof( free_list_t ) - sizeof( void * ) ) / sizeof( _node )), // sizeof( void * ) for virtual table pointer
            SET_INUSE = (1 << ( sizeof( offset_t ) * 8 - 1 )),
            UNSET_INUSE = ~SET_INUSE,
            IDX_OFFSET = offsetof( struct _node, obj)
    };


    free_list_t header;
    _node       objects[ COUNT ];

    static const ManagedPage< N, O, S> TEMPLATE;

    ManagedPage( container_node_t * p, const ManagedPage< N, O, S > & src ) : 
        header(p, COUNT, 0) 
    {
        memcpy( objects, src.objects, sizeof( _node ) * COUNT );
    }

    ManagedPage< N, O, S > * clone( container_node_t * p) const {
        ManagedPage< N, O, S > * cl = new ManagedPage<N, O,S >(p, *this );
        return cl;
    }

    object_t * getNextAvailable() {
        if( header.free_count == 0 ) return NULL;

        object_t * rval = NULL;
        offset_t avail = header.available;
        _node * onode = &objects[avail];
        if( onode->idx.id < SET_INUSE ) {
            onode->idx.id |= SET_INUSE;
            rval = &onode->obj;
            if( --header.free_count > 0 ) {
                assert( onode->idx.fl_offset != COUNT );
                header.available = onode->idx.fl_offset;
            } else {
                header.available = COUNT;
            }
        } else {
            assert(false);
        }

        return rval;
    }

    static container_node_t * release( object_t * obj ) {
        index_t * idx = reinterpret_cast< index_t * >( reinterpret_cast< char * >(obj) - IDX_OFFSET );
        assert( idx->id >= SET_INUSE );
        idx->id &= (UNSET_INUSE);

        free_list_t * header = reinterpret_cast< free_list_t * >( reinterpret_cast< char * >(idx) - sizeof( _node ) * idx->id - sizeof(free_list_t));

        idx->fl_offset = header->available;
        header->available = idx->id;
        ++header->free_count;

        return header->parent;
    }

    virtual ~ManagedPage() { }

protected:
    ManagedPage( ) : 
        header(NULL, COUNT, 0 ) 
    {
        offset_t id = 0;
        _node * tmp = objects;

        while( id < COUNT ) {
            tmp->idx.id = id++;
            tmp->idx.fl_offset = id;
            ++tmp;
        }
    }
};

template < class N, class O, unsigned int S >
const ManagedPage< N, O, S>  ManagedPage< N, O, S>::TEMPLATE;

#endif  // MANAGED_PAGE_HPP_
