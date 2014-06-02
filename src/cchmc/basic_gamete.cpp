#include "basic_gamete.h"

#include <algorithm>

Gamete Gamete::EMPTY_GAMETE;

Gamete::object_manager_t Gamete::m_pool;

Gamete::Gamete() : m_variants( NULL ), m_nActive( 1 ), m_bDirty(false) {}

Gamete::pointer Gamete::copy() {
    ++m_nActive;
    return this;
}

Gamete::pointer Gamete::clone() {
    pointer g = m_pool.malloc();
    
    if( g->m_variants != NULL ) 
        g->m_variants->clear();
    else {
        g->m_variants = new variant_set_t();

        assert( g->m_variants != NULL );
    }

    g->m_nActive = 1;

    std::cout << "Cloned gamete: " << std::endl;
    assert( g->m_variants != NULL );
    if( this->m_variants != NULL ) {
        g->m_variants->insert(g->m_variants->begin(),this->m_variants->begin(), this->m_variants->end());
        g->m_bDirty = this->m_bDirty;
    } else {
        g->m_bDirty = false;
    }

    return g;
}

void Gamete::addVariant( variant_base * vptr ) {
    assert(m_variants != NULL );
    m_variants->push_back( vptr );
    m_bDirty = true;
}

size_t Gamete::size() const {
    return m_variants->size();
}

bool Gamete::operator[]( variant_base * vptr ) {
    if( m_bDirty ) { 
        std::sort( m_variants->begin(), m_variants->end() );
        m_bDirty = false;
    }

    return std::binary_search( m_variants->begin(), m_variants->end(), vptr );
}

void * Gamete::operator new( size_t  s) {
    Gamete::pointer res = m_pool.malloc();

    if( res->m_variants != NULL ) {
        std::cout << "Re-using existing variant set" << std::endl;
        res->m_variants->clear();
    } else {
        std::cout << "Should be getting here" << std::endl;
        res->m_variants = new variant_set_t();

        assert( res->m_variants != NULL );
    }

    return res;
}

void Gamete::operator delete( void * ptr ) {
    Gamete::pointer g = (Gamete::pointer) ptr;

    if( --g->m_nActive == 0 ) {
        std::cout << "Freeing" << std::endl;
        m_pool.free( (Gamete::pointer) ptr );
    }
}

void Gamete::print( std::ostream & o ) const {}

Gamete::~Gamete() {
    if( m_variants != NULL ) {
        std::cout << "Deleting" << std::endl;
        delete m_variants;
        m_variants = NULL;
    }
}
