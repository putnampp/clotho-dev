#ifndef SIMPLE_RANDOM_SELECTION_H_
#define SIMPLE_RANDOM_SELECTION_H_

#include "selection_model.h"

class SimpleRandomSelection : public selection_model {
public:
    SimpleRandomSelection();
    system_id find_mate( const system_id & id, const vector< system_id > & pool );

    virtual ~SimpleRandomSelection();
};

#endif  // SIMPLE_RANDOM_SELECTION_H_
