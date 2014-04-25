#ifndef CLOTHO_COMMANDLINE_H_
#define CLOTHO_COMMANDLINE_H_

#include "clotho.h"
#include <boost/program_options.hpp>

namespace po=boost::program_options;

extern const string RUNTIME_K;

extern const string HELP_K;
extern const string VERSION_K;

extern const string ENGINE_CONFIG_K;
extern const string APP_CONFIG_K;

extern const string SIM_UNTIL_K;
extern const string THREAD_COUNT_K;
extern const string GENERATIONS_K;

extern const string SEQSM_K;
extern const string CENSM_K;
extern const string TCENSM_K;

extern const string DISTRIBUTED_ENV_K;
extern const string FOUNDER_SIZE_K;

extern const unsigned int MAX_THREADS;

bool parse_commandline( int argc, char ** argv, po::variables_map & vm );

#endif  // CLOTHO_COMMANDLINE_H_
