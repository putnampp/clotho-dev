#ifndef REPRODUCE_WITH_RECOMBINATION_HPP_
#define REPRODUCE_WITH_RECOMBINATION_HPP_

#include "rng/random_process.hpp"
//#include "genetics/recombine_bitset.hpp"
#include "genetics/recombine_methods.hpp"

#include "locus_generator.tcc"
#include "infinite_site_generator.tcc"

template < class Gamete, class Alphabet, class IndividualPointer >
class ReproduceWithRecombination : public RandomProcess {
public:
    typedef Gamete gamete_type;
    typedef Alphabet alphabet_type;

    typedef gamete_type * gamete_pointer;
    typedef IndividualPointer individual_pointer;

    typedef recombine_bitset< typename gamete_type::bitset_type::block_type, typename gamete_type::bitset_type::allocator_type, typename gamete_type::alphabet_t > recombination_method_type;
    typedef typename recombination_method_type::recombination_points recombination_points;
    typedef typename recombination_points::iterator recombination_iterator;

    ReproduceWithRecombination( double mu, double rho ) :
        m_mu(mu)
        , m_rho(rho)
        , m_nRecomb(0)
        , m_nRecombEvents(0)
        , m_nMut(0)
        , m_nMutEvents(0) {
    }

    size_t getRecombinationCalls() {
        return m_nRecomb;
    }
    size_t getRecombinationEvents() {
        return m_nRecombEvents;
    }

    size_t getMutationCalls() {
        return m_nMut;
    }
    size_t getMutationEvents() {
        return m_nMutEvents;
    }

    gamete_pointer operator()( individual_pointer ind, unsigned int gen = 0 ) {
        return operator()(ind->getProperties()->getGamete(0), ind->getProperties()->getGamete(1), gen);
    }

    gamete_pointer operator()( gamete_pointer base_gamete, gamete_pointer other_gamete, unsigned int gen = 0 ) {
        return method3( base_gamete, other_gamete, gen );
    }

    gamete_pointer method3( gamete_pointer base_gamete, gamete_pointer other_gamete, unsigned int gen ) {
        unsigned int nMut = m_rng->nextPoisson(m_mu);
        unsigned int nRec = 0;
        if( base_gamete != other_gamete ) {
            nRec = m_rng->nextPoisson( m_rho );
            if( m_rng->nextBool() ) std::swap( base_gamete, other_gamete);
        }

        if( nMut == 0 && nRec == 0) {
            // no recombination or mutation
            // therefore randomly copy one of them
            return base_gamete->copy();
        }

        gamete_pointer res = NULL;

        if( nRec ) {
            if( nMut ) {
                res = new gamete_type( base_gamete->getAlphabet() );
                typename recombination_method_type::result_type status;

                recombine( base_gamete, other_gamete, res->getBits(), nRec, status );
                mutate( res, nMut );
            } else {
                // recombination only
                typename gamete_type::bitset_type recombined_set;
                typename recombination_method_type::result_type status;

                recombine( base_gamete, other_gamete, &recombined_set, nRec, status );

                if( status.is_empty ) {
                    res = gamete_type::EMPTY.copy();
                } else if( status.match_base ) {
                    res = base_gamete->copy();
                } else if( status.match_alt ) {
                    res = other_gamete->copy();
                } else {
                    // this does not guarantee that the gamete will be unique in the population
                    // there is a rare(?) scenario under which this new gamete will be identical
                    // to another gamete in the population.
                    //
                    // Under the infinite site model, two identical child gametes can be produced
                    // when a pair of parents produce multiple children, but recombination events
                    // occur such at locations which result in the same output sequence.  This is
                    // rare because a) both parents must use the same "base" gamete for each
                    // child (.5^C), and b) all of the randomly generated recombination events 
                    // need to occur in the same regions relative to the parent's mutations.
                    res = new gamete_type( recombined_set, base_gamete->getAlphabet() );
                }
            }
        } else {
            // mutation only
            res = base_gamete->clone();
            mutate( res, nMut );
        }

        assert( res != NULL );
        return res;
    }

