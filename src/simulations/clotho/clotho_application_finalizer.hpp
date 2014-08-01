#ifndef CLOTHO_APPLICATION_FINALIZER_HPP_
#define CLOTHO_APPLICATION_FINALIZER_HPP_

#include "finalizer.hpp"

namespace finalizer {

class ClothoAppFinalizer {
public:
    template < typename APP >
    static void finalize( APP * app ) {
//        while( !app->m_objects.empty() ) {
//            system_id tmp = app->m_objects.back();
//            app->m_objects.pop_back();

//            object * tmp_obj = app->m_sim_manager->getObject( tmp );
//            if( tmp_obj ) {
//                tmp_obj->finalize();
//            }
//        }
        app->m_env->finalize();
    }
};

}   // namespace finalizer
#endif  // CLOTHO_APPLICATION_FINALIZER_HPP_
