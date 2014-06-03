#ifndef RNG_GSL_HPP_
#define RNG_GSL_HPP_

#include "general_rng.h"

#include "gsl/gsl_rng.h"
#include "gsl/gsl_randist.h"

#include <sstream>
#include <cassert>

using std::ostringstream;

template < >
class RNG< gsl_rng * > : virtual public iRNG {
public:
    RNG( ) : 
        m_rng(NULL), 
        m_type(""), 
        m_seed(0) 
    {
        initialize();
    }

    RNG( seed_t s ) : 
        m_rng(NULL), 
        m_type(""), 
        m_seed(s) 
    {
        initialize();
    }

    RNG( const type_t & type, seed_t s ) : 
        m_rng(NULL), 
        m_type(type), 
        m_seed(s) 
    {
        initialize();
    }

    RNG( gsl_rng * r, const type_t & t, seed_t s ) : 
        m_rng( r ),
        m_type(t),
        m_seed(s)
    {}

    shared_ptr< iRNG > split() {
        seed_t r = this->nextInt();

        shared_ptr< iRNG> c( new RNG< gsl_rng * >( this->getType(), r ));

        return c;
    }

    const type_t & getType() const {
        return m_type;
    }

    seed_t  getSeed() const {
        return m_seed;
    }

    void    reseed( seed_t s ) {
        m_seed = s;
        gsl_rng_set( m_rng, m_seed );
    }

    unsigned int    getMin() const {
        return gsl_rng_min( m_rng );
    }

    unsigned int    getMax() const {
        return gsl_rng_max( m_rng );
    }

    unsigned int    nextInt() {
        return gsl_rng_get( m_rng );
    }

    unsigned int    nextInt( unsigned int n ) {
        return gsl_rng_uniform_int( m_rng, n );
    }

    bool            nextBool() {
        return (gsl_rng_uniform( m_rng ) < 0.5);
    }

    double          nextUniform() {
        return gsl_rng_uniform( m_rng );
    }

    unsigned int    nextPoisson( double mu ) {
        return gsl_ran_poisson( m_rng, mu );
    }

    virtual ~RNG< gsl_rng * >() {
        if( m_rng ) gsl_rng_free( m_rng );
    }
protected:
    void initialize() {
        const gsl_rng_type * T = NULL;
        if( m_type.empty() ) {
            gsl_rng_env_setup();
            T = gsl_rng_default;
            m_type = T->name;
            m_seed = gsl_rng_default_seed;
        } else {
            const gsl_rng_type **t, **t0;
            t0 = gsl_rng_types_setup();
            for( t = t0; *t != NULL; t++) {
                ostringstream oss( (*t)->name );
                if( oss.str() == m_type ) {
                    T = *t;
                    break;
                }
            }
        }

        m_rng = gsl_rng_alloc( T );
        reseed( m_seed );

        assert( m_rng != NULL );
    }

    gsl_rng *   m_rng;
    type_t      m_type;
    seed_t      m_seed;
};

typedef RNG< gsl_rng * > GSL_RNG;

#endif  // RNG_GSL_HPP_
