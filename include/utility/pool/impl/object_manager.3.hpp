#ifndef OBJECT_MANAGER_3_HPP_
#define OBJECT_MANAGER_3_HPP_

#include "utility/memory/managed_page.h"
#include "utility/splay_tree.hpp"

#include <utility>
#include <ostream>

struct page_header {
    size_t free_count;
    void * available;
    void * page;
    page_header( size_t fc, void * a, void * p ) : free_count(fc), available(a), page(p) {}
};

struct page_header_comp {
    bool operator()( const page_header & lhs, const page_header & rhs ) {
        return lhs.free_count > rhs.free_count;
    }
};

inline std::ostream & operator<<( std::ostream & o, const page_header & fp ) {
    o << "{" << fp.free_count << ", " << fp.available << ", " << fp.page << "}";
    return o;
}

inline bool has_free_objects( const page_header & fp ) {
    return fp.free_count > 0;
}

template < class O, unsigned int S = (1 << 12) >
class ObjectManager {
public:
    typedef O object_t;

    typedef splay_tree< page_header, page_header_comp > splay_tree_t;
    typedef typename splay_tree_t::node header_t;
    typedef ManagedPage< header_t, O, S > page_t;
    typedef typename page_t::_node   object_node_t;

    struct update_header_on_get {
        object_t * operator()( header_t * h ) {
            page_header * ph = &h->key;
            typename page_t::_node * onode = reinterpret_cast< typename page_t::_node * >(ph->available);
            assert( onode != NULL );
            if( --ph->free_count > 0 ) {
                //assert( onode->idx.fl_offset != COUNT );
                //ph->available = onode->idx.fl_offset;
                //
                if( onode->flp == NULL ) {
                    ph->available = onode + 1;
                } else {
                    ph->available = onode->flp;
                }
                onode->flp = ph;
            } else {
                ph->available = NULL;
            }
            return &onode->obj;
        }
    };

    struct update_header_on_release {
        void operator()( header_t * h, typename page_t::_node * n ) {
            page_header * ph = &h->key;
            n->flp = ph->available;
            ph->available = n;
            ++ph->free_count;
        }
    };

    enum {  PAGE_SIZE = S,
            COUNT = page_t::COUNT
         };

    ObjectManager( ) : m_free_objs(0) { }

    object_t * malloc() {
        header_t * fpage = m_pages.getRoot();
        if( fpage == NULL || m_free_objs == 0 ) {
            page_header k( page_t::COUNT, NULL, NULL );

            fpage = m_pages.insert(k);
            page_t * tmp = new page_t();
            tmp->setHeader(fpage);

            fpage->key.available = tmp->objects;
            fpage->key.page = (void *) tmp;

            m_free_objs += page_t::COUNT;
        } else {
            if( fpage->key.free_count == 0 ) {
                fpage = m_pages.find( &has_free_objects );
                assert( fpage != NULL );
            }
        }

        assert( fpage != NULL && fpage->key.available != NULL );
        page_t * tmp = reinterpret_cast< page_t * >( fpage->key.page );
        object_t * obj = tmp->template getObject<update_header_on_get>(fpage);
        --m_free_objs;
        return obj;
    }

    void dump( std::ostream & out ) {
        m_pages.dump(out);
    }

    void free( object_t * ptr ) {
        page_t::template release< update_header_on_release >( ptr );
    }

    virtual ~ObjectManager() {
        header_t * r = m_pages.getRoot();
        while( r != NULL ) {
            delete (page_t * ) r->key.page; // delete the page
            m_pages.erase(r);

            r = m_pages.getRoot();
        }
    }

protected:
    splay_tree_t m_pages;
    size_t       m_free_objs;
};

#endif  // OBJECT_MANAGER_HPP_
