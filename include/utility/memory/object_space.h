#ifndef OBJECT_SPACE_HPP_
#define OBJECT_SPACE_HPP_

#include <stddef.h>
#include <cassert>
#include <cstring>
#include <iostream>

class ObjectSpace {
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
        object_t * obj;
    };

    void        * header;
    _node       objects[ COUNT ];

    ObjectSpace( const ObjectSpace & src ) : 
        header(NULL) 
    {
        memcpy( objects, src.objects, sizeof( _node ) * COUNT );
    }

    ObjectSpace< H, O, S > * clone() const {
        ObjectSpace< H, O, S > * cl = new ObjectSpace< H, O, S >(*this );
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

        if( pidx->id < SET_INUSE ) return;

        pidx->id &= (UNSET_INUSE);

        ObjectSpace< H, O, S > * pg = reinterpret_cast< ObjectSpace< H, O, S > * >( reinterpret_cast< char * >(pidx) - sizeof( _node ) * pidx->id - 2 * sizeof( void * ));

        pred( pg->header, pidx );
    }

    virtual ~ObjectSpace() { }

protected:
    ObjectSpace( ) : 
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

#endif  // OBJECT_SPACE_HPP_
