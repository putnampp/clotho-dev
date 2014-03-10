#include "rng_gsl.hpp"

#include <iostream>
#include <sstream>
#include <cassert>

using std::ostringstream;
using std::cout;
using std::endl;

template<>
void RNG< gsl_rng *>::reseed( seed_t s ) {
    m_seed = s;
    gsl_rng_set( m_rng, m_seed );
}

template<>
void RNG< gsl_rng *>::initialize() {
    gsl_rng_env_setup();
    const gsl_rng_type * T = NULL;
    if( m_type.empty() ) {
        T = gsl_rng_default;
        cout << "Setting RNG type to: " << T->name << endl;
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

template<>
RNG< gsl_rng * >::RNG( ) :
    m_rng( NULL ),
    m_type( "" ),
    m_seed( 0 )
{
    cout << "Basic GSL RNG" << endl;
    initialize();
}

template<>
RNG< gsl_rng * >::RNG( seed_t s ) :
    m_rng( NULL ),
    m_type( "" ),
    m_seed( s )
{
    initialize();
}

template <>
RNG< gsl_rng * >::RNG( const type_t & t, seed_t s ) :
    m_rng( NULL ),
    m_type( t ),
    m_seed( s )
{
    initialize();
}

template<>
unsigned int RNG< gsl_rng * >::getMin() const {
    return gsl_rng_min( m_rng );
}

template<>
unsigned int RNG< gsl_rng * >::getMax() const {
    return gsl_rng_max( m_rng );
}

template<>
unsigned int RNG< gsl_rng * >::nextInt() {
    return gsl_rng_get( m_rng );
}

template<>
unsigned int RNG< gsl_rng * >::nextInt( unsigned int n) {
    return gsl_rng_uniform_int( m_rng, n );
}

template<>
bool    RNG< gsl_rng * >::nextBool() {
    return (gsl_rng_uniform( m_rng ) < 0.5);
}

template<>
double  RNG< gsl_rng * >::nextUniform() {
    return gsl_rng_uniform( m_rng );
}

template<>
unsigned int RNG< gsl_rng * >::nextPoisson( double mu ) {
    return gsl_ran_poisson( m_rng, mu );
}

template<>
RNG< gsl_rng *>::~RNG< gsl_rng * >() {
    if( m_rng ) gsl_rng_free( m_rng );
}
