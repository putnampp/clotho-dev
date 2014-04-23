#ifndef REPRODUCTION_MODELS_HPP_
#define REPRODUCTION_MODELS_HPP_

#include "reproduction.hpp"

#include "../clotho.h"
#include "variant_map.hpp"

namespace reproduction {
namespace models {

namespace mutation {

template < class V, class VM = variant_map< V >, class GM = typename VM::gamete_t >
struct infinite_site {
    typedef V   variant_type;
    typedef VM  variant_map_t;
    typedef GM  gamete_t;

    static std::shared_ptr< variant_map_t > getMap() {
        static std::shared_ptr< variant_map_t > instance( new variant_map_t() );
        return instance;
    }
};

}   // namespace mutation

namespace recombination {

struct simple_recomb {};

}   // namespace recombination

template < class VM,
    class MUT = mutation::infinite_site< VM >, 
    class REC = recombination::simple_recomb >
struct standard_reproduction {
    typedef VM variant_map_t;
    typedef MUT mutation_t;
    typedef REC recombination_t;

    variant_map_t * vm;
};

}   // namespace models
}   // namespace reproduction

#endif  // REPRODUCTION_MODELS_HPP_
