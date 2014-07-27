#ifndef ITERATOR_HELPER_HPP_
#define ITERATOR_HELPER_HPP_

namespace utility {

template < class I >
struct iterator_helper {
    typedef I::iterator         type;
    typedef I::const_iterator   ctype;  
};

}   // namespace utility

#endif  // ITERATOR_HELPER_HPP_
