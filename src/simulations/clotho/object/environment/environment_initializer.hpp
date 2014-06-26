#ifndef ENVIRONMENT_INITIALIZER_HPP_
#define ENVIRONMENT_INITIALIZER_HPP_

#include "initializer.hpp"

namespace initializer {

class EnvironmentInitializer {
public:
    template < typename E >
    static  void    init( E * env ) {
        createPopulation( env, (typename E::individual_ptr_t) NULL );
    }
protected:

    template < typename E, typename IND >
    static void createPopulation( E * env, IND * ) {
        cout << "Creating founding population of " << env->m_nFounder << endl;
        env->m_individuals.reserve( 2 * env->m_nFounder );
        for( unsigned int i = 0; i < env->m_nFounder; ++i ) {
            //system_id id = env->getIndividual();
            //object * ind = env->m_sim_manager->getObject( id );
            //ind->initialize();
            //
            size_t idx = env->getIndividualIndex();
            env->getIndividualAt(idx).initialize();
        }
    }
};

}   // namespace initializer

#endif  // ENVIRONMENT_INITIALIZER_HPP_
