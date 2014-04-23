#ifndef GAMETE_HPP_
#define GAMETE_HPP_

#include <ostream>

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
class gamete {
public:
    typedef unsigned short gamete_key_t;
    typedef unsigned char gamete_source_t;
    typedef unsigned char gamete_type_t;

    typedef VS  variant_set_t;

    gamete( ) : m_id(0) {}
    gamete( gamete_source_t s, gamete_type_t t ) : m_id(s, t ) {}
    gamete( const gamete & z ) : m_id( z.m_id ), m_vars(z.m_vars)  {}
    
    virtual gamete<VS> * clone() const {
        gamete<VS> * child = new gamete<VS>( *this );

        return child;
    }

    inline gamete_type_t getType() const {
        return m_id.id_component.type;
    }

    inline gamete_source_t getSource() const {
        return m_id.id_component.source;
    }

    void setSource( gamete_source_t id ) {
        m_id.id_component.source = id;
    }

    void setType( gamete_type_t id ) {
        m_id.id_component.type = id;
    }

    void addVariant( typename variant_set_t::key_type v ) {
        m_vars.insert( v );
    }

    virtual void print( std::ostream & o ) const {
        o << "{" << (int) m_id.id_component.source << ", " << (int) m_id.id_component.type << "}";
    }

    virtual ~gamete() {
        m_vars.clear();
    }
protected:
    union _gamete_id {
        gamete_key_t    key;
        struct z {
            gamete_source_t source;
            gamete_type_t   type;
            z( gamete_source_t s, gamete_type_t t ) : source(s), type(t) {}
        } id_component;

        _gamete_id( ) : key(0) {}
        _gamete_id( gamete_key_t k ) : key(k) {}
        _gamete_id( gamete_source_t s, gamete_type_t t ) : id_component( s, t) {}
        _gamete_id( const _gamete_id & id ) : key( id.key ) {}
    };

    _gamete_id m_id;
    variant_set_t   m_vars;
};

#endif  // GAMETE_HPP_
