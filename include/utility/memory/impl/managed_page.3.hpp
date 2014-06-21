#ifndef MANAGED_PAGE_3_HPP_
#define MANAGED_PAGE_3_HPP_

#include <stddef.h>
#include <cassert>
#include <cstring>
#include <iostream>

template < class H, class O, unsigned int S >
class ManagedPage {
public:
    typedef O object_t;
    typedef H header_t;
    typedef unsigned short    offset_t;

//    struct index_t {
//        offset_t id, fl_offset;
//        index_t(offset_t _id = 0, offset_t _off = 0) : id(_id), fl_offset(_off) {}
//    };

    struct _node {
//        index_t idx;
        void *   flp;
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

//    static const ManagedPage< H, O, S> TEMPLATE;

/*
    ManagedPage( const ManagedPage< H, O, S > & src ) : 
        header(NULL) 
    {
        memcpy( objects, src.objects, sizeof( _node ) * COUNT );
    }

    ManagedPage< H, O, S > * clone() const {
        ManagedPage< H, O, S > * cl = new ManagedPage< H, O, S >(*this );
        return cl;
    }*/

    inline void setHeader( header_t * h ) {
        header = h;
    }

    template < class FNC >
    object_t * getObject( header_t * h ) {
        static FNC pred;
        
        object_t * obj = pred(h);

        return obj;
    }

    template < typename FNC >
    static void release( object_t * obj ) {
        static FNC pred;
//        index_t * pidx = reinterpret_cast< index_t * >( reinterpret_cast< char * >(obj) - IDX_OFFSET );
//        if( pidx->id < SET_INUSE ) return;

//        pidx->id &= (UNSET_INUSE);

//        void * _flp = reinterpret_cast< void * >( reinterpret_cast< char * >(obj) - sizeof( void * ) );
//        ManagedPage< H, O, S > * pg = reinterpret_cast< ManagedPage< H, O, S > * >( reinterpret_cast< char * >(pidx) - sizeof( _node ) * pidx->id - 2 * sizeof( void * ));

        _node * n = reinterpret_cast< _node * >( reinterpret_cast< char * >(obj) - sizeof( void * ));
        header_t * header = reinterpret_cast< header_t * >( n->flp );
        pred( header, n );
    }

    virtual ~ManagedPage() { }

//protected:
    ManagedPage( ) : 
        header(NULL) 
    {
//        offset_t id = 0;
//        _node * tmp = objects;

        //while( id < COUNT ) {
        //    tmp->idx.id = id++;
        //    tmp->idx.fl_offset = id;
        //    ++tmp;
        //}
        memset( objects, 0, sizeof(_node) * COUNT );
    }
};

//template < class H, class O, unsigned int S >
//const ManagedPage< H, O, S>  ManagedPage< H, O, S>::TEMPLATE;

#endif  // MANAGED_PAGE_3_HPP_
