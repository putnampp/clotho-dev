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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
typedef boost::property_tree::ptree state_log_type;
#ifdef LOGGING
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
#include <cmath>

#include "timing.hpp"

#include "utility/simulation_stats.h"

#include "object/individual.hpp"
#include "object/individual_properties.hpp"

#include "genetics/locus_bitset.h"

#include "rng/rng.hpp"
#include "models/selection_models.hpp"

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

void logMutationFrequencies( boost::property_tree::ptree & p, std::vector< size_t > & frequencies, std::map< size_t, size_t > & freq_dist ) {
    boost::property_tree::ptree v, _v0, _v1;
    _v0.put("", "Allele");
    _v1.put("", "Frequency");
    v.push_back( std::make_pair("", _v0));
    v.push_back( std::make_pair("", _v1));
    p.add_child("frequency.y.smps", v);

    boost::property_tree::ptree x, _x0, _x1;
    _x0.put("", "Region relative position of Allele" );
    _x1.put("", "Frequency of allele in population" );
    x.push_back( std::make_pair("", _x0 ) );
    x.push_back( std::make_pair("", _x1 ) );
    p.add_child( "frequency.x.Description", x);

    boost::property_tree::ptree d,s;

    alphabet_type::active_iterator alpha_it = alphabet_type::getInstance()->active_begin();
    for( std::vector< size_t >::iterator it = frequencies.begin(); it != frequencies.end(); ++it ) {
        if( *it > 0 ) {
            assert( alphabet_type::getInstance()->checkFreeStatus( it - frequencies.begin()) );
            boost::property_tree::ptree x,y,z, _s;
            x.put( "", alpha_it->first);
            y.put( "", (*it));

            z.push_back( std::make_pair("", x ));
            z.push_back( std::make_pair("", y ));

            std::ostringstream oss;
            oss << (it - frequencies.begin() );

            _s.put("", oss.str());
            s.push_back( std::make_pair("", _s));
            d.push_back( std::make_pair("", z ));

            std::map< size_t, size_t >::iterator it2 = freq_dist.find((*it));
            if( it2 == freq_dist.end() ) {
                freq_dist.insert( std::make_pair( (*it), 1 ) );
            } else {
                ++it2->second;
            }
        }
        ++alpha_it;
    }

    p.add_child( "frequency.y.vars", s );
    p.add_child( "frequency.y.data", d );

    boost::property_tree::ptree graph_opts;
    graph_opts.put("graphType", "Scatter2D");
    {
        boost::property_tree::ptree tmp, t;
        t.put("", "Allele");
        tmp.push_back( std::make_pair("", t ) );
        graph_opts.add_child("xAxis", tmp);
    }
    {
        boost::property_tree::ptree tmp, t;
        t.put("", "Frequency");
        tmp.push_back( std::make_pair( "",t));
        graph_opts.add_child("yAxis", tmp);
    }
    graph_opts.put( "title", "Allele Frequency" );
    p.add_child( "frequency.graph_opts", graph_opts );
}

