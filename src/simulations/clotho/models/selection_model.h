#ifndef SELECTION_MODEL_H_
#define SELECTION_MODEL_H_

#include "../clotho.h"

#include <vector>

using std::vector;

struct selection_model {
    virtual selection_model * clone() = 0;

    virtual system_id find_mate( const system_id & p, const vector< system_id > & pool ) = 0;

    virtual unsigned int find_mate_index( const system_id & p, const size_t pool_size ) = 0;

    virtual ~selection_model() {}
};

#endif  // SELECTION_MODEL_H_
