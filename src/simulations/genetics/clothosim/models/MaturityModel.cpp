/*******************************************************************************
 * Copyright (c) 2013, Patrick P. Putnam (putnampp@gmail.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 ******************************************************************************/

#include "../clothoobjects/common_types.h"
#include "MaturityModel.h"

#include <iostream>

#include "../clothoobjects/events/MaturityEvent.h"
#include "../clothoobjects/events/ShellMaturityEvent.h"
#include "gsl/gsl_randist.h"

#include <time.h>

#include "IntVTime.h"

using std::cout;
using std::endl;

MaturityModel::MaturityModel( distribution_params & female, distribution_params & male, distribution_params & unk) :
    m_rng( gsl_rng_alloc( gsl_rng_taus ) ), m_female(female), m_male(male), m_unk(unk) {
    long seed = time(NULL);
    gsl_rng_set( m_rng, seed );
}

MaturityModel::~MaturityModel() {
    gsl_rng_free( m_rng );
}

void MaturityModel::operator()(const BirthEvent * evt, Individual * ind ) {
    if(! evt ) return;

    double expected_age = computeExpectedAge( evt->getSex() );

    IntVTime tMaturity = dynamic_cast< const IntVTime & >( evt->getBirthTime() ) + (int)expected_age;
    Event * mEvent = new MaturityEvent( evt->getBirthTime(), tMaturity, ind, ind );

    ind->receiveEvent( mEvent );
}

void MaturityModel::operator()(const ShellBirthEvent * evt, IndividualShell * ind ) {
    if(! evt ) return;

    double expected_age = computeExpectedAge( ind->getSex() );

    IntVTime tMaturity = *evt->getBirthTime() + (int)expected_age;
    cout << "Next maturity: " << tMaturity << " (" << evt->getReceiveTime() << ")" << endl;
    Event * mEvent = new ShellMaturityEvent( evt->getReceiveTime(), tMaturity, ind, ind );

    ind->receiveEvent( mEvent );
}

double MaturityModel::computeExpectedAge( sex_t s ) {
    double expected_age = 0.0;

    switch( s ) {
    case FEMALE:
        expected_age = gsl_ran_gaussian( m_rng, m_female.sigma );
        expected_age += m_female.mean;
        break;
    case MALE:
        expected_age = gsl_ran_gaussian( m_rng, m_male.sigma );
        expected_age += m_male.mean;
        break;
    default:
        expected_age = gsl_ran_gaussian( m_rng, m_unk.sigma );
        expected_age += m_unk.mean;
        break;
    };

    return expected_age;
}

void MaturityModel::dump( ostream & out ) {

}