void logMutationBinFrequencies( boost::property_tree::ptree & p, std::vector< size_t > & frequencies, unsigned int bin_count ) {
    boost::property_tree::ptree v, _v0, _v1, _v2;
    string key = "genome_bin";
    _v0.put("", "Bin");
    _v1.put("", "Offset");
    _v2.put("", "Frequency");
    v.push_back( std::make_pair("", _v0));
    v.push_back( std::make_pair("", _v1));
    v.push_back( std::make_pair("", _v2));
    p.add_child( key +".y.smps", v);

    boost::property_tree::ptree x, _x0, _x1, _x2;
    _x0.put("", "Bin index" );
    _x1.put("", "Genomic offset relative to bin" );
    _x2.put("", "Frequency of allele in population" );
    x.push_back( std::make_pair("", _x0 ) );
    x.push_back( std::make_pair("", _x1 ) );
    x.push_back( std::make_pair("", _x2 ) );

    p.add_child( key + ".x.Description", x);

    boost::property_tree::ptree d,s;

    alphabet_type::active_iterator alpha_it = alphabet_type::getInstance()->active_begin();

    typedef  std::vector< std::map< double, size_t > > bin_freq_type;
    bin_freq_type bin_freq( bin_count, std::map<double, size_t>() );

    for( std::vector< size_t >::iterator it = frequencies.begin(); it != frequencies.end(); ++it ) {
        if( *it > 0 ) {
            assert( alphabet_type::getInstance()->checkFreeStatus( it - frequencies.begin()) );
            unsigned int bin_idx = alpha_it->first * bin_count;

            double lo = (double)(bin_idx) / (double) bin_count;

            double offset = alpha_it->first - lo;

            bin_freq[bin_idx].insert( std::make_pair( offset, (*it)));
        }
        ++alpha_it;
    }

    unsigned int bin_idx = 0;
    for( bin_freq_type::iterator it = bin_freq.begin(); it != bin_freq.end(); ++it ) {
        double lo = (double)(bin_idx) / (double) bin_count;
        boost::property_tree::ptree w;
        w.put( "", lo);
        unsigned int offset = 0;
        for( std::map< double, size_t >::iterator it2 = it->begin(); it2 != it->end(); ++it2 ) {
            boost::property_tree::ptree x,y,z, _s;
            x.put( "", it2->first);
            y.put( "", it2->second);
            z.push_back( std::make_pair("", w ));
            z.push_back( std::make_pair("", x ));
            z.push_back( std::make_pair("", y ));

            d.push_back( std::make_pair("", z ));

            std::ostringstream oss;
            oss << bin_idx << "_" << offset++;
            _s.put("", oss.str());
            s.push_back( std::make_pair("", _s));
        }
        ++bin_idx;
    }

    p.add_child( key + ".y.vars", s );
    p.add_child( key + ".y.data", d );

    boost::property_tree::ptree graph_opts;

    graph_opts.put("graphType", "Scatter3D");

    {
        boost::property_tree::ptree tmp, t;
        t.put("", "Bin");
        tmp.push_back( std::make_pair("", t ) );
        graph_opts.add_child("xAxis", tmp);
    }
    {
        boost::property_tree::ptree tmp, t;
        t.put("", "Offset");
        tmp.push_back( std::make_pair( "",t));
        graph_opts.add_child("zAxis", tmp);
    }
    {
        boost::property_tree::ptree tmp, t;
        t.put("", "Frequency");
        tmp.push_back( std::make_pair("", t ));
        graph_opts.add_child("yAxis", tmp);
    }
    graph_opts.put( "title", "Mutation Distribution grouped by bin" );

    p.add_child( key + ".graph_opts", graph_opts );
}

void logMutationDistribution( boost::property_tree::ptree & p, std::map< size_t, size_t > & freq_dist ) {
    boost::property_tree::ptree v, _v0, _v1;
    _v0.put("", "Frequency");
    _v1.put("", "Count");
    v.push_back( std::make_pair("", _v0));
    v.push_back( std::make_pair("", _v1));
    p.add_child("distribution.y.smps", v);

    boost::property_tree::ptree x, _x0, _x1;
    _x0.put("", "Region relative position of Allele" );
    _x1.put("", "Frequency of allele in population" );
    x.push_back( std::make_pair("", _x0 ) );
    x.push_back( std::make_pair("", _x1 ) );
    p.add_child( "distribution.x.Description", x);
    boost::property_tree::ptree d,s;

    unsigned int i = 0;
    for( std::map< size_t, size_t >::iterator it = freq_dist.begin(); it != freq_dist.end(); ++it ) {
        boost::property_tree::ptree x,y,z, _s;
        x.put( "", it->first);
        y.put( "", it->second);

        z.push_back( std::make_pair("", x ) );
        z.push_back( std::make_pair("", y ) );

        std::ostringstream oss;
        oss << i++;

        _s.put("", oss.str());
        s.push_back( std::make_pair("", _s));
        d.push_back( std::make_pair("", z ));
    }

    p.add_child( "distribution.y.vars", s );
    p.add_child( "distribution.y.data", d );

    boost::property_tree::ptree graph_opts;
    graph_opts.put("graphType", "Scatter2D");
    {
        boost::property_tree::ptree tmp, t;
        t.put("", "Frequency");
        tmp.push_back( std::make_pair("", t ) );
        graph_opts.add_child("xAxis", tmp);
    }
    {
        boost::property_tree::ptree tmp, t;
        t.put("", "Count");
        tmp.push_back( std::make_pair( "",t));
        graph_opts.add_child("yAxis", tmp);
    }
    graph_opts.put( "title", "Allele Frequency Distribution" );
    p.add_child( "distribution.graph_opts", graph_opts );
}

