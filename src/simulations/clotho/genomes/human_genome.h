#ifndef HUMAN_GENOME_H_
#define HUMAN_GENOME_H_

#include "../clotho.h"
#include "genome.h"

#include <vector>

using std::vector;

#include "human_gamete.h"

class HumanGenome : public genome {
public:
    HumanGenome( HumanGamete * hz0, HumanGamete * hz1 );
    HumanGenome( const HumanGenome & );

    genome * clone() const;

    inline gamete::gamete_source_t getSource() const {
        return ((m_sex == MALE) ? HumanGamete::FROM_FATHER : HumanGamete::FROM_MOTHER );
    }

    inline gamete::gamete_source_t getMaxSources() const {
        return 2;
    }

    Sex getSex() const;

    gamete * getGamete( gamete::gamete_source_t ) const;
    gamete * getGamete( Sex ) const;

    void setGamete( gamete * );
    void setGamete( HumanGamete * );

    virtual ~HumanGenome();
protected:
    HumanGenome();
    HumanGamete * m_mother, * m_father;
    Sex m_sex;
};

#endif  // HUMAN_GENOME_H_
