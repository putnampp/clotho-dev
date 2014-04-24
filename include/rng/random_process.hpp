#ifndef RANDOM_PROCESS_HPP_
#define RANDOM_PROCESS_HPP_

#include "rng.hpp"
#include <memory>

class RandomProcess {
public:
    static void initialize( std::shared_ptr< iRNG > r ) {
        if( m_init ) return;
        reinitialize( r );
    }

    static std::shared_ptr< iRNG > getRNG() {
        assert( m_rng );
        return m_rng;
    }
protected:
    static void reinitialize( std::shared_ptr< iRNG > r ) {
        m_rng = r;
        m_init = true;
    }

    static std::shared_ptr< iRNG > m_rng;
    static bool m_init;
};

#endif  // RANDOM_PROCESS_HPP_
