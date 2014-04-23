#ifndef DEFAULT_LIFE_CYCLE_H_
#define DEFAULT_LIFE_CYCLE_H_

#include "life_cycle_model.h"

//#include "../object/individual.hpp"
//#include "../object/environment.hpp"
//#include "../object/distributed_environment.hpp"

class default_life_cycle : public life_cycle_model {};

//
// TIndividual
//

//template < class LCM, class IP > void TIndividual<default_life_cycle, IP, default_life_cycle >::initialize();
//template < class VT > void TIndividual<default_life_cycle, VT>::perform_event( const event * );
//template < class VT > void TIndividual<default_life_cycle, VT>::finalize();
//template < class VT > void TIndividual<default_life_cycle, VT>::handle_birth( const ClothoEvent * evt );
//template < class VT > void TIndividual<default_life_cycle, VT>::handle_death( const ClothoEvent * evt );
//template < class VT > void TIndividual<default_life_cycle, VT>::handle_inherit( const ClothoEvent * evt );
//template < class VT > void TIndividual<default_life_cycle, VT>::handle_mate( const ClothoEvent * evt );
//template < class VT > void TIndividual<default_life_cycle, VT>::handle_maturity( const ClothoEvent * evt );

//
// TENVIRONMENT
//
/*template < class VT > void TEnvironment<default_life_cycle, VT>::initialize();
template < class VT > void TEnvironment<default_life_cycle, VT>::perform_event( const event * e );
template < class VT > void TEnvironment<default_life_cycle, VT>::finalize();
template < class VT > system_id TEnvironment<default_life_cycle, VT>::getIndividual();
template < class VT > void TEnvironment<default_life_cycle, VT>::handle_birth( const ClothoEvent * ce );
template < class VT > void TEnvironment<default_life_cycle, VT >::handle_maturity( const ClothoEvent * ce );
template < class VT > void TEnvironment<default_life_cycle, VT>::handle_death( const ClothoEvent * ce );

//
// TDistributedEnvironment
//
template < class VT > void TDistributedEnvironment<default_life_cycle, VT>::process();
template < class VT > void TDistributedEnvironment<default_life_cycle, VT>::perform_event(const event * e );
template < class VT > void TDistributedEnvironment<default_life_cycle, VT>::handle_birth( const ClothoEvent * ce );
template < class VT > void TDistributedEnvironment<default_life_cycle, VT>::handle_maturity( const ClothoEvent * ce );
template < class VT > void TDistributedEnvironment<default_life_cycle, VT>::handle_signal_mate( const ClothoEvent * ce );
template < class VT > void TDistributedEnvironment<default_life_cycle, VT>::handle_snapshot( const ClothoEvent * ce );
template < class VT > void TDistributedEnvironment<default_life_cycle, VT>::handle_selection( const ClothoEvent * ce );
*/
//
// Default typedefs
//
//typedef TIndividual< default_life_cycle > DefaultIndividual;
//typedef TEnvironment< default_life_cycle, variant_base >  DefaultEnvironment;
//typedef DefaultEnvironment::individual_t    DefaultIndividual;
//typedef TDistributedEnvironment< default_life_cycle > DefaultDistributedEnvironment;





#endif  // DEFAULT_LIFE_CYCLE_H_
