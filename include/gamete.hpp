#ifndef GAMETE_HPP_
#define GAMETE_HPP_

#include <iostream>

#include "gamete_base.h"

/*
 * A gamete is a logical unit of population genetic simulation
 *
 * Each gamete is a cloneable object
 *
 * This base class provides a "source" and "type" fields.  The
 * intended use of these fields for loose gamete identification.
 * The intended use of the "source" value is to identify the gender
 * of the source individual.  The "type" value is intended to
 * differentiate between the types of gametes an individual can
 * produce.  For example, in a human population a zygote forms
 * from gametes of different sources (male and female). Males
 * can produce gametes of different types (X and Y), where as
 * females only produce one type of gamete (X).
 */
template < class VS >
class gamete : public gamete_base {
public:
    typedef VS  variant_set_t;

    gamete( ) : gamete_base(0) {}
    gamete( gamete_source_t s, gamete_type_t t ) : gamete_base(s, t) {}
    gamete( const gamete & z ) : gamete_base( z.m_id ), m_vars(z.m_vars)  {}
    
    virtual gamete<VS> * clone() const {
        gamete<VS> * child = new gamete<VS>( *this );

        return child;
    }

    void addVariant( typename variant_set_t::key_type v ) {
        m_vars.insert( v );
    }

    bool operator[]( typename variant_set_t::key_type k ) const {
        return m_vars.find(k) != m_vars.end();
    }

    void increasePenetrance() {
        for( typename variant_set_t::iterator it = m_vars.begin(); it != m_vars.end(); it++ ) {
            (*it)->incrementPenetrance();
        }
    }

    void decreasePenetrance() {
        for( typename variant_set_t::iterator it = m_vars.begin(); it != m_vars.end(); it++ ) {
            (*it)->decrementPenetrance();
        }

    }

    size_t size() const {
        return m_vars.size();
    }

    virtual void print( std::ostream & o ) const {
        o << "{" << (int) m_id.id_component.source << ", " << (int) m_id.id_component.type << "}";
    }

    virtual ~gamete() {
        m_vars.clear();
    }
protected:
    variant_set_t   m_vars;
};

#endif  // GAMETE_HPP_
