#ifndef OBJECT_MANAGER_HPP_
#define OBJECT_MANAGER_HPP_

//#include "pager.2.hpp"
#include "managed_page.2.hpp"
#include "splay_tree.hpp"

#include <utility>
#include <ostream>

//typedef std::pair< unsigned short, void * > free_page_t;
//struct free_page_comp { 
//    bool operator()( const free_page_t & lhs, const free_page_t & rhs ) {
//        return lhs.first > rhs.first;
//    }
//};
//
struct page_header {
    unsigned short free_count, available;
    void * page;
    page_header( unsigned short fc, unsigned short a, void * p ) : free_count(fc), available(a), page(p) {}
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
        object_t * operator()( header_t * h, typename page_t::_node * objs ) {
            page_header * ph = &h->key;
            typename page_t::_node * onode = &objs[ph->available];
            if( onode->idx.id < page_t::SET_INUSE ) {
                onode->idx.id |= page_t::SET_INUSE;
                if( --ph->free_count > 0 ) {
                    assert( onode->idx.fl_offset != COUNT );
                    ph->available = onode->idx.fl_offset;
                } else {
                    ph->available = COUNT;
                }
            } else {
                assert(false);
            }
            return &onode->obj;
        }
    };

    struct update_header_on_release {
        void operator()( header_t * h, typename page_t::index_t * idx ) {
            page_header * ph = &h->key;
            idx->fl_offset = ph->available;
            ph->available = idx->id;
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
            page_header k( page_t::COUNT, 0, NULL );

            fpage = m_pages.insert(k);
            fpage->key.page = page_t::TEMPLATE.clone();

            ((page_t*)fpage->key.page)->setHeader(fpage);

            m_free_objs += page_t::COUNT;
        } else {
            if( fpage->key.free_count == 0 ) {
                fpage = m_pages.find( &has_free_objects );
                assert( fpage != NULL );
            }
        }

        assert( fpage != NULL && fpage->key.available < page_t::COUNT );
//        object_t * obj = tmp->template getObject<update_header_on_get>( fpage );
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
