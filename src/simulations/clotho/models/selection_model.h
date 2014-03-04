#ifndef SELECTION_MODEL_H_
#define SELECTION_MODEL_H_

#include "../clotho.h"

#include <vector>

using std::vector;

struct selection_model {
    virtual system_id find_mate( const system_id & p, const vector< system_id > & pool ) = 0;
};

#endif  // SELECTION_MODEL_H_
