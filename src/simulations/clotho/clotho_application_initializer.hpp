#ifndef CLOTHO_APPLICATION_INITIALIZER_HPP_
#define CLOTHO_APPLICATION_INITIALIZER_HPP_

#include "initializer.hpp"

namespace initializer {

class ClothoAppInitializer {
public:
    template < typename T >
    static  void init( T * t ) {
        createEnvironment( t, (typename T::environment_ptr_t) NULL);
    }

protected:
    template < typename APP, typename ENV >
    static void createEnvironment( APP * a, ENV * ) {
        typename APP::manager_t::object_t * cobj = a->m_sim_manager->createSimulationObject();

        typename APP::environment_ptr_t env = new typename APP::environment_t( cobj );
        env->initialize();
        a->m_objects.push_back( env->getSystemID() );
    }
};

}

#endif  // CLOTHO_APPLICATION_INITIALIZER_HPP_
