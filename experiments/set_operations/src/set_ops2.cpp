#include <iostream>
#include <string>

#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include <boost/dynamic_bitset.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/poisson_distribution.hpp>
#include <boost/random/discrete_distribution.hpp>

#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

//#include "active_space_subset.hpp"
#include "ActiveSpaceSubset.h"

#include "random_pair.hpp"
#include "union_operation.hpp"
#include "intersection_operation.hpp"
#include "difference_operation.hpp"
//
//#include "set_operations.h"

#include "timing.h"

using namespace timing;
using std::string;

template < >
void union_operation< ActiveSpaceSubset, ActiveSpaceSubset >::operator()( const ActiveSpaceSubset & s1, const ActiveSpaceSubset & s2, ActiveSpaceSubset & res )  {
    res = (s1 | s2);
}

template < >
void intersection_operation< ActiveSpaceSubset, ActiveSpaceSubset >::operator()( const ActiveSpaceSubset & s1, const ActiveSpaceSubset & s2, ActiveSpaceSubset & res )  {
    res = (s1 & s2);
}

template < >
void difference_operation< ActiveSpaceSubset, ActiveSpaceSubset >::operator()( const ActiveSpaceSubset & s1, const ActiveSpaceSubset & s2, ActiveSpaceSubset & res )  {
    res = (s1 - s2);
}

template < >
void symmetric_difference_operation< ActiveSpaceSubset, ActiveSpaceSubset >::operator()( const ActiveSpaceSubset & s1, const ActiveSpaceSubset & s2, ActiveSpaceSubset & res )  {
    res = (s1 ^ s2);
}

namespace po=boost::program_options;

typedef ActiveSpaceSubset::value_type       value_type;

typedef std::vector< value_type >           dynamic_subset_type;
typedef std::vector< dynamic_subset_type >  dynamic_subsets;

typedef ActiveSpaceSubset                   indirect_subset_type;
typedef std::vector< indirect_subset_type > indirect_subsets;

// FUNCTION PROTOTYPES
int parse_options( int argc, char ** argv, po::variables_map & vm );

template < class SIterator >
void add_to_log( boost::property_tree::ptree & props, SIterator first, SIterator last, const string & prefix ) {
    std::ostringstream cur_key;
    string ckey = prefix;
    unsigned int i = 0;
    while( first != last ) {
        for( unsigned int j = 0; j < first->size(); ++j) {
            cur_key.str("");
            cur_key.clear();

            cur_key << ckey << "." << i << "." << j;
            props.put( cur_key.str(), (*first)[j]);
        }
        ++i;
        ++first;
    }
}

size_t computeMemoryUsage( const std::vector< double > & s ) {
    return s.size() * sizeof( double );
}

size_t computeMemoryUsage( const ActiveSpaceSubset & s ) {
    return s.num_blocks() * sizeof( typename ActiveSpaceSubset::bitset_type::block_type );
}

size_t result_size( const std::vector< double > & s ) {
    return s.size();
}

size_t result_size( const ActiveSpaceSubset & s ) {
    return s.count();
}

string data_type( const std::vector< double > & s ) {
    static string res("dynamic");
    return res;
}

string data_type( const ActiveSpaceSubset & s ) {
    static string res("binary");
    return res;
}

template < class LogType = boost::property_tree::ptree >
class PerformanceTester {
public:
    typedef LogType performance_log_type;

    PerformanceTester( performance_log_type & log, string key_prefix) : m_log(&log), m_prefix( key_prefix ) {}
    PerformanceTester( performance_log_type * log, string key_prefix) : m_log(log), m_prefix( key_prefix ) {}