    gamete_pointer method2( gamete_pointer base_gamete, gamete_pointer other_gamete, unsigned int gen ) {
#ifdef LOGGING
        static unsigned int nCalls = 0;
        std::ostringstream oss;
        oss << gen <<  "." << nCalls++;
        std::string log_key = oss.str();
#endif
        unsigned int nMut = m_rng->nextPoisson( m_mu );

        unsigned int nRec = (( base_gamete == other_gamete) ? 0 : m_rng->nextPoisson(m_rho));

        if( base_gamete != other_gamete && m_rng->nextBool() ) {
            // have different gametes; therefore should randomly swap them
            std::swap( base_gamete, other_gamete );
        }

        if( nMut == 0 && nRec == 0) {
            // no recombination or mutation
            // therefore randomly copy one of them
            return base_gamete->copy();
        }

        gamete_pointer res = NULL;
        if( nRec > 0 ) {
#ifdef LOGGING
            global_log.put( log_key + ".recombine", true);
#endif
            res = recombine( base_gamete, other_gamete, nRec, gen );

            if( nMut ) {
#ifdef LOGGING
                global_log.put( log_key + ".mutate", true);
#endif
                mutate(res, nMut);
            }
        } else {
#ifdef LOGGING
            global_log.put( log_key + ".mutate", true);
#endif
            res = base_gamete->clone();
            mutate(res, nMut);
        }

#ifdef LOGGING
        oss.str("");
        oss.clear();
        oss << *res->getBits();
        global_log.put( log_key + ".new_gamete.sequence", oss.str());
        global_log.put( log_key + ".new_gamete.size", res->getBits()->size() );
        global_log.put( log_key + ".new_gamete.count", res->getBits()->count() );

        if( base_gamete->getBits()->count() > 0 && other_gamete->getBits()->count() > 0) {
            oss.str("");
            oss.clear();
            oss << *base_gamete->getBits();
            global_log.put( log_key + ".base_gamete.sequence", oss.str());
            global_log.put( log_key + ".base_gamete.size", base_gamete->getBits()->size() );
            global_log.put( log_key + ".base_gamete.count", base_gamete->getBits()->count() );

            oss.str("");
            oss.clear();
            oss << *other_gamete->getBits();
            global_log.put( log_key + ".other_gamete.sequence", oss.str());
            global_log.put( log_key + ".other_gamete.size", other_gamete->getBits()->size() );
            global_log.put( log_key + ".other_gamete.count", other_gamete->getBits()->count() );
        }
#endif

        return res;
    }

    void recombine( gamete_pointer base_gamete, gamete_pointer other_gamete, typename gamete_type::bitset_type * res, unsigned int nRec, typename recombination_method_type::result_type & result_status ) {
        ++m_nRecomb;
        m_nRecombEvents += nRec;

        typename gamete_type::alphabet_t::pointer  alpha = base_gamete->getAlphabet();

        recombination_points rec_points;
        generateRecombination( rec_points, nRec );

        recombination_method_type recomb( base_gamete->getBits(), res, alpha, &rec_points, &result_status );

        boost::to_block_range( *other_gamete->getBits(), recomb );
    }

    gamete_pointer recombine( gamete_pointer base_gamete, gamete_pointer other_gamete, unsigned int nRec, unsigned int gen = 0 ) {

        typename gamete_type::bitset_type symm_diff;
#ifdef LOGGING
        typename recombination_method_type::result_type status(gen);
#else
        typename recombination_method_type::result_type status;
#endif
        recombine( base_gamete, other_gamete, &symm_diff, nRec, status );

        gamete_pointer res = new gamete_type( symm_diff, base_gamete->getAlphabet() );
        return res;
    }

    void mutate( gamete_pointer res, unsigned int nMut ) {
        assert( res->copies() == 1 );
        assert( nMut );

        ++m_nMut;
        m_nMutEvents += nMut;

#ifdef LOGGING
        state_log_type m;
#endif
        while( nMut-- ) {
            typedef symbol_generator< typename alphabet_type::locus_t, typename alphabet_type::allele_t, typename alphabet_type::index_type, alphabet_type > sgen_type;
            typedef typename sgen_type::symbol_type symbol_type;

            static sgen_type sgen;
            symbol_type s = sgen( res->getAlphabet(), (infinite_site * ) NULL );
            //std::cout << "Adding variant: " << s << std::endl;
#ifdef LOGGING
            state_log_type _m;
            _m.put( "", s );

            m.push_back( std::make_pair("", _m));
#endif
            res->addVariant( s );
        }
    }

