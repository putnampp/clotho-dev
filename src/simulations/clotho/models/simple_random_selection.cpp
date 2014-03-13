#include "simple_random_selection.h"


SimpleRandomSelection::SimpleRandomSelection( shared_ptr< iRNG > rng ) : m_rng(rng) 
{}

system_id SimpleRandomSelection::find_mate( const system_id & id, const vector< system_id > & pool ) {
    int r = m_rng->nextInt(pool.size());
    return pool[r];
}

SimpleRandomSelection::~SimpleRandomSelection() {
}
