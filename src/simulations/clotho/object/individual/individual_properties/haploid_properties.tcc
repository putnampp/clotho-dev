#ifndef HAPLOID_PROPERTIES_TCC_
#define HAPLOID_PROPERTIES_TCC_

template < class G >
class individual_props< G, 1 > {
public:
//    typedef VT variant_t;
    typedef G gamete_t;
    typedef typename gamete_t::pointer gamete_ptr;

    enum { GAMETE_COUNT = 1 };

    individual_props() :
        p0_id( ),
        p0( NULL ) {
    }

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

    bool isAlive() const {
        return m_dob != SystemClock::POSITIVE_INFINITY;
    }

    bool hasSourceGametes() const {
        return (p0 != NULL);
    }

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

#endif  // HAPLOID_PROPERTIES_TCC_
