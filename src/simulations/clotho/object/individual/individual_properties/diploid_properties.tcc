#ifndef DIPLOID_PROPERTIES_TCC_
#define DIPLOID_PROPERTIES_TCC_

template < class G, class S >
class individual_props< G, 2, S > {
public:
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
        //std::cout << "Resetting individual properties" << std::endl;
        if( p0 ) {
//            delete p0;
       //     std::cout << "Deconstructing P0: " << p0 << std::endl;
            p0->~gamete_t();
            p0 = NULL;
        }

        if( p1 ) {
//            delete p1;
       //     std::cout << "Deconstructing P1: " << p1 << std::endl;
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

#endif  // DIPLOID_PROPERTIES_TCC_
