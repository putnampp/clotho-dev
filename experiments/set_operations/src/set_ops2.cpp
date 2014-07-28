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

//using set_operations::union_operation;
//using set_operations::intersect_operation;
//using set_operations::difference_operation;
//using set_operations::symmetric_difference_operation;

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

namespace po=boost::program_options;

typedef ActiveSpaceSubset::value_type       value_type;

typedef std::vector< value_type >           dynamic_subset_type;
typedef std::vector< dynamic_subset_type >  dynamic_subsets;

typedef ActiveSpaceSubset                   indirect_subset_type;
typedef std::vector< indirect_subset_type > indirect_subsets;

// FUNCTION PROTOTYPES
int parse_options( int argc, char ** argv, po::variables_map & vm );

// TEMPLATE Objects
template < class S >
struct build;

template < template < class, class... > class SMPLS, class SUBSET, class... REST >
struct build< SMPLS< SUBSET, REST...> > {
    typedef SUBSET subset_type;
    typedef SMPLS< SUBSET, REST... > samples_type;

    template < class URNG, class SubsetDist, class ValueDist >
    void operator()( samples_type & samples, unsigned int N, URNG & rgen, SubsetDist & ss, ValueDist & vdist ) {
        std::cerr << "Building sample of " << N << " space subsets" << std::endl;
        for( unsigned int i = 0; i < N; ++i ) {
            unsigned int m = ss( rgen );

            subset_type samp;
            for( unsigned int j = 0; j < m; ++j ) {
                double v = vdist( rgen );
                samp.push_back( v );
            }

            std::sort( samp.begin(), samp.end() );
            samples.push_back(samp);
        }
    }
};

template < class SIterator >
void add_to_log( boost::property_tree::ptree & props, SIterator first, SIterator last, const string & prefix ) {
    std::ostringstream cur_key;
    props.put( prefix + ".N", (last - first) );
    string ckey = prefix + ".data";
    unsigned int i = 0;
    while( first != last ) {
        cur_key.str("");
        cur_key.clear();

        cur_key << ckey << "." << i << ".N";
        props.put( cur_key.str(), first->size());
        for( unsigned int j = 0; j < first->size(); ++j) {
            cur_key.str("");
            cur_key.clear();

            cur_key << ckey << "." << i << ".data." << j;
            props.put( cur_key.str(), (*first)[j]);
        }
        ++i;
        ++first;
    }
}


template < class LogType = boost::property_tree::ptree >
class PerformanceTester {
public:
    typedef LogType performance_log_type;

    PerformanceTester( performance_log_type & log, string key_prefix) : m_log(&log), m_prefix( key_prefix ) {}