void logMutationStats( boost::property_tree::ptree & p, std::vector< size_t > & frequencies ) {
    std::map< size_t, size_t > freq_dist;
    logMutationFrequencies( p, frequencies, freq_dist );
    logMutationBinFrequencies( p, frequencies, 1000 );
    logMutationDistribution( p, freq_dist );
}

int main( int argc, char ** argv ) {

    po::variables_map vm;
    if( !parse_commandline( argc, argv, vm ) ) {
        return 0;
    }

    timer runtime_timer;
    state_log_type  log;

    SystemClock::vtime_t tUntil = vm[ SIM_UNTIL_K ].as< SystemClock::vtime_t >();
    unsigned int nGen = vm[ GENERATIONS_K ].as< unsigned int >();

    if( nGen != (unsigned int) -1 ) {
        tUntil =  nGen;
    }

    gsl_rng_env_setup();
    const gsl_rng_type * T = gsl_rng_default;
    string m_type = T->name;
    unsigned int m_seed = gsl_rng_default_seed;

    gsl_rng * my_rng = gsl_rng_alloc( T );
    gsl_rng_set( my_rng, m_seed );

    shared_ptr< iRNG > rng( new GSL_RNG( my_rng, m_type, m_seed ));
    RandomProcess::initialize( rng );

    double mu = vm[ MUTATION_RATE_K ].as<double>();
    double rho = vm[ RECOMBINATION_RATE_K ].as< double >();

    log.put( "configuration.random_number.generator", rng->getType() );
    log.put( "configuration.random_number.seed", rng->getSeed() );

    log.put( "configuration.generations", tUntil );
    log.put( "configuration.population.size", vm[ FOUNDER_SIZE_K ].as< unsigned int >() );
    log.put( "configuration.mutation.model", "infinite site");
    log.put( "configuration.mutation.rate_per_region", mu );

    log.put( "configuration.recombination.model", "uniform random" );
    log.put( "configuration.recombination.rate_per_region", rho );

    log.put( "configuration.region.mutation_rate_per_base", pow( 10.0, -8.0) );
    log.put( "configuration.region.base_per_region", mu / pow( 10.0, -8.0) );
    log.put( "configuration.region.regions_per_individual",  _PLOIDY );

    mmodel_type::initialize( mu, false);


    environment_type population;
    environment_type buffer;

    system_id blank_id;

    fitness_multiplicative< het_fitness, hom_fitness > fmult;

    timer init_time;
    for( unsigned int i = 0; i < vm[ FOUNDER_SIZE_K ].as< unsigned int >(); ++i) {
        population.push_back( new individual_type() );

        population.back()->getProperties()->inheritFrom( blank_id,  gamete_type::EMPTY.copy() );
        population.back()->getProperties()->inheritFrom( blank_id,  gamete_type::EMPTY.copy() );

        buffer.push_back( new individual_type() );
    }

    std::cerr << gamete_type::EMPTY.copies() << std::endl;
    assert( gamete_type::EMPTY.copies() - 1 == 2 * vm[ FOUNDER_SIZE_K ].as<unsigned int>() );

    init_time.stop();

    environment_type * parent = &population, * child = &buffer;

    unsigned int fitness_size = 0;
    double * fitness = NULL;

    ReproduceWithRecombination repro( mu, rho );

    size_t nSelfing = 0;
    timer sim_time;
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
            oss << i;
            std::string k = oss.str();

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
    sim_time.stop();

    timer finalize_timer;
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
    size_t nMaxSymbols = 0, nMinSymbols = -1;
    double nBlocks = 0;
    unsigned int n = 0;

    typedef std::vector< std::pair< locus_bitset::active_iterator, size_t > > count_vector_type;
    count_vector_type g_counts;

    std::vector< size_t > frequencies( alphabet_type::getInstance()->database_size(), 0);

    for( typename locus_bitset::active_iterator it = locus_bitset::active_begin(); it != locus_bitset::active_end(); it++ ) {
        (*it)->updateFrequencies( frequencies );
        size_t s = (*it)->size();
        nBlocks += (double)(*it)->block_count();
        nSymbols += (double)s;

        if( nMaxSymbols < s ) {
            nMaxSymbols = s;
        }

        if( nMinSymbols > s ) {
            nMinSymbols = s;
        }

        n += (*it)->copies();

        bool isUnique = true;
        for( count_vector_type::iterator it2 = g_counts.begin(); it2 != g_counts.end(); it2++ ) {
            if( (*it) == *(it2->first) ) {
                it2->second += (*it)->copies();
                isUnique = false;
                break;
            }
        }

        if( isUnique ) {
            g_counts.push_back(std::make_pair( it, (*it)->copies() ));
        }
    }

    double nGametes = (double)g_counts.size();

    boost::property_tree::ptree mut_stats;
    logMutationStats( mut_stats, frequencies );

    n += (gamete_type::EMPTY.copies() - 1);
    alphabet_type::getInstance()->logState( log );

    log.put( "population.regions.count", n );
    log.put( "population.regions.unique", nGametes );
    log.put( "population.regions.active", locus_bitset::activeCount() );
    log.put( "population.regions.reference", gamete_type::EMPTY.copies() - 1);

    log.put( "population.individual.count", parent->size() );
    log.put( "population.individual.unique", nUniqueInd );
    log.put( "population.individual.selfing", nSelfing );

    log.put( "population.mutations.count", alphabet_type::getInstance()->active_count());
    log.put( "population.mutations.lost_or_fixed", alphabet_type::getInstance()->fixed_lost_count());
    log.put( "population.mutations.single_region.max", nMaxSymbols );
    log.put( "population.mutations.single_region.min", nMinSymbols );
    log.put( "population.mutations.single_region.average", nSymbols/nGametes );

    log.add_child( "population.mutations", mut_stats );

    log.put( "simulation.events.recombination", repro.getRecombinationEvents() );
    log.put( "simulation.events.mutation", repro.getMutationEvents() );

    log.put( "symbol_database.count", alphabet_type::getInstance()->database_size() );
    log.put( "symbol_database.symbol_size", sizeof( alphabet_type::symbol_type));
    log.put( "symbol_database.symbol_per_block", locus_bitset::bitset_type::bits_per_block );
    log.put( "symbol_database.max_block_per_region", (alphabet_type::getInstance()->database_size() / locus_bitset::bitset_type::bits_per_block) + 1 );
    log.put( "simulation.profile.memory_usage.total_blocks", nBlocks );
    log.put( "simulation.profile.memory_usage.blocks_per_region", nBlocks/nGametes );


#ifdef LOGGING
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

    finalize_timer.stop();
    runtime_timer.stop();

    log.put( "simulation.performance.initialize.nanosecond", init_time.elapsed().count() );
    log.put( "simulation.performance.simulate.nanoseconds", sim_time.elapsed().count() );
    log.put( "simulation.performance.finalize.nanoseconds", finalize_timer.elapsed().count() );
    log.put( "simulation.performance.runtime.nanoseconds", runtime_timer.elapsed().count() );
    
    boost::property_tree::write_json( std::cout, log );
    delete [] fitness;

    return 0;
}