    template< class ForwardIterator, typename OP >
    void operator()( ForwardIterator first, ForwardIterator last, OP op ) {
        std::ostringstream oss;

        INIT_LAPSE_TIME;

        string op_type = op.operation_name();

        string all_path = m_prefix;
        string op_path = m_prefix;

        if( m_prefix.back() != '.') { all_path += "."; op_path += "."; }

        all_path += "all";
        op_path += op_type;

        if( m_log->get_child_optional( all_path ) == boost::none ) {
            boost::property_tree::ptree dt;
            m_log->add_child( all_path + ".z.DataType", dt );
            m_log->add_child( all_path + ".z.OperationType", dt );

            boost::property_tree::ptree data;
            boost::property_tree::ptree smps;
            boost::property_tree::ptree vars;

            boost::property_tree::ptree _1, _2, _3, _4;
            _1.put( "", "Set1 Size");
            _2.put( "", "Set2 Size");
            _3.put( "", "Result Size");
            _4.put( "", "Runtime");

            smps.push_back( std::make_pair( "", _1));
            smps.push_back( std::make_pair( "", _2));
            smps.push_back( std::make_pair( "", _3));
            smps.push_back( std::make_pair( "", _4));

            m_log->add_child( all_path + ".y.smps", smps);
            m_log->add_child( all_path + ".y.data", data);
            m_log->add_child( all_path + ".y.vars", vars);
        }

        if( m_log->get_child_optional( op_path +  ".z" ) == boost::none ||
            m_log->get_child_optional( op_path + ".z.DataType") == boost::none ) {
            boost::property_tree::ptree dt;
            m_log->add_child( op_path + ".z.DataType", dt );
        }

        unsigned int k = 0;
        if( m_log->get_child_optional( op_path + ".y" ) == boost::none) {
            boost::property_tree::ptree data;
            boost::property_tree::ptree smps;
            boost::property_tree::ptree vars;

            boost::property_tree::ptree _1, _2, _3, _4;
            _1.put( "", "Set1 Size");
            _2.put( "", "Set2 Size");
            _3.put( "", "Result Size");
            _4.put( "", "Runtime");

            smps.push_back( std::make_pair( "", _1));
            smps.push_back( std::make_pair( "", _2));
            smps.push_back( std::make_pair( "", _3));
            smps.push_back( std::make_pair( "", _4));

            m_log->add_child( op_path + ".y.smps", smps);
            m_log->add_child( op_path + ".y.data", data);
            m_log->add_child( op_path + ".y.vars", vars);
        } else { 
            k = m_log->get_child( op_path + ".y.data" ).count( "" );
        }

        while( first != last ) {
            typename OP::result_type r;
            RECORD_START;
            op( (*first->first), (*first->second), r );
            RECORD_STOP;

            oss.str("");
            oss.clear();
            PRINT_LAPSE( oss, "" );
            string val = oss.str().substr(0, oss.str().size() - 1);

            boost::property_tree::ptree smp;

            boost::property_tree::ptree s1, s2, s3, s4, s5;
            s1.put( "", result_size((*first->first)));
            s2.put( "", result_size((*first->second)));
            s3.put( "", result_size(r));
            s4.put( "", val );

            smp.push_back( std::make_pair( "", s1) );
            smp.push_back( std::make_pair( "", s2) );
            smp.push_back( std::make_pair( "", s3) );
            smp.push_back( std::make_pair( "", s4) );

            oss.str("");
            oss.clear();
            oss << "s" << k;
            boost::property_tree::ptree v;
            v.put("", oss.str() );

            m_log->get_child(op_path + ".y.vars").push_back( std::make_pair("", v));
            m_log->get_child(op_path + ".y.data").push_back( std::make_pair("", smp));

            m_log->get_child(all_path + ".y.vars").push_back( std::make_pair("", v) );
            m_log->get_child(all_path + ".y.data").push_back( std::make_pair("", smp) );

            boost::property_tree::ptree dt;
            dt.put( "", data_type(r) );

            m_log->get_child(op_path + ".z.DataType").push_back( std::make_pair("", dt));
            m_log->get_child(all_path + ".z.DataType").push_back( std::make_pair("", dt));
            
            boost::property_tree::ptree oper;
            oper.put( "", op.operation_name() );
            m_log->get_child(all_path + ".z.OperationType").push_back( std::make_pair("", oper ));

            ++first;
            ++k;
        }
    }

    virtual ~PerformanceTester() {}
protected:
    performance_log_type *  m_log;
    string                  m_prefix;
};

