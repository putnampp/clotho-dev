#include "genetics/genetic_mapping.h"
#include <cassert>

const double genetic_mapping::MAX_CHROM_LENGTH = 3.0e8;

genetic_mapping::chromosome_map_t   genetic_mapping::m_chrom_lookup;
genetic_mapping::chromosome_list_t       genetic_mapping::m_chroms;


template <> 
double genetic_mapping::convertToKey<double>( const chromosome_t & c, pos_t p ) {
    chromosome_map_t::iterator it = m_chrom_lookup.find(c);

    assert( it != m_chrom_lookup.end() );
    double res = (((double) it->second) / ((double) m_chroms.size()));

    res += ((double)p) / MAX_CHROM_LENGTH;

    return res;
}

void genetic_mapping::addChromosome( const chromosome_t & c ) {
    if( genetic_mapping::m_chrom_lookup.find(c) == genetic_mapping::m_chrom_lookup.end() ) {
        genetic_mapping::m_chrom_lookup.insert( std::make_pair(c, genetic_mapping::m_chroms.size()));
        genetic_mapping::m_chroms.push_back( c );
    } 
}

void genetic_mapping::addChromosomeAlias( const chromosome_t & c, const chromosome_t & a ) {
    typename genetic_mapping::chromosome_map_t::iterator it = genetic_mapping::m_chrom_lookup.find(c);
    typename genetic_mapping::chromosome_map_t::iterator it2 = genetic_mapping::m_chrom_lookup.find(a);

    if( it == genetic_mapping::m_chrom_lookup.end() ) {
        // chromosome id 1 not found
        if( it2 == genetic_mapping::m_chrom_lookup.end() ) {
            // both ids are new
            genetic_mapping::m_chrom_lookup.insert( std::make_pair( c, genetic_mapping::m_chroms.size() ));
            genetic_mapping::m_chrom_lookup.insert( std::make_pair( a, genetic_mapping::m_chroms.size() ));
            genetic_mapping::m_chroms.push_back(c);
        } else {
            // chromosome id 2 is found. using that id for
            // chromosome id 1
            genetic_mapping::m_chrom_lookup.insert( std::make_pair( c, it2->second ) );
        }
    } else if( it2 == genetic_mapping::m_chrom_lookup.end() ) {
        // c found, a not found
        genetic_mapping::m_chrom_lookup.insert( std::make_pair(a, it->second ));
    } else {
        // both chromosomes exist; assert that they already
        // map to the same id
        // Basically, disallow the user from re-mapping chromosomes
        assert( it->second == it2->second );
    }
}

genetic_mapping::chrom_id_t genetic_mapping::getChromosomeID( const genetic_mapping::chromosome_t & c ) {
    genetic_mapping::chromosome_map_t::iterator it = genetic_mapping::m_chrom_lookup.find( c );

    assert( it != genetic_mapping::m_chrom_lookup.end() );

    return it->second;
}

genetic_mapping::chromosome_t genetic_mapping::getChromosome( chrom_id_t c ) {
    assert( c < m_chroms.size() );
    return m_chroms[c];
}

std::shared_ptr< genetic_mapping::chromosome_list_t > genetic_mapping::getAllChromosomes( const chromosome_t & c ) {
    genetic_mapping::chromosome_map_t::iterator it = genetic_mapping::m_chrom_lookup.find( c );
    assert( it != genetic_mapping::m_chrom_lookup.end() );

    return getAllChromosomes( it->second );
}

std::shared_ptr< genetic_mapping::chromosome_list_t > genetic_mapping::getAllChromosomes( chrom_id_t c ) {
    std::shared_ptr< genetic_mapping::chromosome_list_t > res(new genetic_mapping::chromosome_list_t() );

    genetic_mapping::chromosome_map_t::iterator it = genetic_mapping::m_chrom_lookup.begin();

    while( it != genetic_mapping::m_chrom_lookup.end() ) {
        if( it->second == c ) {
            res->push_back( it->first );
        }
        it++;
    }

    return res;
}

genetic_mapping::~genetic_mapping() {}
