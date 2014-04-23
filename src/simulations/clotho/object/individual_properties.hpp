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

    individual_props() :
        p0_id( 0 ),
        p0( NULL )
    {}

    void died() {
        setDOB( SystemClock::POSITIVE_INFINITY);
    }

    void setDOB( SystemClock::vtime_t d ) {
        m_dob = d;
    }
    
    bool isAlive() const { return m_dob != SystemClock::POSITIVE_INFINITY; }

    bool hasSourceGametes() const { return (p0 != NULL); }

    void inheritFrom( const system_id & p_id, gamete_t * g, unsigned char p = 0 ) {
        assert( !p0 );
        p0_id = p_id;
        p0 = g;
    }

    void reset() {
        died();
        if(p0) {
            delete p0;
            p0 = NULL;
        }
    }

    virtual ~individual_props() {
        reset();
    }
protected:
    SystemClock::vtime_t       m_dob;
    system_id     p0_id;
    gamete_t *    p0;
};

template < /*class VT,*/ class G >
class individual_props< /*VT,*/ G, 2 > {
public:
//    typedef VT  variant_t;
    typedef G   gamete_t;

    individual_props() :
        m_dob( SystemClock::POSITIVE_INFINITY ),
        p0_id(0),
        p1_id(0),
        p0( NULL ), 
        p1( NULL )
    {}

    void died() {
        setDOB( SystemClock::POSITIVE_INFINITY );
    }

    void setDOB( SystemClock::vtime_t d ) {
        m_dob = d;
    }

    bool isAlive() const {
        return (m_dob != SystemClock::POSITIVE_INFINITY);
    }

    bool hasSourceGametes() const { return (p0 != NULL && p1 != NULL); }

    void inheritFrom( const system_id & p_id, gamete_t * g, unsigned char gidx) {
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

    void reset() {
        died();
        if( p0 ) {
            delete p0;
            p0 = NULL;
        }

        if( p1 ) {
            delete p1;
            p1 = NULL;
        }
    }

    virtual ~individual_props() {
        reset();
    }
protected:
    SystemClock::vtime_t         m_dob;
    system_id     p0_id, p1_id;
    gamete_t      * p0, * p1;
};

#endif  // INDIVIDUAL_PROPERTIES_HPP_