void convert( const dynamic_subsets & src, indirect_subsets & dest ) {
    dest.reserve( src.size() );
    for( dynamic_subsets::const_iterator it = src.begin(); it != src.end(); it++ ) {
        indirect_subset_type s;
        for( dynamic_subset_type::const_iterator it2 = it->begin(); it2 != it->end(); it2++) {
            s.add( *it2 );
        }
        dest.push_back( s );
    }
}

template < class URNG >
struct my_uniform_generator {
    typedef URNG rng_type;
    rng_type    * rng;
    boost::random::uniform_int_distribution<> dist;

    my_uniform_generator( rng_type * r ) : rng(r) {}
    my_uniform_generator( const my_uniform_generator< URNG > & other ) : rng(other.rng) {}

    unsigned int operator()( unsigned int min, unsigned int max ) {
        unsigned int range = (max - min);
        unsigned int val = dist( *rng ) % range;
        return min + val;
    }
};

struct simulator {

    simulator( boost::property_tree::ptree & p, boost::random::mt19937 & rgen ) : sim_props( &p ), mug( &rgen ) {}
    simulator( boost::property_tree::ptree * p, boost::random::mt19937 * rgen ) : sim_props( p ), mug( rgen ) {}

    template < class SAMPLES >
    void operator()( SAMPLES & samples, unsigned int nReps, unsigned int nCases, string prefix ) {
        typedef typename SAMPLES::iterator siterator;

        for( unsigned int i = 0; i < nReps; ++i ) {
            std::ostringstream oss;

            std::vector< std::pair< siterator, siterator > > pairs;
            random_pair_n( samples.begin(), samples.end(), nCases, std::back_inserter( pairs ), mug );

            oss << prefix;
            if( prefix.back() != '.' ) {
                oss << ".";
            }
            oss << i;

            string round_prefix = oss.str();

            {
                PerformanceTester<> pt( sim_props, round_prefix );
                pt( pairs.begin(), pairs.end(), union_operation< typename SAMPLES::value_type >() );
            }

            {
                PerformanceTester<> pt( sim_props, round_prefix );
                pt( pairs.begin(), pairs.end(), intersection_operation< typename SAMPLES::value_type >() );
            }

            {
                PerformanceTester<> pt( sim_props, round_prefix );
                pt( pairs.begin(), pairs.end(), difference_operation< typename SAMPLES::value_type >() );
            }

            {
                PerformanceTester<> pt( sim_props, round_prefix );
                pt( pairs.begin(), pairs.end(), symmetric_difference_operation< typename SAMPLES::value_type >() );
            }

            string tmpk = round_prefix + ".pairs." + data_type(samples[0]);
            if( sim_props->get_child_optional( tmpk ) == boost::none ) {
                boost::property_tree::ptree pt;
                sim_props->add_child( tmpk, pt);
            }

            for( unsigned int j = 0; j < pairs.size(); ++j) {
                boost::property_tree::ptree c1, c2, p;
                c1.put( "", (pairs[j].first - samples.begin()));
                c2.put( "", (pairs[j].second - samples.begin()));

                p.push_back( std::make_pair( "", c1));
                p.push_back( std::make_pair( "", c2));

                sim_props->get_child( tmpk ).push_back( std::make_pair( "", p ));
            }
        }
    }

    boost::property_tree::ptree * sim_props;
    my_uniform_generator< boost::random::mt19937> mug;
};

template < class SAMPLE, class URNG, class SizeDistribution, class ValueDistribution >
struct sample_generator {

    sample_generator( URNG & rng, SizeDistribution & sdist, ValueDistribution & vdist ) : m_rng( &rng ), m_size_dist( &sdist), m_value_dist( &vdist ) {}

    SAMPLE operator()() {
        SAMPLE res;

        unsigned int size = (*m_size_dist)( *m_rng );
        for( unsigned int i = 0; i < size; ++i ) {
            res.push_back( (*m_value_dist)( *m_rng ) );
        }
        std::sort( res.begin(), res.end() );
        return res;
    }

    URNG *  m_rng;
    SizeDistribution * m_size_dist;
    ValueDistribution * m_value_dist;
};

struct constant_weight {
    template < class T >
    double operator()( T & s) {
        return 1.0;
    }
};

template < typename ValueDistribution, typename WeightGenerator >
struct fixed_value_space {

