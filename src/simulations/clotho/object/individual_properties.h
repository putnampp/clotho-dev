#ifndef INDIVIDUAL_PROPERTIES_H_
#define INDIVIDUAL_PROPERTIES_H_

#include "../clotho.h"

struct IndividualProperties {
    system_id mother, father;
    system_id population;

    SystemClock::vtime_t dob, eol;
    Sex   sex;
    AlleleGroupPtr genotypes;

    IndividualProperties( ) :
        mother(0),
        father(0),
        dob( SystemClock::getPositiveInfinity() ),
        eol( SystemClock::getPositiveInfinity() ),
        sex( UNASSIGNED ),
        genotypes( NULL )
    {}

    void inheritFromFather( AlleleGroupPtr genos );
    void inheritFromMother( AlleleGroupPtr genos );
};

#endif  // INDIVIDUAL_PROPERTIES_H_
