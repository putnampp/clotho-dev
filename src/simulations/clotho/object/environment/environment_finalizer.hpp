#ifndef ENVIRONMENT_FINALIZER_HPP_
#define ENVIRONMENT_FINALIZER_HPP_

#include "finalizer.hpp"

namespace finalizer {

class EnvironmentFinalizer {
public:
    template < typename E >
    static void finalize( E * env ) {
        env->m_individual_pool.reset();

        env->m_id_pool_index_map.clear();

        while( !env->m_individuals.empty() ) {
            env->m_individuals.back().finalize();

            env->m_individuals.pop_back();
        }
/*        while( !env->m_active_individuals.empty() ) {
            object * tmp = env->m_sim_manager->getObject( env->m_active_individuals.begin()->first );
            env->m_active_individuals.erase( env->m_active_individuals.begin() );

            tmp->finalize();
            delete tmp;
        }

        while( !env->m_pending.empty() ) {
            object * tmp = env->m_sim_manager->getObject( *(env->m_pending.begin()) );
            env->m_pending.erase( env->m_pending.begin());

            if( tmp ) { 
                tmp->finalize();
                delete tmp;
            }
        }

        while( !env->m_available_individuals.empty() ) {
            object * tmp = env->m_sim_manager->getObject( env->m_available_individuals.front() );
            env->m_available_individuals.pop_front();

            if( tmp ) {
                tmp->finalize();
                delete tmp;
            }
        }
*/
    }

protected:
    
};

}       // namespace finalizer

#endif  // ENVIRONMENT_FINALIZER_HPP_