    gamete_pointer method1( gamete_pointer base_gamete, gamete_pointer other_gamete, unsigned int gen ) {
#ifdef LOGGING
        static unsigned int nCalls = 0;
        std::ostringstream oss;
        oss << gen <<  "." << nCalls++;
        std::string log_key = oss.str();
#endif

        unsigned int nMut = m_rng->nextPoisson( m_mu );
        unsigned int nRec = 0;
        gamete_pointer res = NULL;

        if( base_gamete != other_gamete ) {
            nRec = m_rng->nextPoisson( m_rho );

            if( m_rng->nextBool() ) {
                res = base_gamete;
                base_gamete = other_gamete;
                other_gamete = res;
                res = NULL;
            }
        } else if( nMut == 0 ) {
            // both gametes are the same
            // AND no new mutations will be added
            // hence just return a copy
#ifdef LOGGING
            global_log.put( log_key + ".result", "Same gamete; Copying base gamete");
#endif
            return base_gamete->copy();
//        } else {
//            // both gametes are the same
//            // recombination will result in no new gametes
//            // therefore skip recombinations.
//            // there will be mutations, so clone one of the gametes
//            assert( nMut > 0 );
//            assert( nRec == 0 );
        }

#ifdef LOGGING
        global_log.put( log_key + ".nRec", nRec );
        global_log.put( log_key + ".nMut", nMut );
#endif

        if( nRec > 0 ) {
            ++m_nRecomb;
            m_nRecombEvents += nRec;

            typename gamete_type::bitset_type symm_diff;
            typename alphabet_type::pointer  alpha = base_gamete->getAlphabet();

            recombination_points rec_points;
            generateRecombination( rec_points, nRec );

#ifdef LOGGING
            state_log_type p;
            for( unsigned int i = 0; i < rec_points.size(); ++i ) {
                state_log_type t;
                t.put( "", rec_points[i]);
                p.push_back( std::make_pair( "", t ) );
            }

            global_log.add_child( log_key + ".points", p );
#endif

#ifdef LOGGING
            typename recombination_method_type::result_type stats(gen);
#else
            typename recombination_method_type::result_type stats;
#endif

            recombination_method_type recomb( base_gamete->getBits(), &symm_diff, alpha, &rec_points, &stats );

            boost::to_block_range( *other_gamete->getBits(), recomb );

            if( nMut == 0) {
                if( stats.is_empty ) {
#ifdef LOGGING
                    global_log.put( log_key + ".result" , "EMPTY");
#endif
                    return gamete_type::EMPTY.copy();
                } else if( stats.match_base ) {

#ifdef LOGGING
                    global_log.put( log_key + ".result" , "Matched Base");
#endif
                    return base_gamete->copy();
                } else if( stats.match_alt ) {
#ifdef LOGGING
                    global_log.put( log_key + ".result" , "Matched Alternate");
#endif

                    return other_gamete->copy();
                } else {
#ifdef LOGGING
                    global_log.put( log_key + ".result" , "New gamete, no new mutations");
#endif
                    res = new gamete_type( symm_diff, alpha );
                    return res;
                }
            } else {
#ifdef LOGGING
                global_log.put( log_key + ".result", "New gamete, New mutations");
#endif
                res = new gamete_type( symm_diff, alpha );
            }
        } else if( nMut == 0 ) {
#ifdef LOGGING
            global_log.put( log_key + ".result", "Different Gametes; No Mutaions; Copy random gamete" );
#endif
            return base_gamete->copy();
        } else {
#ifdef LOGGING
            global_log.put( log_key + ".result", "Different Gamete; Clone random gamete");
#endif
            res = base_gamete->clone();
        }

        assert( res != NULL);
        assert( nMut > 0 );

        ++m_nMut;
        m_nMutEvents += nMut;

#ifdef LOGGING
        state_log_type m;
#endif
        while( nMut-- ) {
            typedef symbol_generator< typename alphabet_type::locus_t, typename alphabet_type::allele_t, typename alphabet_type::index_type, alphabet_type > sgen_type;
            typedef typename sgen_type::symbol_type symbol_type;

            static sgen_type sgen;
            symbol_type s = sgen( res->getAlphabet(), (infinite_site * ) NULL );
            //std::cout << "Adding variant: " << s << std::endl;
#ifdef LOGGING
            state_log_type _m;
            _m.put( "", s );

            m.push_back( std::make_pair("", _m));
#endif
            res->addVariant( s );
        }

#ifdef LOGGING
        global_log.add_child( log_key + ".mutations", m);
#endif
        return res;
    }

    virtual ~ReproduceWithRecombination() {}
protected:
/*
    void generateRecombination( recombination_points & rec_points, unsigned int nRec ) {
        static locus_generator< typename alphabet_type::locus_t, RandomProcess::rng_pointer> lgen;

        rec_points.reserve( nRec + 2 );

        rec_points.push_back( std::numeric_limits< typename alphabet_type::locus_t >::min() );
        rec_points.push_back( std::numeric_limits< typename alphabet_type::locus_t >::max() );

        std::generate_n( std::back_inserter( rec_points ), nRec, lgen );
        std::sort( rec_points.begin(), rec_points.end() );
        recombination_iterator it = std::unique( rec_points.begin(), rec_points.end() );
        rec_points.resize((it - rec_points.begin()));
    }
*/

    inline void generateRecombination( recombination_points & rec_points, unsigned int nRec, bool bSkipUnique = true ) {
        static locus_generator< typename alphabet_type::locus_t, RandomProcess::rng_pointer > lgen;

        while( nRec ) {
            std::generate_n( std::back_inserter( rec_points ), nRec, lgen );

            std::sort( rec_points.begin(), rec_points.end() );

            if( bSkipUnique ) break;
            recombination_iterator it = std::unique( rec_points.begin(), rec_points.end() );
            rec_points.resize(( it - rec_points.begin()));

            nRec -= rec_points.size();
        }
    }

    double m_mu, m_rho;
    size_t m_nRecomb, m_nRecombEvents;
    size_t m_nMut, m_nMutEvents;
};

#endif  // REPRODUCE_WITH_RECOMBINATION_HPP_
