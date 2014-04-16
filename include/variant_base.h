#ifndef VARIANT_BASE_H_
#define VARIANT_BASE_H_

#include "genetic_region.h"

enum variant_flags { NEUTRAL = 1 };

class variant_base : public genetic_region {
public:
    variant_base( const chromosome_t & c, pos_t s, double cSelection, double cDominance, variant_flags f);
    variant_base( const chromosome_t & c, pos_t s, pos_t e, double cSelection, double cDominance, variant_flags f );

    bool isNeutral() const;
    unsigned int getPenetrance() const;

    double getSelection() const;
    double getDominance() const;

    virtual void incrementPenetrance();
    virtual void decrementPenetrance();

    virtual ~variant_base();

protected:
    inline bool check_flag( variant_flags f ) const { return (m_flags & f); }

    unsigned int  m_penetrance;

    double  m_coeff_selection, m_coeff_dominance;
    variant_flags m_flags;
};

#endif  // VARIANT_BASE_H_