    typedef typename ValueDistribution::result_type result_type;

    template < class URNG >
    fixed_value_space( URNG rng, unsigned int size, ValueDistribution & vgen,  WeightGenerator & wgen ) : m_dist(NULL), m_vgen( &vgen ), m_wgen( &wgen ) {
        generate_space( rng, size );
    }

    template < class URNG >
    result_type operator()( URNG & rng ) {
        return m_values[ (*m_dist)(rng) ];
    }

    template < class URNG >
    void generate_space( URNG & rng, unsigned int size ) {
        if( m_dist != NULL ) delete m_dist;

        std::set< double > vals;
        while( vals.size() < size ) {
            vals.insert( (*m_vgen)( rng ) );
        }

        m_values.clear();
        std::copy( vals.begin(), vals.end(), std::back_inserter( m_values ));

        std::vector< double > weights(size);
        for( unsigned int i = 0; i < size; ++i ) {
            weights[i] = (*m_wgen)( rng );
        }
        m_dist = new boost::random::discrete_distribution< unsigned int, double >( weights.begin(), weights.end());
    }

    virtual ~fixed_value_space() {
        if( m_dist != NULL ) delete m_dist;
    }

    std::vector< result_type >         m_values;
    boost::random::discrete_distribution< unsigned int, double > * m_dist;
    ValueDistribution * m_vgen;
    WeightGenerator * m_wgen;
};

// CONSTANT VALUES
const unsigned int DEFAULT_SUBSETS = 10000;
const double DEFAULT_MU = 1.0;
const unsigned int INFINITE_SPACE = -1;

// CONSTANT STRINGS
const string HELP_K = "help";

const string MAX_SUBSETS_K = "max-subsets";
const string MU_K = "mu";
const string SEED_K = "seed";
const string OUTPUT_FILE_K = "output";
const string MAX_UNIQUE_VALUES_K = "max-unique-values";

const string BINARY_SUBSET_K = "binary";
const string DYNAMIC_SUBSET_K = "dynamic";

const string REPETITION_K = "reps";
const string TEST_CASE_RATE_K = "sampling-rate";

