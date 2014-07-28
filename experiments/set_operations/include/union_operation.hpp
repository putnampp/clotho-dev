#ifndef UNION_OPERATION_HPP_
#define UNION_OPERATION_HPP_

#include <algorithm>
#include <string>

//template < class SIterator, class SetType, class ResultType = SetType >
//struct union_operation {
//    typedef SetType  set_type;
//    typedef ResultType  result_type;
//    typedef SIterator   sample_iterator;
//
//    void operator()( std::pair< sample_iterator, sample_iterator > paired, result_type & res ) {
//        typename set_type::element_type::iterator b1 = (*paired.first)->begin(),
//                                                  e1 = (*paired.first)->end(),
//                                                  b2 = (*paired.second)->begin(),
//                                                  e2 = (*paired.second)->end();
//        std::set_union( b1, e1, b2, e2, std::back_inserter( *res ) );
//    }
//
//    std::string name() {
//        static const std::string n = "union";
//        return n;
//    }
//};

//template < class SIterator, class V >
//struct union_operation< SIterator, std::shared_ptr< active_space_subset<V> >, std::shared_ptr< active_space_subset<V> > > {
//    typedef std::shared_ptr< active_space_subset<V> >  set_type;
//    typedef std::shared_ptr< active_space_subset<V> > result_type;
//    typedef SIterator   sample_iterator;
//
//    void operator()( std::pair< sample_iterator, sample_iterator > paired, result_type & res ) {
//        (*res) = (*(*paired.first));
//        (*res) |= (*(*paired.second));
//    }
//
//    std::string name() {
//        static const std::string n = "union";
//        return n;
//    }
//};

template < class SetType, class ResultType = SetType >
struct union_operation {
    typedef SetType set_type;
    typedef ResultType result_type;

    void operator()( const set_type & s1, const set_type & s2, result_type & res ) {
        std::set_union( s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter( res ) );
    }
};

#endif  // UNION_OPERATION_HPP_
