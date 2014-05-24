#ifndef PAGER_HPP_
#define PAGER_HPP_

#include <iostream>
#include <stddef.h>

template < class T, unsigned int S = 4096 >
class Pager {
public:
    typedef T             object_t;
    typedef unsigned short offset_t;

    union index_t {
        unsigned int value;
        struct k {
            offset_t id, fl_offset;
            k( offset_t _id, offset_t _off ): id(_id), fl_offset(_off) {}
        } key;
        index_t(unsigned int v = 0) : value(v) {}
        index_t( offset_t _id, offset_t _off ) : key(_id, _off) {}
    };

    union free_list_t {
        unsigned int block;
        struct s {
            offset_t free_count, available;
            s( offset_t n, offset_t a ) : free_count(n), available(a) {}
        } stats;

        free_list_t( unsigned int b = 0 ) : block(b) {}
        free_list_t( offset_t c, offset_t a ) : stats(c, a) {}
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

    Pager( Pager< T, S > * n = NULL ) : header(n, COUNT, 0) {
//        memset( objects, 0, COUNT * sizeof( _node ) );
        offset_t id = 0;
        _node * tmp = objects;

        while( id < COUNT ) {
            tmp->idx.key.id = id++;
            tmp->idx.key.fl_offset = id;
            ++tmp;
        }
    }

    object_t * malloc() {
        object_t * res = NULL;
        Pager< T, S > * tmp = this;
        do {
            if( tmp->header.free_list.stats.free_count == 0 ) {
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

//        std::cout << "Allocated: " << (void *)res << std::endl;
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
            idx->key.fl_offset = header.free_list.stats.available;
            header.free_list.stats.available = idx->key.id;
            ++header.free_list.stats.free_count;
        } else {
            std::cout << "object not found in space" << std::endl;
        }
    }

    static void release( object_t * obj ) {
//        std::cout << "Releasing: " << (void *)obj << std::endl;
        index_t * idx = reinterpret_cast< index_t * >( reinterpret_cast< char * >(obj) - IDX_OFFSET );
//        std::cout << "found node id: " << (void *) idx << " - "<< idx->key.id << "," << idx->key.fl_offset << std::endl;
        assert( idx->key.id >= SET_INUSE );
        idx->key.id &= (UNSET_INUSE);

        Header * header = reinterpret_cast< Header * >( reinterpret_cast< char * >(idx) - sizeof( _node ) * idx->key.id - sizeof(Header));

        idx->key.fl_offset = header->free_list.stats.available;
        header->free_list.stats.available = idx->key.id;
        ++header->free_list.stats.free_count;
    }

    virtual ~Pager() {
        if( header.next != NULL ) {
            // cascading clean up of linked pages
            delete header.next;
        }
    }
protected:
    object_t * getNextAvailable() {
        if( header.free_list.stats.free_count == 0 ) return NULL;

        object_t * rval = NULL;
        offset_t avail = header.free_list.stats.available;
//        _node & onode = objects[ avail ];
        _node * onode = &objects[avail];
        if( onode->idx.key.id < SET_INUSE ) {
//            std::cout << "updating key id: " << &onode->idx << " - " << onode->idx.key.id << std::endl;
            onode->idx.key.id |= SET_INUSE;
//            std::cout << "updated key id: " << onode->idx.key.id << std::endl;
            rval = &onode->obj;
            if( --header.free_list.stats.free_count > 0 ) {
                assert( onode->idx.key.fl_offset != COUNT );
                header.free_list.stats.available = onode->idx.key.fl_offset;
            } else {
                header.free_list.stats.available = COUNT;
            }
        } else {
            std::cout << "Attempted to get object which is still in use: " << avail << ", " << onode->idx.key.id << std::endl;
            assert(false);
        }

        return rval;
    }
};

#endif  // PAGER_HPP_
