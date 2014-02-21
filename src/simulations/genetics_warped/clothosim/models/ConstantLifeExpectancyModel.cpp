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
#include "ConstantLifeExpectancyModel.h"

#include <iostream>

#include "../clothoobjects/events/DeathEvent.h"
#include "../clothoobjects/events/ShellDeathEvent.h"
#include "gsl/gsl_randist.h"

#include <time.h>

#include "IntVTime.h"

using std::cout;
using std::endl;

ConstantLifeExpectancyModel::ConstantLifeExpectancyModel( int age ) :
    m_age(age) { }

ConstantLifeExpectancyModel::~ConstantLifeExpectancyModel() { }

void ConstantLifeExpectancyModel::operator()( const BirthEvent * evt, Individual * ind ) {
    if(! evt ) return;

    IntVTime tDeath = dynamic_cast< const IntVTime & >( evt->getBirthTime() ) + (int)m_age;
    Event * dEvent = new DeathEvent( evt->getBirthTime(), tDeath, ind, ind );

    ind->receiveEvent( dEvent );
}

void ConstantLifeExpectancyModel::operator()( const ShellBirthEvent * evt, IndividualShell * ind ) {
    if(! evt ) return;

    IntVTime tDeath = *evt->getBirthTime() + (int)m_age;
    Event * dEvent = new ShellDeathEvent( *evt->getBirthTime(), tDeath, ind, ind, ind );

    ind->receiveEvent( dEvent );
}

void ConstantLifeExpectancyModel::dump( ostream & out ) {

}