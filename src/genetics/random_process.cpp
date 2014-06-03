#include "rng/random_process.hpp"

std::shared_ptr< iRNG > RandomProcess::m_rng;
bool RandomProcess::m_init = false;
