#ifndef SIMPLE_RANDOM_SELECTION_H_
#define SIMPLE_RANDOM_SELECTION_H_

#include "selection_model.h"
#include "rng/rng.hpp"

class SimpleRandomSelection : public selection_model {
public:
    SimpleRandomSelection( shared_ptr< iRNG > rng );
    system_id find_mate( const system_id & id, const vector< system_id > & pool );

    virtual ~SimpleRandomSelection();
protected:
    shared_ptr< iRNG >  m_rng;
};

#endif  // SIMPLE_RANDOM_SELECTION_H_
