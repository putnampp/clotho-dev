#ifndef OBJECT_MANAGER_HPP_
#define OBJECT_MANAGER_HPP_

//#include "pager.2.hpp"
#include "managed_page.hpp"
#include "splay_tree.hpp"

#include <utility>
#include <ostream>

typedef std::pair< unsigned short, void * > free_page_t;
struct free_page_comp {
    bool operator()( const free_page_t & lhs, const free_page_t & rhs ) {
        return lhs.first > rhs.first;
    }
};

inline std::ostream & operator<<( std::ostream & o, const free_page_t & fp ) {
    o << "{" << fp.first << ", " << fp.second << "}";
    return o;
}

inline bool has_free_objects( const free_page_t & fp ) {
    return fp.first > 0;
}

template < class O, unsigned int S = (1 << 12) >
class ObjectManager {
public:
    typedef O object_t;

    typedef splay_tree< free_page_t, free_page_comp > splay_tree_t;
    typedef typename splay_tree_t::node container_node_t;
    typedef ManagedPage< container_node_t, O, S > page_t;

    enum {  PAGE_SIZE = S,
            COUNT = page_t::COUNT
         };

    ObjectManager( ) : m_free_objs(0) { }

    object_t * malloc() {
        container_node_t * fpage = m_pages.getRoot();
        page_t * tmp = NULL;

        if( fpage == NULL || m_free_objs == 0 ) {
            free_page_t k( page_t::COUNT, NULL );
            fpage = m_pages.insert(k);

            tmp = page_t::TEMPLATE.clone();
            tmp->setContainerNode(fpage);

            fpage->key.second = tmp;
            m_free_objs += page_t::COUNT;
        } else {
            if( fpage->key.first == 0 ) {
                fpage = m_pages.find( &has_free_objects );
                assert( fpage != NULL );
            }
            tmp = (page_t*) fpage->key.second;
        }

        object_t * obj = tmp->getNextAvailable();
        assert( obj != NULL );

        --fpage->key.first;
        --m_free_objs;
        return obj;
    }

    void dump( std::ostream & out ) {
        m_pages.dump(out);
    }

    void free( object_t * ptr ) {
        container_node_t * p = page_t::release( ptr );

        if( p ) {
            ++p->key.first;
            ++m_free_objs;
        }
    }

    virtual ~ObjectManager() {
        container_node_t * r = m_pages.getRoot();
        while( r != NULL ) {
            delete (page_t * ) r->key.second; // delete the page
            m_pages.erase(r);

            r = m_pages.getRoot();
        }
    }

protected:
    splay_tree_t m_pages;
    size_t       m_free_objs;
};

#endif  // OBJECT_MANAGER_HPP_
