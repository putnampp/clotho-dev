#ifndef ENVIRONMENT_INITIALIZER_HPP_
#define ENVIRONMNET_INITIALIZER_HPP_

#include "../initializer.hpp"

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
        for( unsigned int i = 0; i < env->m_nFounder; ++i ) {
            system_id id = env->getIndividual();
            object * ind = env->m_sim_manager->getObject( id );

            ind->initialize();
        }
    }
};

}   // namespace initializer

#endif  // ENVIRONMENT_INITIALIZER_HPP_
