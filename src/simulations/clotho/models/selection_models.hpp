#ifndef SELECTION_MODELS_HPP_
#define SELECTION_MODELS_HPP_

#include "selection.hpp"
#include "../clotho.h"

#include <utility>

namespace selection {
namespace models {

struct random_selection {
    typedef int parameter_t;
};

}   // namespace models

template < >
class MateSelector< models::random_selection > : public RandomProcess {
public:

//    template < class ENV >
    static std::pair< size_t, size_t > select( size_t nInd ) {
        std::pair< size_t, size_t > res;

        res.first = m_rng->nextInt( nInd );
        res.second = res.first;

        while( res.second  == res.first ) {
            res.second = m_rng->nextInt( nInd );
        }

        return res;
    }
/*
    template < class ENV, class ID = system_id >
    static std::pair< ID, ID > select( ENV * env, ID * ) {
        std::pair< ID, ID > res;

        if( env->m_gender_group_map.size() == 1 ) {
            size_t nInd = env->m_gender_group_map.begin()->second->size();

            size_t i0 = m_rng->nextInt( nInd );
            size_t i1 = i0;

            while( i1 == i0 ) {
                i1 = m_rng->nextInt( nInd );
            }

            res.first = env->m_gender_group_map.begin()->second->at( i0 );
            res.second = env->m_gender_group_map.begin()->second->at( i1 );
        } else {
            typename ENV::gender_group_map_t::iterator it_g0 = env->m_gender_group_map.begin();
            typename ENV::gender_group_map_t::iterator it_g1 = env->m_gender_group_map.begin();
            if( env->m_gender_group_map.size() == 2 ) {
                it_g1++;
            } else {
                size_t nGroups = env->m_gender_group_map.size();
                size_t g0 = m_rng->nextInt( nGroups );
                size_t g1 = g0;

                while( g1 == g0 ) {
                    g1 = m_rng->nextInt( nGroups );
                }
                while( g0 > 0 && g1 > 0) {
                    it_g0++;
                    it_g1++;
                    --g0;
                    --g1;
                }

                while( g0 > 0 ) {
                    it_g0++;
                    --g0;
                }

                while( g1 > 0 ) {
                    it_g1++;
                    --g1;
                }
            }
            assert(it_g0 != it_g1 && it_g0 != env->m_gender_group_map.end() && it_g1 != env->m_gender_group_map.end() );

            res.first = it_g0->second->at( m_rng->nextInt( it_g0->second->size() ));
            res.second =it_g1->second->at( m_rng->nextInt( it_g1->second->size() ));
        }

        return res;
    }
*/
protected:

    template < class ID >
    static std::pair< ID , ID > select( std::vector< ID > * env, ID ) {
        std::pair< ID, ID > res;

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

}   // namespace selection

#endif  // SELECTION_MODELS_HPP_
