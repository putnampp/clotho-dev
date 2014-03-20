#ifndef DEFAULT_LIFE_CYCLE_H_
#define DEFAULT_LIFE_CYCLE_H_

#include "life_cycle_model.h"

#include "../object/individual.hpp"
#include "../object/environment.hpp"
#include "../object/distributed_environment.hpp"

class default_life_cycle : public life_cycle_model {};

//
// TIndividual
//
template <> void TIndividual<default_life_cycle>::initialize();
template <> void TIndividual<default_life_cycle>::perform_event( const event * );
template <> void TIndividual<default_life_cycle>::finalize();
template <> void TIndividual<default_life_cycle>::handle_birth( const ClothoEvent * evt );
template <> void TIndividual<default_life_cycle>::handle_death( const ClothoEvent * evt );
template <> void TIndividual<default_life_cycle>::handle_inherit( const ClothoEvent * evt );
template <> void TIndividual<default_life_cycle>::handle_mate( const ClothoEvent * evt );
template <> void TIndividual<default_life_cycle>::handle_maturity( const ClothoEvent * evt );

//
// TENVIRONMENT
//
template <> void TEnvironment<default_life_cycle>::initialize();
template <> void TEnvironment<default_life_cycle>::perform_event( const event * e );
template <> void TEnvironment<default_life_cycle>::finalize();
template <> system_id TEnvironment<default_life_cycle>::getIndividual();
template <> void TEnvironment<default_life_cycle>::handle_birth( const ClothoEvent * ce );
template <> void TEnvironment< default_life_cycle >::handle_maturity( const ClothoEvent * ce );
template <> void TEnvironment<default_life_cycle>::handle_death( const ClothoEvent * ce );

//
// TDistributedEnvironment
//
template <> void TDistributedEnvironment<default_life_cycle>::process();
template <> void TDistributedEnvironment<default_life_cycle>::perform_event(const event * e );
template <> void TDistributedEnvironment<default_life_cycle>::handle_birth( const ClothoEvent * ce );
template <> void TDistributedEnvironment<default_life_cycle>::handle_maturity( const ClothoEvent * ce );
template <> void TDistributedEnvironment<default_life_cycle>::handle_signal_mate( const ClothoEvent * ce );
template <> void TDistributedEnvironment<default_life_cycle>::handle_snapshot( const ClothoEvent * ce );
template <> void TDistributedEnvironment<default_life_cycle>::handle_selection( const ClothoEvent * ce );

//
// Default typedefs
//
typedef TIndividual< default_life_cycle > DefaultIndividual;
typedef TEnvironment< default_life_cycle > DefaultEnvironment;
typedef TDistributedEnvironment< default_life_cycle > DefaultDistributedEnvironment;

#endif  // DEFAULT_LIFE_CYCLE_H_