    template< class ForwardIterator, typename OP >
    void operator()( ForwardIterator first, ForwardIterator last, OP op ) {
        std::ostringstream oss;

        INIT_LAPSE_TIME;

        unsigned int k = 0;
        while( first != last ) {
            typename OP::result_type r;
            RECORD_START;
            op( (*first->first), (*first->second), r );
            RECORD_STOP;

            oss.str( "" );
            oss.clear();
            oss << m_prefix << k++;
            string ckey = oss.str();

            oss.str("");
            oss.clear();
            PRINT_LAPSE( oss, "" );

            m_log->put( ckey + ".lapse", oss.str() );
            ++first;
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


// CONSTANT VALUES
const unsigned int DEFAULT_SUBSETS = 10000;
const double DEFAULT_MU = 1.0;

// CONSTANT STRINGS
const string HELP_K = "help";

const string MAX_SUBSETS_K = "max-subsets";
const string MU_K = "mu";
const string SAMPLING_SIZE_K = "sampling-rate";
const string SEED_K = "seed";
const string OUTPUT_FILE_K = "output";

const string BINARY_SUBSET_K = "binary";
const string DYNAMIC_SUBSET_K = "dynamic";

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

    boost::random::poisson_distribution< unsigned int, double> poisson(mu);
    boost::random::uniform_01< double > uni;

    sim_props.put( "value_space.distribution", "Uniform");
    sim_props.put( "value_space.range", "[0, 1)" );
    sim_props.put( "subsets.size.distribution", "Poisson" );
    sim_props.put( "subsets.size.rate", mu );

    std::cerr << "Simulating a data set with " << N << " objects" << std::endl;
    std::cerr << "Each object is expected to have " << mu << " elements (Poisson Distributed)" << std::endl;
    std::cerr << "Elements are random values in [0, 1) following a Uniform Distribution" << std::endl;

    dynamic_subsets samples;
    build< dynamic_subsets > b;
    b( samples, N, rgen, poisson, uni );

    add_to_log( sim_props, samples.begin(), samples.end(), "samples.dynamic" );

    my_uniform_generator< boost::random::mt19937> mug( &rgen );

//    union_operation< dynamic_subsets::value_type > dynamic_union_op;

    bool bRanAnalysis = false;
    if( vm.count( DYNAMIC_SUBSET_K ) ) {
        std::cerr << "Running dynamic vector set" << std::endl;

        for( int i = 0; i < 10; ++i ) {
            std::ostringstream oss;

            std::vector< std::pair< dynamic_subsets::iterator, dynamic_subsets::iterator > > pairs;
            random_pair_n( samples.begin(), samples.end(), (unsigned int) 5, std::back_inserter( pairs ), mug );

            oss << "samples.dynamic.test." << i << ".union.";
            {
                PerformanceTester<> pt( sim_props, oss.str() );
                pt( pairs.begin(), pairs.end(), union_operation< dynamic_subsets::value_type >() );
            }


            oss.str("");
            oss.clear();
            oss << "samples.dynamic.test." << i << ".intersection.";
            {
                PerformanceTester<> pt( sim_props, oss.str() );
                pt( pairs.begin(), pairs.end(), intersection_operation< dynamic_subsets::value_type >() );
            }

            oss.str("");
            oss.clear();
            oss << "samples.dynamic.test." << i << ".difference.";
            {
                PerformanceTester<> pt( sim_props, oss.str() );
                pt( pairs.begin(), pairs.end(), difference_operation< dynamic_subsets::value_type >() );
            }

            oss.str("");
            oss.clear();
            oss << "samples.dynamic.test." << i << ".pairs.";
            string tmpk = oss.str();
            for( unsigned int j = 0; j < pairs.size(); ++j) {
                oss.str("");
                oss.clear();
                oss << tmpk << j;
                sim_props.put( oss.str() + ".first", pairs[ j ].first - samples.begin() );
                sim_props.put( oss.str() + ".second", pairs[ j ].second - samples.begin() );
            }
        }

        bRanAnalysis = true;
    }

    if ( vm.count( BINARY_SUBSET_K) ) {
        std::cerr << "Running binary set" << std::endl;

        indirect_subsets samples2;
        convert( samples, samples2);

        for( int i = 0; i < 10; ++i ) {
            std::ostringstream oss;

            std::vector< std::pair< indirect_subsets::iterator, indirect_subsets::iterator > > pairs;
            random_pair_n( samples2.begin(), samples2.end(), (unsigned int) 5, std::back_inserter( pairs ), mug );

            oss << "samples.indirect.test." << i << ".union.";
            {
                PerformanceTester<> pt( sim_props, oss.str() );
                pt( pairs.begin(), pairs.end(), union_operation< indirect_subsets::value_type >() );
            }

            oss.str("");
            oss.clear();
            oss << "samples.indirect.test." << i << ".intersection.";
            {
                PerformanceTester<> pt( sim_props, oss.str() );
                pt( pairs.begin(), pairs.end(), intersection_operation< indirect_subsets::value_type >() );
            }

            oss.str("");
            oss.clear();
            oss << "samples.indirect.test." << i << ".difference.";
            {
                PerformanceTester<> pt( sim_props, oss.str() );
                pt( pairs.begin(), pairs.end(), difference_operation< indirect_subsets::value_type >() );
            }

            oss.str("");
            oss.clear();
            oss << "samples.indirect.test." << i << ".pairs.";
            string tmpk = oss.str();
            for( unsigned int j = 0; j < pairs.size(); ++j) {
                oss.str("");
                oss.clear();
                oss << tmpk << j;
                sim_props.put( oss.str() + ".first", pairs[ j ].first - samples2.begin() );
                sim_props.put( oss.str() + ".second", pairs[ j ].second - samples2.begin() );
            }
        }
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
    ( MU_K.c_str(), po::value< double >()->default_value( DEFAULT_MU ), "Expected number of elements per subset")
    ( SAMPLING_SIZE_K.c_str(), po::value< unsigned int >()->default_value( 10 ), "Performance test sample size")
    ( SEED_K.c_str(), po::value< unsigned long >()->default_value(-1), "Random number generator seed value" )
    ( (OUTPUT_FILE_K + ",o" ).c_str(), po::value< string >()->default_value(""), "Output file" )
    ;

    po::options_description control("Control");
    control.add_options()
    ( BINARY_SUBSET_K.c_str(), "Enumerate space elements, and represent subsets as binary sequence")
    ( DYNAMIC_SUBSET_K.c_str(), "Represent subsets as dynamic sorted vectors [Default]" )
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
