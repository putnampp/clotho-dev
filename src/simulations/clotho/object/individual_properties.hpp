#ifndef INDIVIDUAL_PROPERTIES_HPP_
#define INDIVIDUAL_PROPERTIES_HPP_

#include "../clotho.h"

#include <iostream>

/*
possible interface for all individual properties:

struct iproperties {
    // necessary in birth/death state
    virtual void setDOB( event::SystemClock::vtime_t dob ) = 0;
    virtual bool isAlive() const = 0;

    // necessary in default inherit_event
    virtual bool hasSourceGametes() const = 0;
    virtual void inheritFrom( system_id pid, gamete_t * g, unsigned char gamete_idx ) = 0;
    virtual void reset() = 0;
};


 */

static const unsigned char UNKNOWN_GAMETE_INDEX = (unsigned char) -1;

template < /*class VT,*/ class G, unsigned char P >
class individual_props;

template < /*class VT,*/ class G >
class individual_props< /*VT,*/ G, 1 > {
public:
//    typedef VT variant_t;
    typedef G gamete_t;
    typedef typename gamete_t::pointer gamete_ptr;

    enum { GAMETE_COUNT = 1 };

    individual_props() :
        p0_id( ),
        p0( NULL )
    {}

    void died() {
        setDOB( SystemClock::POSITIVE_INFINITY);
    }

    void setDOB( SystemClock::vtime_t d ) {
        if( m_dob != d ) {
            if( d != SystemClock::POSITIVE_INFINITY ) {
                p0->increasePenetrance();
            } else {
                p0->decreasePenetrance();
            }
            m_dob = d;
        }
    }
    
    bool isAlive() const { return m_dob != SystemClock::POSITIVE_INFINITY; }

    bool hasSourceGametes() const { return (p0 != NULL); }

    void inheritFrom( const system_id & p_id, gamete_t * g, unsigned char p = 0 ) {
        assert( !p0 );
        p0_id = p_id;
        p0 = g;
    }

    gamete_t * getGamete( unsigned char ) const {
        if( p0 ) {
            return p0;
        }
        return gamete_t::create();
    }

    void reset() {
        died();
        if(p0) {
        //    delete p0;
            p0->~gamete_t();
            p0 = NULL;
        }
    }

    virtual ~individual_props() {
        reset();
    }
protected:
    SystemClock::vtime_t       m_dob;
    system_id     p0_id;
    gamete_ptr    p0;
};

template < /*class VT,*/ class G >
class individual_props< /*VT,*/ G, 2 > {
public:
//    typedef VT  variant_t;
    typedef G   gamete_t;
    typedef typename gamete_t::pointer gamete_ptr;

    enum { GAMETE_COUNT = 2 };

    individual_props() :
        m_dob( SystemClock::POSITIVE_INFINITY ),
        p0_id(),
        p1_id(),
        p0( ), 
        p1( )
    {}

    individual_props( const individual_props< G, 2> & ip ) :
        m_dob( ip.m_dob ),
        p0_id(ip.p0_id),
        p1_id(ip.p1_id),
        p0(ip.p0),
        p1(ip.p1)
    {}

    void died() {
        setDOB( SystemClock::POSITIVE_INFINITY );
    }

    void setDOB( SystemClock::vtime_t d ) {
        if( m_dob != d ) {
            //if( d != SystemClock::POSITIVE_INFINITY ) {
            //    p0->increasePenetrance();
            //    p1->increasePenetrance();
            //} else {
            //    p0->decreasePenetrance();
            //    p1->decreasePenetrance();
            //}
            m_dob = d;
        }
    }

    bool isAlive() const {
        return (m_dob != SystemClock::POSITIVE_INFINITY);
    }

    bool hasSameGamates() const { return p0 == p1; }

    bool hasSourceGametes() const { return (p0 != NULL && p1 != NULL); }

    void inheritFrom( const system_id & p_id, gamete_ptr g, unsigned char gidx = UNKNOWN_GAMETE_INDEX) {
        if( gidx == UNKNOWN_GAMETE_INDEX) {
            if( !p0 ) {
                p0_id = p_id;
                p0 = g;
            } else if( !p1 ) {
                p1_id = p_id;
                p1 = g;
            } else {
                // both parents have already been set
                assert( false );
            }
        } else if( gidx == 0 ) {
            assert( !p0 );
            p0_id = p_id;
            p0 = g;
        } else if( gidx == 1 ) {
            assert( !p1 );
            p1_id = p_id;
            p1 = g;
        } else {
            std::cout << "Unexpected gamete index: " << (int) gidx << std::endl;
            assert(false);
        }
    }

    gamete_ptr getGamete( unsigned char gamete_idx ) const {
        if( gamete_idx == 0 && p0 != NULL ) {
            return p0;
        } else /*if( p1 != NULL )*/ {
            return p1;
        }
//        return new gamete_t();
    }

    void reset() {
        died();
        if( p0 ) {
//            delete p0;
            p0->~gamete_t();
            p0 = NULL;
        }

        if( p1 ) {
//            delete p1;
            p1->~gamete_t();
            p1 = NULL;
        }
    }

    virtual ~individual_props() {
        reset();
    }
protected:
    SystemClock::vtime_t         m_dob;
    system_id     p0_id, p1_id;
    gamete_ptr    p0, p1;
};

#endif  // INDIVIDUAL_PROPERTIES_HPP_
