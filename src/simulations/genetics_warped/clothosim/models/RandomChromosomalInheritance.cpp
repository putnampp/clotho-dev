#include "RandomChromosomalInheritance.h"
/*
template <>
void ChromosomalInheritance< RAND, 2 >::inherit( AlleleGroupPtr p0, AlleleGroupPtr p1, AlleleGroupPtr offspring ) {
    assert( m_genetic_map );
    size_t prev_offset = 0; 
    for(GeneticMap::chrom_offset_map_t::const_iterator it = m_genetic_map->begin_offsets(); it != m_genetic_map->end_offsets(); it++ ) {
        // copy from p0
        double rnd = m_rng->Uniform();

        memcpy( offspring[0] + prev_offset, p0[(rnd < 0.5)] + prev_offset, (it->second - prev_offset) * sizeof( allele_t ) );
        memcpy( offspring[1] + prev_offset, p1[((0.25<= rnd ) &&(rnd < 0.75))] + prev_offset, (it->second - prev_offset) * sizeof( allele_t ) );
        prev_offset = it->second;
    }
}

typedef ChromosomalInheritance< RAND, 2 > RandomChromosomalInheritance;
*/
