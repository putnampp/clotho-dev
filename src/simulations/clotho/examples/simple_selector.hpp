#ifndef SIMPLE_SELECTOR_HPP_
#define SIMPLE_SELECTOR_HPP_

#include "rng/random_process.hpp"

template < class IndividualPointer, class EnvironmentPointer >
class SimpleSelector : public RandomProcess {
public:
    static std::pair< IndividualPointer , IndividualPointer > select( EnvironmentPointer env ) {
        std::pair< IndividualPointer, IndividualPointer > res;

        size_t nInd = env->size();

        size_t i0 = m_rng->nextInt( nInd );
        size_t i1 = i0;

        while( i1 == i0 ) {
            i1 = m_rng->nextInt( nInd );
        }

        res.first = env->at( i0 );
        res.second = env->at( i1 );

        return res;
    }
};
#endif  // SIMPLE_SELECTOR_HPP_
