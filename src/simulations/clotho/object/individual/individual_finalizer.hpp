#ifndef INDIVIDUAL_FINALIZER_HPP_
#define INDIVIDUAL_FINALIZER_HPP_

//#include "../finalizer.hpp"
#include "object/individual/config.hpp"

namespace finalizer {

class IndividualFinalizer {
public:
    template < typename IND >
    static void finalize( IND * ind ) { }

protected:
};

}   // namespace finalizer

#endif  // INDIVIDUAL_FINALIZER_HPP_