int main(int argc, char ** argv) {
    int status = 0;
    po::variables_map vm;

    if((status = parse_options( argc, argv, vm )) != 0 ) {
        return status;
    }

    std::ostream * out = &std::cout;
    bool bCleanupOut = false;

    if( (bCleanupOut = !vm[ OUTPUT_FILE_K ].as< string >().empty()) ) {
        out = new std::ofstream( vm[ OUTPUT_FILE_K ].as<string>().c_str() );
    }

    boost::property_tree::ptree sim_props;

    std::cerr << "Random Seed: " << vm[ SEED_K ].as< unsigned long >() << std::endl;
    boost::random::mt19937 rgen( vm[ SEED_K ].as< unsigned long >() );

    sim_props.put( "rng.type", "mt19937");
    sim_props.put( "rng.seed", vm[ SEED_K ].as< unsigned long >() );

    unsigned int N = vm[ MAX_SUBSETS_K ].as< unsigned int >();
    double mu = vm[ MU_K].as< double >();

    sim_props.put( "value_space.distribution", "Uniform");
    sim_props.put( "value_space.range", "[0, 1)" );
    sim_props.put( "subsets.size.distribution", "Poisson" );
    sim_props.put( "subsets.size.rate", mu );

    std::cerr << "Simulating a data set with " << N << " objects" << std::endl;
    std::cerr << "Each object is expected to have " << mu << " elements (Poisson Distributed)" << std::endl;
    std::cerr << "Elements are random values in [0, 1) following a Uniform Distribution" << std::endl;

    unsigned int nReps = vm[ REPETITION_K ].as< unsigned int >();
    unsigned int nCases = N * vm[ TEST_CASE_RATE_K ].as< double >();

    sim_props.put( "runtime.units", "seconds");

    
    boost::random::poisson_distribution< unsigned int, double> poisson(mu);
    dynamic_subsets samples;
    if( vm[ MAX_UNIQUE_VALUES_K ].as< unsigned int >() == INFINITE_SPACE ) {
        sim_props.put( "value_space.max_size", "infinite");
        typedef boost::random::uniform_01< double > value_space_type;
        typedef sample_generator< dynamic_subsets::value_type, 
                    boost::random::mt19937, 
                    boost::random::poisson_distribution< unsigned int, double>, 
                    value_space_type > sample_gen_type;
        value_space_type uni;

        std::generate_n( std::back_inserter( samples ), N, sample_gen_type( rgen, poisson, uni ) );
    } else {
        sim_props.put( "value_space.max_size", vm[ MAX_UNIQUE_VALUES_K ].as< unsigned int >() );
        typedef fixed_value_space< boost::random::uniform_01< double >, constant_weight > value_space_type;
        boost::random::uniform_01< double > uni;
        constant_weight cw;

        value_space_type values(rgen, vm[ MAX_UNIQUE_VALUES_K ].as< unsigned int >(), uni, cw );

        typedef sample_generator< dynamic_subsets::value_type, boost::random::mt19937, boost::random::poisson_distribution< unsigned int, double>, value_space_type > sample_gen_type;

        std::generate_n( std::back_inserter( samples ), N, sample_gen_type(rgen, poisson, values) );
    }

    add_to_log( sim_props, samples.begin(), samples.end(), "samples" );

    bool bRanAnalysis = false;
    simulator sim( sim_props, rgen );

    sim_props.put( "runtime.units", "seconds");

    if( vm.count( DYNAMIC_SUBSET_K ) ) {
        std::cerr << "Running dynamic vector set" << std::endl;

        sim( samples, nReps, nCases, "cases" );

        bRanAnalysis = true;
    }

    if ( vm.count( BINARY_SUBSET_K) ) {
        std::cerr << "Running binary set" << std::endl;

        indirect_subsets samples2;
        convert( samples, samples2);

        sim( samples2, nReps, nCases, "cases" );

        sim_props.put( "value_space.enumerated", ActiveSpaceSubset::enumerated_space_size());
        bRanAnalysis = true;
    }

    if(!bRanAnalysis) {
        (*out) << "Didn't run an analysis" << std::endl;
    }

    boost::property_tree::json_parser::write_json( (*out), sim_props);

    if( bCleanupOut ) {
        ((std::ofstream * )out)->close();
        delete out;
    }
    return status;
}

int parse_options( int argc, char ** argv, po::variables_map & vm ) {

    po::options_description general( "General options" );
    general.add_options()
    ( (HELP_K + ",h").c_str(), "Print this stuff" )
    ;

    po::options_description params( "Parameters" );
    params.add_options()
    ( MAX_SUBSETS_K.c_str(), po::value< unsigned int >()->default_value( DEFAULT_SUBSETS ), "Number of subsets to produce")
    ( MAX_UNIQUE_VALUES_K.c_str(), po::value< unsigned int >()->default_value( INFINITE_SPACE ), "Maximum number of unique values found in subsets. [ (unsigned int) -1 = infinite]")
    ( MU_K.c_str(), po::value< double >()->default_value( DEFAULT_MU ), "Expected number of elements per subset")
    ( SEED_K.c_str(), po::value< unsigned long >()->default_value(-1), "Random number generator seed value" )
    ;

    po::options_description control("Simulation Control");
    control.add_options()
    ( BINARY_SUBSET_K.c_str(), "Enumerate space elements, and represent subsets as binary sequence")
    ( DYNAMIC_SUBSET_K.c_str(), "Represent subsets as dynamic sorted vectors [Default]" )
    ( REPETITION_K.c_str(), po::value< unsigned int >()->default_value( 10 ), "Number of times to repeat the performance simulation" )
    ( TEST_CASE_RATE_K.c_str(), po::value< double >()->default_value( 1.0 ), "Random Tests per Data Set per Simulation Repetition")
    ( (OUTPUT_FILE_K + ",o" ).c_str(), po::value< string >()->default_value(""), "Output file" )
    ;

    po::options_description cmdline;
    cmdline.add(general).add(params).add( control );

    po::store( po::parse_command_line( argc, argv, cmdline), vm );
    po::notify(vm);

    if( vm.count(HELP_K) ) {
        std::cout << cmdline << std::endl;
        return 1;
    }

    return 0;
}
