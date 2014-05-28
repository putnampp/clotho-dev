#ifndef MANAGED_PAGE_2_HPP_
#define MANAGED_PAGE_2_HPP_

#include <stddef.h>
#include <cassert>
#include <cstring>

template < class H, class O, unsigned int S >
class ManagedPage {
public:
    typedef O object_t;
    typedef H header_t;
    typedef unsigned short    offset_t;

    struct index_t {
        offset_t id, fl_offset;
        index_t(offset_t _id = 0, offset_t _off = 0) : id(_id), fl_offset(_off) {}
    };

    struct _node {
        index_t idx;
        object_t obj;
    };

    enum {  PS = S,
            COUNT = ((PS - sizeof( header_t * ) - sizeof( void * ) ) / sizeof( _node )), // sizeof( void * ) for virtual table pointer
            SET_INUSE = (1 << ( sizeof( offset_t ) * 8 - 1 )),
            UNSET_INUSE = ~SET_INUSE,
            IDX_OFFSET = offsetof( struct _node, obj)
    };


    header_t    * header;
    _node       objects[ COUNT ];

    static const ManagedPage< H, O, S> TEMPLATE;

    ManagedPage( const ManagedPage< H, O, S > & src ) : 
        header(NULL) 
    {
        memcpy( objects, src.objects, sizeof( _node ) * COUNT );
    }

    ManagedPage< H, O, S > * clone() const {
        ManagedPage< H, O, S > * cl = new ManagedPage< H, O, S >(*this );
        return cl;
    }

    inline void setHeader( header_t * h ) {
        header = h;
    }

    template < class FNC >
    object_t * getObject( header_t * h ) {
        static FNC pred;
        
        object_t * obj = pred(h, objects);

        return obj;
    }

    template < typename FNC >
    static void release( object_t * obj ) {
        static FNC pred;
        index_t * pidx = reinterpret_cast< index_t * >( reinterpret_cast< char * >(obj) - IDX_OFFSET );

        assert( pidx->id >= SET_INUSE );
        pidx->id &= (UNSET_INUSE);

        header_t * header = reinterpret_cast< header_t * >( reinterpret_cast< char * >(pidx) - sizeof( _node ) * pidx->id - sizeof(header_t *));

        pred( header, pidx );
    }

    virtual ~ManagedPage() { }

protected:
    ManagedPage( ) : 
        header(NULL) 
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

template < class H, class O, unsigned int S >
const ManagedPage< H, O, S>  ManagedPage< H, O, S>::TEMPLATE;

#endif  // MANAGED_PAGE_2_HPP_
