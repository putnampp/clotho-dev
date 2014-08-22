/*******************************************************************************
 * Copyright (c) 2013, Patrick P. Putnam (putnampp@gmail.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 ******************************************************************************/

//#define LOGGING 1

#ifdef LOGGING
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
typedef boost::property_tree::ptree state_log_type;
state_log_type global_log;

#include "utility/lowest_bit.h"
#endif

#include "clotho.h"
#include "clotho_commandline.h"
#include <cstdlib>
#include <functional>
#include <algorithm>

#include <vector>
#include <memory>
#include <limits>

#include "utility/simulation_stats.h"

#include "object/individual.hpp"
#include "object/individual_properties.hpp"

#include "genetics/locus_bitset.h"

#include "rng/rng.hpp"
//#include "models/default_life_cycle.hpp"
#include "models/selection_models.hpp"
//#include "models/reproduction_models.hpp"

//#include "processes.hpp"
#include "processes/reproduction_models.hpp"

#include "genetics/recombine_bitset.hpp"
#include "genetics/fitness_bitset.hpp"

#define _PLOIDY 2

using std::shared_ptr;

namespace finalizer {

template < unsigned char P, class S >
class FinalWorker< individual_props< locus_bitset, P, S > > {
public:
    static void finalize( individual_props< locus_bitset, P, S > * props ) {
        for( unsigned char i = 0; i < P; ++i ) {
            if( props->m_gametes[i].second != NULL ) {
                props->m_gametes[i].second->release();
                props->m_gametes[i].second = NULL;
            }
        }
        props->m_free_gametes = P;
    }
};

}

template < >
struct locus_generator< double, RandomProcess::rng_pointer > {
    typedef double return_type;
    typedef RandomProcess::rng_pointer parameter_type;

    locus_generator() : m_params( RandomProcess::getRNG() ) {}
    locus_generator( parameter_type p ) : m_params( p ) {}

    return_type operator()() {
        return m_params->nextUniform();
    }
    parameter_type m_params;
};

struct infinite_site {};

//#define SYM_SPEC symbol_generator< PopulationAlphabet::locus_t, PopulationAlphabet::allele_t, PopulationAlphabet::index_type, PopulationAlphabet >
//#undef SYM_SPEC

typedef locus_bitset gamete_type;
typedef typename gamete_type::alphabet_t    alphabet_type;
typedef typename gamete_type::allele_type   allele_type;


#define SYM_SPEC symbol_generator< alphabet_type::locus_t, alphabet_type::allele_t, alphabet_type::index_type, alphabet_type >

template <> template <>
SYM_SPEC::symbol_type SYM_SPEC::operator()< infinite_site >( alphabet_pointer alpha, infinite_site * inf) {
    static locus_generator< alphabet_type::locus_t, RandomProcess::rng_pointer> lgen;
    static allele_generator< alphabet_type::allele_t, void > agen;

    SYM_SPEC::symbol_type res = alpha->getSymbol(lgen(), agen(), true );
    while( res == alphabet_type::npos ) {
        res = alpha->getSymbol(lgen(), agen(), true );
    }
    return res;
}
#undef SYM_SPEC

#include "locus_bitset_mutate.tcc"

typedef allele_type *   allele_pointer;
typedef gamete_type *   gamete_pointer;

//typedef typename gamete_type::adjacency_iterator       adjacency_iterator;

typedef reproduction::models::mutation::mutate_site< allele_type, alphabet_type, gamete_type >   mmodel_type;
typedef reproduction::models::recombination::no_recomb< _PLOIDY >     rcmodel_type;
typedef reproduction::IndividualReproduction< mmodel_type, rcmodel_type > rmodel_type;

typedef TIndividual< gamete_type, _PLOIDY, system_id, individual_props< gamete_type, _PLOIDY, system_id >> individual_type;
typedef individual_type  *                  individual_pointer;
typedef std::vector< individual_pointer >   environment_type;


class DiscreteSelector {
public:
    DiscreteSelector( gsl_rng * r, double * fitnesses, size_t s ) :
        m_rng( r ),
        m_lookup( NULL ) {
        m_lookup = gsl_ran_discrete_preproc( s, fitnesses );
    }

    size_t operator()() {
        return gsl_ran_discrete( m_rng, m_lookup );
    }

