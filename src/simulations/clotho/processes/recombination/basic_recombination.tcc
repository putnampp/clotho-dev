#ifndef BASIC_RECOMBINATION_TCC_
#define BASIC_RECOMBINATION_TCC_

namespace reproduction {
namespace models {
namespace recombination {

template < class IP, class GM = typename IP::gamete_t >
class basic_recomb : public RandomProcess {
public:
    typedef IP individual_properties_t;
    typedef GM gamete_t;
    typedef typename gamete_t::pointer gamete_ptr;

    basic_recomb( double g = 0.0001 ) : m_gamma(((g < 0.0) ? 0.0 : g )) { }

    gamete_ptr operator()( individual_properties_t * ip ) {
        gamete_ptr p0 = ip->getGamete(0);
        gamete_ptr p1 = ip->getGamete(1);

        if( p0 == p1 ) return p0; // gametes the same, recombination has no effect

        size_t nVar = p0->size() + p1->size();
        if( nVar == 0 || nVar == 1 ) return p0;

        unsigned int nB = m_rng->nextPoisson( m_gamma );

        if( nB ) {
            gamete_ptr res = gamete_t::create();

            std::vector< double > pos;

            for( unsigned int i = 0; i < nB; ++i ) {
                pos.push_back( m_rng->nextUniform() );
            }

            std::sort( pos.begin(), pos.end() );

            bool sw = m_rng->nextBool();

            typename gamete_t::var_iterator p0_it = p0->begin();
            typename gamete_t::var_iterator p0_end = p0->end();
            typename gamete_t::var_iterator p1_it = p1->begin();
            typename gamete_t::var_iterator p1_end = p1->end();

            for( unsigned int i = 0; i < nB; ) {
                double b = pos[i++];
                if( sw ) {
                    while( p0_it != p0_end && p0_it->getKey() < b ) {
                        res->addVariant( *p0_it++ );
                    }

                    while( p1_it != p1_end && p1_it->getKey() < b ) {
                        p1_it++;
                    }
                } else {
                    while( p1_it != p1_end && p1_it->getKey() < b) {
                        res->addVariant( *p1_it++ );
                    }

                    while( p0_it != p0_end && p0_it->getKey() < b ) {
                        p0_it++;
                    }
                }
                sw = !sw;
            }

            if( sw ) {
                while( p0_it != p0_end ) {
                    res->addVariant( *p0_it++ );
                }
            } else {
                while( p1_it != p1_end ) {
                    res->addVariant( *p1_it++ );
                }
            }

            return res;
        }

        return ((m_rng->nextBool() ) ? p0 : p1);
    }

    virtual ~basic_recomb() {}
protected:
    double m_gamma;
};

}   // namespace recombination
}   // namespace models
}   // namespace reproduction
#endif  // BASIC_RECOMBINATION_TCC_
