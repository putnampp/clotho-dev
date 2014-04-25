#include "clotho_commandline.h"

const string RUNTIME_K = "total_runtime";

const string HELP_K = "help";
const string VERSION_K = "version";

const string ENGINE_CONFIG_K    = "engine_config";
const string APP_CONFIG_K       = "app_config";

const string SIM_UNTIL_K        = "sim-until";
const string THREAD_COUNT_K     = "thread-count";
const string GENERATIONS_K       = "generations";

const string SEQSM_K = "sequential";
const string CENSM_K = "centralized";
const string TCENSM_K = "threaded-centralized";

const string DISTRIBUTED_ENV_K = "dist-env";
const string FOUNDER_SIZE_K = "founder-size";

bool parse_commandline( int argc, char ** argv, po::variables_map & vm ) {
    po::options_description gen( "General" );
    gen.add_options()
    ( (HELP_K + ",h").c_str(), "Print this" )
    ( (VERSION_K + ",v").c_str(), "Version" )
    ;

    po::options_description simulation( "Simulation Parameters" );
    simulation.add_options()
    ( SIM_UNTIL_K.c_str(), po::value<SystemClock::vtime_t>()->default_value( SystemClock::POSITIVE_INFINITY ), "Simulate until time. Default value is positive infinity.")
    ( THREAD_COUNT_K.c_str(), po::value< unsigned int >()->default_value( 4 ), "Thread count for thread aware simulation managers are used. Does not apply when --sequential, or --centralized flags are used")
    ( SEQSM_K.c_str(), "Run the simulation with the sequential simulation manager" )
    ( CENSM_K.c_str(), "Run the simulation with the centralized simulation manager" )
    ( TCENSM_K.c_str(), "Run the simulation with the centralized simulation manager (thread aware)")
    ;

    po::options_description clotho_app( "Clotho Application Parameters" );
    clotho_app.add_options()
    ( GENERATIONS_K.c_str(), po::value<unsigned int>()->default_value(-1), "Simulate a number of generations.")
    ( DISTRIBUTED_ENV_K.c_str(), po::value< unsigned int >()->default_value( 1 ), "Number of environment partitions; Thread aware simulation managers will partition the environment into the max of the number of partitions and thread count" )
    ( FOUNDER_SIZE_K.c_str(), po::value< unsigned int >()->default_value(10000), "Founding population size" )
    ;

    po::options_description cmdline;

    cmdline.add(gen).add(simulation).add( clotho_app );
    po::store( po::command_line_parser( argc, argv ).options( cmdline ).run(), vm );

    bool res = true;
    if( vm.count( HELP_K ) ) {
        std::cout << cmdline << std::endl;
        res = false;
    }

    return res;
}