    std::pair< individual_pointer , individual_pointer > operator()( environment_type * env, double f = 0.0 ) {
        size_t i0 = gsl_ran_discrete( m_rng, m_lookup );
        size_t i1 = ((gsl_rng_uniform(m_rng) <= f ) ? i0 : gsl_ran_discrete( m_rng, m_lookup ));

        std::pair< individual_pointer, individual_pointer > res = make_pair( env->at(i0), env->at(i1));

        return res;
    }

    virtual ~DiscreteSelector() {
        gsl_ran_discrete_free( m_lookup );
    }
protected:
    gsl_rng * m_rng;
    gsl_ran_discrete_t * m_lookup;
};

class ReproduceWithRecombination : public RandomProcess {
public:
        typedef recombine_bitset< typename gamete_type::bitset_type::block_type, typename gamete_type::bitset_type::allocator_type, typename gamete_type::alphabet_t > recombination_method_type;
    typedef typename recombination_method_type::recombination_points recombination_points;
    typedef typename recombination_points::iterator recombination_iterator;

    ReproduceWithRecombination( double mu, double rho ) : 
        m_mu(mu)
        , m_rho(rho)
        , m_nRecomb(0)
        , m_nRecombEvents(0)
        , m_nMut(0)
        , m_nMutEvents(0) 
    {}

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
        //unsigned int nRec = m_rng->nextPoisson(m_rho);

        //if( m_rng->nextBool() ) {
        //    // have different gametes; therefore should randomly swap them
        //    std::swap( base_gamete, other_gamete );
        //}

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
                recombination_method_type::result_type status;

