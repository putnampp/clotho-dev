#ifndef THREADED_RNG_HPP_
#define THREADED_RNG_HPP_

#include <pthread.h>
#include "irng.h"

#include <cassert>
#include <map>

using std::map;

class ThreadedRNG : public iRNG {
public:
    typedef shared_ptr< iRNG >          rng_t;
    typedef map< pthread_t, rng_t >     rng_group_t;

    ThreadedRNG( shared_ptr< iRNG > r ) :
        m_rng( r ),
        m_type( r->getType() ),
        m_seed( r->getSeed() ),
        m_min( m_rng->getMin() ),
        m_max( m_rng->getMax() )
    {
        m_lock = PTHREAD_MUTEX_INITIALIZER;
        getRNG();
    }

    shared_ptr< iRNG > split() {
        pthread_mutex_lock( &m_lock );
        shared_ptr< iRNG > r = m_rng->split();
        pthread_mutex_unlock( &m_lock );

        shared_ptr< iRNG > nrng( new ThreadedRNG( r ) );

        return nrng;
    }

    const type_t & getType() const { return m_type; }
    seed_t    getSeed() const { return m_seed; }

    void    reseed( seed_t s ) {
        rng_t r = getRNG();

        r->reseed( s );
    }

    unsigned int    getMin() const {
        return m_min;
    }

    unsigned int    getMax() const { 
        return m_max;
    }

    unsigned int    nextInt() {
        rng_t r = getRNG();
        return r->nextInt();
    }

    unsigned int    nextInt( unsigned int max ) {
        rng_t r = getRNG();
        return r->nextInt( max );
    }

    bool            nextBool() {
        rng_t r = getRNG();
        return r->nextBool();
    }

    double          nextUniform() {
        rng_t r = getRNG();
        return r->nextUniform();
    }

    /*
    * Common Distribution interfaces
    */ 
    unsigned int    nextPoisson( double mu ) {
        rng_t r = getRNG();
        return r->nextPoisson( mu );
    }
    
    virtual ~ThreadedRNG() {
        m_thread_rng.clear();
    }
protected:

    rng_t       getRNG() {
        pthread_t s = pthread_self();
        
        pthread_mutex_lock( &m_lock );

        rng_group_t::iterator it = m_thread_rng.find(s);
        if( it == m_thread_rng.end() ) {
            rng_t r = m_rng->split();
            std::pair< rng_group_t::iterator, bool > res = m_thread_rng.insert( std::make_pair( s, r ) );
            assert( res.second );
            it = res.first;
        }

        pthread_mutex_unlock( &m_lock );

        return it->second;
    }

    rng_t       m_rng;

    type_t m_type;
    seed_t m_seed;

    unsigned int m_min, m_max;

    rng_group_t m_thread_rng;
    pthread_mutex_t     m_lock;
};

#endif  // THREADED_RNG_HPP_
