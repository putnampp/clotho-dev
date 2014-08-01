#ifndef PAGER_HPP_
#define PAGER_HPP_

#include <iostream>
#include <stddef.h>

template < class T, unsigned int S = 4096 >
class Pager {
public:
    typedef T             object_t;
    typedef unsigned short offset_t;

    struct index_t {
        offset_t id, fl_offset;
        index_t( offset_t _id = 0, offset_t _off = 0 ) : id(_id), fl_offset(_off) {}
    };

    struct free_list_t {
        offset_t free_count, available;
        free_list_t( offset_t c = 0, offset_t a  = 0) : free_count(c), available(a) {}
    };

    struct Header {
        Pager< T, S > * next;
        free_list_t free_list;
        Header( Pager< T, S > * n = NULL, offset_t f = 0, offset_t a = -1) : next( n ), free_list(f,a) {}
    };

    struct _node {
        index_t idx;
        object_t obj;
    };

    enum {  PS = S,
            COUNT = ((S - sizeof( Header ) - sizeof( void * ) ) / sizeof( _node )), // sizeof( void * ) for virtual table pointer
            SET_INUSE = (1 << ( sizeof( offset_t ) * 8 - 1 )),
            UNSET_INUSE = ~SET_INUSE,
            IDX_OFFSET = offsetof( struct _node, obj)
         };

    Header      header;
    _node       objects[ COUNT ];

    static const Pager< T, S > TEMPLATE;

    Pager( Pager< T, S > * n ) :
        header( n, COUNT, 0 ) {
        memcpy( objects, n->objects, sizeof(_node) * COUNT );
    }

    Pager< T, S > * clone() {
        Pager< T, S > * res = new Pager< T, S >( this );

        return res;
    }

    object_t * malloc() {
        object_t * res = NULL;
        Pager< T, S > * tmp = this;
        do {
            if( tmp->header.free_list.free_count == 0 ) {
                tmp = tmp->header.next;
            } else {
                res = tmp->getNextAvailable();
                break;
            }
        } while( tmp != NULL );

        if( res == NULL ) {
            tmp = new Pager<T,S>( this->header.next );
            this->header.next = tmp;

            res = tmp->getNextAvailable();
            assert( res != NULL );
        }

        return res;
    }

    bool isOnPage( object_t * obj ) {
        char * b = reinterpret_cast< char * >(objects);
        char * e = b + COUNT * sizeof(_node);
        char * o = reinterpret_cast< char * >(obj);
        return ( b <= o && o < e );
    }

    void freeObject( object_t * obj ) {
        if( isOnPage( obj ) ) {
            index_t * idx = reinterpret_cast< index_t * >( reinterpret_cast< char * >(obj) - IDX_OFFSET );

            assert( idx->key.id >= SET_INUSE );

            idx->key.id &= (UNSET_INUSE);
            idx->key.fl_offset = header.free_list.available;
            header.free_list.available = idx->key.id;
            ++header.free_list.free_count;
        } else {
            std::cout << "object not found in space" << std::endl;
        }
    }

    static void release( object_t * obj ) {
        index_t * idx = reinterpret_cast< index_t * >( reinterpret_cast< char * >(obj) - IDX_OFFSET );
        assert( idx->key.id >= SET_INUSE );
        idx->key.id &= (UNSET_INUSE);

        Header * header = reinterpret_cast< Header * >( reinterpret_cast< char * >(idx) - sizeof( _node ) * idx->key.id - sizeof(Header));

        idx->key.fl_offset = header->free_list.available;
        header->free_list.available = idx->key.id;
        ++header->free_list.free_count;
    }

    virtual ~Pager() {
        if( header.next != NULL ) {
            // cascading clean up of linked pages
            delete header.next;
        }
    }
protected:
    object_t * getNextAvailable() {
        if( header.free_list.free_count == 0 ) return NULL;

        object_t * rval = NULL;
        offset_t avail = header.free_list.available;
        _node * onode = &objects[avail];
        if( onode->idx.id < SET_INUSE ) {
            onode->idx.id |= SET_INUSE;
            rval = &onode->obj;
            if( --header.free_list.free_count > 0 ) {
                assert( onode->idx.fl_offset != COUNT );
                header.free_list.available = onode->idx.fl_offset;
            } else {
                header.free_list.available = COUNT;
            }
        } else {
            std::cout << "Attempted to get object which is still in use: " << avail << ", " << onode->idx.id << std::endl;
            assert(false);
        }

        return rval;
    }

    Pager( ) : header(NULL, COUNT, 0) {
        offset_t id = 0;
        _node * tmp = objects;

        while( id < COUNT ) {
            tmp->idx.id = id++;
            tmp->idx.fl_offset = id;
            ++tmp;
        }
    }
};

#endif  // PAGER_HPP_
