#ifndef SELECTION_HPP_
#define SELECTION_HPP_

#include "rng/random_process.hpp"

namespace selection {

namespace models {

template < class P >
struct model {
    typedef P parameter_t;
};

}   // namespace models

template < class MODEL >
class MateSelector : public RandomProcess {
public:
    typedef MODEL model_t;
    typedef typename MODEL::parameter_t parameter_t;

    template < class ENV, typename PARAM, typename RES = PARAM >
    static RES select( ENV * env, PARAM p) {
        return select( env, p, env->selection_parameter() );
    }

    template < class ENV, typename RES >
    static RES select( ENV * env ) {
        assert(false);
    }

    /**
     *  by default this selection just returns the input param
     *  ie. selfing (cloning) reproduction scheme
     */
    template < class ENV, typename PARAM, typename RES = PARAM >
    static RES select( ENV * env, PARAM p, parameter_t * c ) {
        return p;
    }
};

}   // namespace selection

#endif  // SELECTION_HPP_
