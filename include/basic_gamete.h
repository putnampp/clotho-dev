#ifndef BASIC_GAMETE_H_
#define BASIC_GAMETE_H_

#include <iostream>

#include <vector>

#include "variant_base.h"
#include "engine/object_manager.h"

using std::vector;

class Gamete {
public:
    typedef vector< variant_base * > variant_set_t;
    typedef Gamete * pointer;

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

    virtual void print( std::ostream & o ) const;

    virtual ~Gamete();
protected:
    variant_set_t * m_variants;
    size_t          m_nActive;
    bool            m_bDirty;

    static object_manager_t m_pool;
};

#endif  // BASIC_GAMETE_H_
