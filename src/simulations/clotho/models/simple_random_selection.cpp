#include "simple_random_selection.h"

SimpleRandomSelection::SimpleRandomSelection( shared_ptr< iRNG > rng ) : m_rng(rng) 
{}

selection_model * SimpleRandomSelection::clone() {
    selection_model * c = new SimpleRandomSelection( m_rng->clone() );
    return c;
}

system_id SimpleRandomSelection::find_mate( const system_id & id, const vector< system_id > & pool ) {
    unsigned int r = m_rng->nextInt( pool.size() );
    return pool[r];
}

unsigned int SimpleRandomSelection::find_mate_index( const system_id & id, const size_t pool_size ) {
    return m_rng->nextInt( pool_size );
}

SimpleRandomSelection::~SimpleRandomSelection() {
}
