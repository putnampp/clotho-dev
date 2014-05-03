#ifndef REPRODUCTION_HPP_
#define REPRODUCTION_HPP_

#include "rng/random_process.hpp"

namespace reproduction {

namespace models {

template < class P >
struct model {
    typedef P parameter_t;
};

}   // namespace models

//template < class MODEL = models::model<void> >
template < class MUT, class REC >
class IndividualReproduction {
public:
//    typedef MODEL model_t;
    typedef MUT mutation_t;
    typedef REC recombination_t;

    template < typename IND, typename GAM = typename IND::properties_t::gamete_t >
    static GAM * reproduce( IND * ind, GAM * ) {
        GAM * res = recombination_t::recombine(ind->getProperties(), (GAM *) NULL );
        mutation_t::mutate( res );

        return res;
    }
};

} // namespace reproduction

#endif  // REPRODUCTION_HPP_