                recombine( base_gamete, other_gamete, res->getBits(), nRec, status );
                mutate( res, nMut );
            } else {
                // recombination only
                gamete_type::bitset_type recombined_set;
                recombination_method_type::result_type status;

                recombine( base_gamete, other_gamete, &recombined_set, nRec, status );

                if( status.is_empty ) {
                    res = gamete_type::EMPTY.copy();
                } else if( status.match_base ) {
                    res = base_gamete->copy();
                } else if( status.match_alt ) {
                    res = other_gamete->copy();
                } else {
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

    void recombine( gamete_pointer base_gamete, gamete_pointer other_gamete, gamete_type::bitset_type * res, unsigned int nRec, recombination_method_type::result_type & result_status ) {
        ++m_nRecomb;
        m_nRecombEvents += nRec;

        gamete_type::alphabet_t::pointer  alpha = base_gamete->getAlphabet();

        recombination_points rec_points;
        generateRecombination( rec_points, nRec );

        recombination_method_type recomb( base_gamete->getBits(), res, alpha, &rec_points, &result_status );

        boost::to_block_range( *other_gamete->getBits(), recomb );
    }

    gamete_pointer recombine( gamete_pointer base_gamete, gamete_pointer other_gamete, unsigned int nRec, unsigned int gen = 0 ) {

        gamete_type::bitset_type symm_diff;
#ifdef LOGGING
            recombination_method_type::result_type status(gen);
#else
            recombination_method_type::result_type status;
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
            typedef symbol_generator< alphabet_type::locus_t, alphabet_type::allele_t, alphabet_type::index_type, alphabet_type > sgen_type;
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

            gamete_type::bitset_type symm_diff;
            gamete_type::alphabet_t::pointer  alpha = base_gamete->getAlphabet();

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
            recombination_method_type::result_type stats(gen);
#else
            recombination_method_type::result_type stats;
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
            typedef symbol_generator< alphabet_type::locus_t, alphabet_type::allele_t, alphabet_type::index_type, alphabet_type > sgen_type;
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
    void generateRecombination( recombination_points & rec_points, unsigned int nRec ) {
        static locus_generator< alphabet_type::locus_t, RandomProcess::rng_pointer> lgen;

        rec_points.reserve( nRec + 2 );

        rec_points.push_back( std::numeric_limits< typename alphabet_type::locus_t >::min() );
        rec_points.push_back( std::numeric_limits< typename alphabet_type::locus_t >::max() );

        std::generate_n( std::back_inserter( rec_points ), nRec, lgen );
        std::sort( rec_points.begin(), rec_points.end() );
        recombination_iterator it = std::unique( rec_points.begin(), rec_points.end() );
        rec_points.resize((it - rec_points.begin()));
    }

    double m_mu, m_rho;
    size_t m_nRecomb, m_nRecombEvents;
    size_t m_nMut, m_nMutEvents;
};

class SimpleSelector : public RandomProcess {
public:
    static std::pair< individual_pointer , individual_pointer > select( environment_type * env ) {
        std::pair< individual_pointer, individual_pointer > res;

        size_t nInd = env->size();

        size_t i0 = m_rng->nextInt( nInd );
        size_t i1 = i0;

        while( i1 == i0 ) {
            i1 = m_rng->nextInt( nInd );
        }

        res.first = env->at( i0 );
        res.second = env->at( i1 );

        return res;
    }
};

typedef SimpleSelector selector_t;

struct het_fitness {
    inline void operator()( double & f, allele_pointer v ) {
        f *= (1. + v->dominance * v->selection);
    }

    inline void operator()( double & f, const allele_type & v ) {
//        std::cout << v.dominance << std::endl;
//        std::cout << v.selection << std::endl;
        f *= (1. + v.dominance * v.selection);
    }
};

struct hom_fitness {
    hom_fitness( double s = 1. ) : m_scaling(1.) {}
    inline void operator()( double & f, allele_pointer v ) {
        f *= (1. + v->selection * m_scaling);
    }

    inline void operator()( double & f, const allele_type & v ) {
        f *= (1. + v.selection * m_scaling);
    }

    double m_scaling;
};

template < class het_policy, class hom_policy >
class fitness_multiplicative {
public:
//    typedef void (*update_policy)( double & f, allele_pointer v );
//
    fitness_multiplicative() {}

    fitness_multiplicative( het_policy & het, hom_policy & hom ) :
        m_het_case(het),
        m_hom_case(hom) {
    }

    double operator()( double f, individual_pointer ind ) {
        return (*this)(f, ind->getProperties()->getGamete(0), ind->getProperties()->getGamete(1) );
    }

    double operator()( double f, gamete_pointer g1, gamete_pointer g2 ) {
        if( g1 == g2 ) return f;

        gamete_type::alphabet_t::pointer alpha = g1->getAlphabet();

        fitness_bitset< typename gamete_type::bitset_type::block_type, typename gamete_type::bitset_type::allocator_type, typename gamete_type::alphabet_t, hom_policy, het_policy, double > fit( g1->getBits(),alpha,m_hom_case, m_het_case, f);

        boost::to_block_range( *g2->getBits(), fit );

        return fit.getResult();
    }
    virtual ~fitness_multiplicative() {}
protected:
    het_policy m_het_case;
    hom_policy m_hom_case;

};

int main( int argc, char ** argv ) {

    po::variables_map vm;
    if( !parse_commandline( argc, argv, vm ) ) {
        return 0;
    }

    string engine_config( "" );
    string clotho_config( "" );

    SystemClock::vtime_t tUntil = vm[ SIM_UNTIL_K ].as< SystemClock::vtime_t >();
    unsigned int nGen = vm[ GENERATIONS_K ].as< unsigned int >();

    if( nGen != (unsigned int) -1 ) {
        tUntil =  nGen;
    }

    cout << "Simulate until: " << tUntil << endl;

    gsl_rng_env_setup();
    const gsl_rng_type * T = gsl_rng_default;
    string m_type = T->name;
    unsigned int m_seed = gsl_rng_default_seed;

    gsl_rng * my_rng = gsl_rng_alloc( T );
    gsl_rng_set( my_rng, m_seed );

    shared_ptr< iRNG > rng( new GSL_RNG( my_rng, m_type, m_seed ));
    cout << "RNG: " <<  rng->getType() << "; seed: " << rng->getSeed() << endl;

    RandomProcess::initialize( rng );

    double mu = vm[ MUTATION_RATE_K ].as<double>();
    double rho = vm[ RECOMBINATION_RATE_K ].as< double >();

    mmodel_type::initialize( mu, false);

    shared_ptr< SimulationStats > stats( new SimulationStats() );

    stats->startPhase( RUNTIME_K );

    environment_type population;
    environment_type buffer;

    system_id blank_id;

    stats->startPhase( "PopInit" );

#ifdef LOGGING
    {
        lowest_bit_256 lbit;
        for( unsigned int i = 0; i < 256; ++i ) {
            const lowest_bit_256::value_type v = lbit[i];

            state_log_type nnode, inode, snode, node;
            inode.put("", v.bit_index);
            snode.put("", v.bit_shift_next);
            node.push_back( std::make_pair("", inode));
            node.push_back( std::make_pair("", snode));

            std::ostringstream oss;
            oss << "lowest_bit." << i;
            global_log.add_child( oss.str(), node);
        }
    }
#endif

    fitness_multiplicative< het_fitness, hom_fitness > fmult;

    for( unsigned int i = 0; i < vm[ FOUNDER_SIZE_K ].as< unsigned int >(); ++i) {
        population.push_back( new individual_type() );

        population.back()->getProperties()->inheritFrom( blank_id,  gamete_type::EMPTY.copy() );
        population.back()->getProperties()->inheritFrom( blank_id,  gamete_type::EMPTY.copy() );

        buffer.push_back( new individual_type() );
    }

    std::cout << gamete_type::EMPTY.copies() << std::endl;
    assert( gamete_type::EMPTY.copies() - 1 == 2 * vm[ FOUNDER_SIZE_K ].as<unsigned int>() );

    stats->stopPhase( "PopInit" );

    environment_type * parent = &population, * child = &buffer;

    unsigned int fitness_size = 0;
    double * fitness = NULL;

    ReproduceWithRecombination repro( mu, rho );

    size_t nSelfing = 0;
    stats->startPhase( "Sim" );
    for( SystemClock::vtime_t i = 0; i < tUntil; ++i ) {
        assert( parent != child );
        if( fitness_size < parent->size() ) {
            if( fitness != NULL ) {
                delete [] fitness;
            }
            fitness_size = parent->size();
            fitness = new double[ fitness_size ];
        }

        memset( fitness, 0, sizeof(double) * fitness_size );

//        std::cerr << "Generation: " << i << std::endl;

        locus_bitset::updateActiveAlphabet();

#ifdef LOGGING
        {
            std::ostringstream oss;
//            oss << i << ".free_list";
            oss << i;
            std::string k = oss.str();

//            oss.str("");
//            oss.clear();
            //oss <<  *AlleleAlphabet::getInstance()->getFreeMask();
//            oss << *alphabet_type::getInstance()->getFreeMask();
//            global_log.put( k + ".size", AlleleAlphabet::getInstance()->getFreeMask()->size());
//            global_log.put( k + ".count", AlleleAlphabet::getInstance()->getFreeMask()->count());
//            global_log.put( k + ".sequence", oss.str());
//            global_log.put( k + ".total_mutations", repro.getMutationEvents());

            state_log_type tmp;
            alphabet_type::getInstance()->logState( tmp );

            global_log.add_child( oss.str(), tmp );
        }
#endif

        // measure fitness of parent population
        //
        double * tmp = fitness;
//        double e_fitness = 0.0;
        for( environment_type::iterator it = parent->begin(); it != parent->end(); it++ ) {
            (*tmp) = 1.0;
            (*tmp) = fmult( (*tmp), (*it) );
            //e_fitness += (*tmp++);
            ++tmp;
        }
        //e_fitness /= (double)parent->size();

        //
        // mate
        //
        DiscreteSelector ds( my_rng, fitness, parent->size() );
        unsigned int child_idx = 0;
        while( child_idx < child->size()) {
            std::pair< individual_pointer, individual_pointer > mate_pair = ds( parent );

            nSelfing += (( mate_pair.first == mate_pair.second ) ? 1 : 0);
            gamete_pointer g = repro( mate_pair.first, i );
            //assert( gamete_type::isGamete( g ) );
            (*child)[child_idx]->getProperties()->inheritFrom(blank_id, g);

            gamete_pointer g1 = repro( mate_pair.second, i );
            (*child)[child_idx]->getProperties()->inheritFrom(blank_id, g1);

            (*child)[child_idx++]->getProperties()->setDOB( i );
        }

        assert( parent->size() == vm[ FOUNDER_SIZE_K ].as< unsigned int > () );
        for( environment_type::iterator it = parent->begin(); it != parent->end(); it++ ) {
            (*it)->reset();
        }

        std::swap( parent, child );
    }

    locus_bitset::updateActiveAlphabet();
    stats->stopPhase( "Sim" );
    stats->startPhase( "Final" );

    unsigned int nUniqueInd = 0;
    for( unsigned int i = 1; i < parent->size(); ++i ) {
        bool no_dup = true;
        for( unsigned int j = 0; no_dup && j < i; ++j ) {
            no_dup = !((*parent)[i]->getProperties()->getGamete(0) == (*parent)[j]->getProperties()->getGamete(0) && (*parent)[i]->getProperties()->getGamete(1) == (*parent)[j]->getProperties()->getGamete(1))
                    && !((*parent)[i]->getProperties()->getGamete(0) == (*parent)[j]->getProperties()->getGamete(1) && (*parent)[i]->getProperties()->getGamete(1) == (*parent)[j]->getProperties()->getGamete(0));
 
        }
        if( no_dup ) {
            ++nUniqueInd;
        }
    }

    double nSymbols = 0;
    double nGametes = 0;
    size_t nMaxSymbols = 0, nMinSymbols = -1;
    double nBlocks = 0;
    unsigned int n = 0;
    for( typename locus_bitset::active_iterator it = locus_bitset::active_begin(); it != locus_bitset::active_end(); it++ ) {
        size_t s = (*it)->size();
        nBlocks += (double)(*it)->block_count();
        nSymbols += (double)s;
        nGametes += 1.0;

        if( nMaxSymbols < s ) {
            nMaxSymbols = s;
        }

        if( nMinSymbols > s ) {
            nMinSymbols = s;
        }

        n += (*it)->copies();
    }

    n += (gamete_type::EMPTY.copies() - 1);
    std::cout << "Final population has " << nGametes  << " (" << n << ") gametes" << std::endl;
    std::cout << "Final population has " << nUniqueInd << " unique individuals" << std::endl;

    std::cout << "Average number of blocks per Gamete: " << nBlocks/nGametes << std::endl;

    std::cout << "Max Mutations single Gamete: " << nMaxSymbols << std::endl;
    std::cout << "Min Mutations single Gamete: " << nMinSymbols << std::endl;
    std::cout << "Average Mutations per Gamete: " << nSymbols/nGametes << std::endl;

    std::cout << "Population Mutation count: " << alphabet_type::getInstance()->active_count() << " (" << mmodel_type::getVariantMap()->size() << ")" << std::endl;

    std::cout << "Simulation Recombinations: " << repro.getRecombinationCalls() << std::endl;
    std::cout << "Simulation Recombination Events: " << repro.getRecombinationEvents() << std::endl;

    std::cout << "Simulation Mutation Calls: " << repro.getMutationCalls() << std::endl;
    std::cout << "Simulation Mutation Events: " << repro.getMutationEvents() << std::endl;

    std::cout << "Selfing count: " << nSelfing << std::endl;

    std::cout << "Lost or Fixed from previous generation: " << alphabet_type::getInstance()->fixed_lost_count() << std::endl;

#ifdef LOGGING

    // testing whether reproduction works
    typename locus_bitset::active_iterator it = locus_bitset::active_begin();
    unsigned int e = 10;
    while( it != locus_bitset::active_end() ) {
        if( e-- == 0 ) { break; }
        gamete_pointer p0 = NULL, p1 = NULL;
        for( ; it != locus_bitset::active_end(); it++ ) {
            if( (*it)->getBits()->count() >= 1 ) {
                if( p0 == NULL ) {
                    p0 = *it;
                } else {
                    p1 = *it++;
                    break;
                }
            }
        }

        if( p0 != NULL && p1 != NULL ) {
            std::cout << "Test reproduction" << std::endl;
            gamete_pointer res = repro.recombine(p0, p1, 1, tUntil + 1);
        }
    }

    boost::property_tree::write_json( std::cerr, global_log ); 
#endif

    while( !population.empty() ) {
        individual_type * ind = population.back();
        population.pop_back();

        delete ind;
    }

    while( !buffer.empty() ) {
        individual_type * ind = buffer.back();
        buffer.pop_back();
        delete ind;
    }
    stats->stopPhase( "Final" );

    stats->stopPhase( RUNTIME_K );

    cout << *stats;

    delete [] fitness;

    return 0;
}
