#ifndef GENERIC_ALPHABET_HPP_
#define GENERIC_ALPHABET_HPP_

#include "genetics/alphabet/alphabet_def.hpp"

namespace grammar {
namespace alphabets {

#define BOILER_PLATE_TEMPLATE template < typename S, typename SA, typename SK, typename D, typename SR, typename TR >
#define BOILER_PLATE_SPECIALIZED_CLASS alphabet< S, SA, SK, D, SR, TR >
#define BOILER_PLATE_STRUCTOR( method ) BOILER_PLATE_TEMPLATE BOILER_PLATE_SPECIALIZED_CLASS::method
#define BOILER_PLATE_METHOD( t, m ) BOILER_PLATE_TEMPLATE t BOILER_PLATE_SPECIALIZED_CLASS::m




#undef BOILER_PLATE_TEMPLATE
#undef BOILER_PLATE_SPECIALIZED_CLASS
#undef BOILER_PLATE_STRUCTOR
#undef BOILER_PLATE_METHOD

}   // namespace alphabets {
}   // namespace grammar {

#endif  // GENERIC_ALPHABET_HPP_
