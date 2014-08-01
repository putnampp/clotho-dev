#ifndef BASIC_GAMETE_H_
#define BASIC_GAMETE_H_

#include <iostream>

#include <vector>

#include "variant_base.h"
#include "utility/pool/object_manager.h"

using std::vector;

class Gamete {
public:
    typedef vector< variant_base * > variant_set_t;
    typedef Gamete * pointer;
    typedef typename variant_set_t::const_iterator var_iterator;

    typedef ObjectManager< Gamete > object_manager_t;

    static Gamete EMPTY_GAMETE;

    Gamete();

    pointer clone() const;
    pointer copy();

    size_t size() const;

    void addVariant( variant_base * var );

    bool operator[]( variant_base * );

    static void * operator new( size_t );
    static void operator delete( void * );

    var_iterator begin() const;
    var_iterator end() const;

    virtual void print( std::ostream & o ) const;

    virtual ~Gamete();
protected:
    variant_set_t * m_variants;
    size_t          m_nActive;
    bool            m_bDirty;

    static object_manager_t m_pool;
};

#endif  // BASIC_GAMETE_H_
