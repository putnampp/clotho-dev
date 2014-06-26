#ifndef INDIVIDUAL_PROPERTIES_DETAILS_HPP_
#define INDIVIDUAL_PROPERTIES_DETAILS_HPP_

//#include "../../clotho.h"
#include "config.hpp"

#include <iostream>
#include <utility>
#include <vector>

#include <boost/pool/object_pool.hpp>

static const unsigned char UNKNOWN_GAMETE_INDEX = (unsigned char) -1;

template < class G, unsigned char P = 2, class S = system_id >
class individual_props {
public:
    typedef G                                       gamete_type;
    typedef typename gamete_type::pointer           gamete_ptr;
    typedef S                                       source_type;
    typedef std::pair< source_type, gamete_ptr >    source_gamete_type;
    typedef source_gamete_type                      gametes_type;

    typedef boost::object_pool< individual_props<G,P,S> >  pool_type;

    static const unsigned char GAMETE_COUNT = P;

    static void * operator new( size_t );
    static void operator delete( void * );

    individual_props();

    void died();
    void setDOB( SystemClock::vtime_t d );   
    bool isAlive() const;

    bool hasSourceGametes() const; 
    void inheritFrom( const system_id & p_id, gamete_ptr g, unsigned char p = UNKNOWN_GAMETE_INDEX );

    gamete_ptr getGamete( unsigned char ) const;

    void reset();

    void release();
 
    virtual ~individual_props();
protected:
    SystemClock::vtime_t        m_dob;
    unsigned char               m_free_gametes;
    gametes_type                m_gametes[GAMETE_COUNT];

    static pool_type            m_pool;
};

//
// IMPLEMENTATION
//
//
template < class G, unsigned char P, class S >
typename individual_props<G,P,S>::pool_type individual_props<G,P,S>::m_pool;

template < class G, unsigned char P, class S >
individual_props<G,P,S>::individual_props() :
    m_dob( SystemClock::POSITIVE_INFINITY ),
    m_free_gametes(GAMETE_COUNT)
{}


template < class G, unsigned char P, class S >
void * individual_props<G,P,S>::operator new( size_t ) {
    return m_pool.malloc();
}


template < class G, unsigned char P, class S >
void individual_props<G,P,S>::operator delete( void * d ) {
    m_pool.free( (individual_props<G,P,S> *) d );
}

template < class G, unsigned char P, class S >
void individual_props<G,P,S>::died() {
    setDOB( SystemClock::POSITIVE_INFINITY );
}

template < class G, unsigned char P , class S>
void individual_props<G,P,S>::setDOB( SystemClock::vtime_t d ) {
    if( m_dob != d ) {
        m_dob = d;
    }
}


template < class G, unsigned char P , class S>
bool individual_props<G,P,S>::isAlive() const {
    return m_dob != SystemClock::POSITIVE_INFINITY;
}

template < class G, unsigned char P, class S >
bool individual_props<G,P,S>::hasSourceGametes() const {
    return (m_free_gametes == 0);
}

template < class G, unsigned char P, class S >
void individual_props<G,P,S>::inheritFrom( const system_id & p_id, gamete_ptr g, unsigned char p ) {
    if( m_free_gametes > 0 ) {
        --m_free_gametes;
        if( p == UNKNOWN_GAMETE_INDEX ) p = m_free_gametes;

        m_gametes[p].first = p_id;
        m_gametes[p].second = g;
    }
}

template < class G, unsigned char P, class S >
typename individual_props<G,P,S>::gamete_ptr individual_props<G,P,S>::getGamete( unsigned char idx ) const {
    return m_gametes[idx].second;
}

template < class G, unsigned char P, class S >
void individual_props<G,P,S>::reset() {
    for( unsigned char i = 0; i < GAMETE_COUNT; ++i ) {
        delete m_gametes[i].second;
    }
    m_free_gametes = GAMETE_COUNT;
}

template < class G, unsigned char P, class S >
void individual_props<G,P,S>::release( ) {
    reset();
    m_pool.free(this);
}

template < class G, unsigned char P, class S >
individual_props<G,P,S>::~individual_props() {
    reset();
}

#endif  // INDIVIDUAL_PROPERTIES_DETAILS_HPP_
