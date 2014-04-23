#ifndef GENETIC_REGION_H_
#define GENETIC_REGION_H_

#include "genetic_mapping.h"

class genetic_region : public genetic_mapping {
public:
    genetic_region( key_t k );
    genetic_region( const chromosome_t & c, pos_t s );

    key_t   getKey() const;

    virtual bool operator==( const genetic_region & rhs ) const;
    virtual bool operator<( const genetic_region & rhs ) const;

    virtual ~genetic_region();
protected:
    key_t    m_key;
};
#endif  // GENETIC_REGION_H_

