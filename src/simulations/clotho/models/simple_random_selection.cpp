#include "simple_random_selection.h"

#include <cstdlib>
#include <ctime>


SimpleRandomSelection::SimpleRandomSelection() {
    srand( time(NULL) );
}

system_id SimpleRandomSelection::find_mate( const system_id & id, const vector< system_id > & pool ) {
    int r = rand() % pool.size();
    return pool[r];
}

SimpleRandomSelection::~SimpleRandomSelection() {
}
