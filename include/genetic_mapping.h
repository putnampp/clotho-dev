#ifndef GENETIC_MAPPING_H_
#define GENETIC_MAPPING_H_

#include <string>
#include <unordered_map>
#include <deque>
#include <memory>

/*
 * Case-sensitive mapping of chromosome names
 * to an ID
 *
 * By default, the first chromosome name provided
 * will be the name used for all aliases
 *
 */
class genetic_mapping {
public:
    typedef std::string     chromosome_t;
    typedef unsigned int    chrom_id_t;
    typedef unsigned int    pos_t;
    typedef double          key_t;
    typedef std::deque< chromosome_t > chromosome_list_t;
    typedef std::unordered_map< chromosome_t, chrom_id_t > chromosome_map_t;

    static void addChromosome( const chromosome_t & c );
    static void addChromosomeAlias( const chromosome_t & c, const chromosome_t & a );

    static chrom_id_t   getChromosomeID( const chromosome_t & c );
    static chromosome_t  getChromosome( chrom_id_t c );

    static std::shared_ptr< chromosome_list_t > getAllChromosomes( const chromosome_t & c );
    static std::shared_ptr< chromosome_list_t > getAllChromosomes( chrom_id_t c );

    static key_t  getKey( const chromosome_t & c, pos_t p );

    virtual ~genetic_mapping();

protected:
    static chromosome_map_t     m_chrom_lookup;
    static chromosome_list_t    m_chroms;
};

#endif  // GENETIC_MAPPING_H_
