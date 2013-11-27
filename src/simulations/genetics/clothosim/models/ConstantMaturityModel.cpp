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
#include "ConstantMaturityModel.h"

#include <iostream>

#include "../clothoobjects/events/MaturityEvent.h"
#include "../clothoobjects/events/ShellMaturityEvent.h"
#include "gsl/gsl_randist.h"

#include <time.h>

#include "IntVTime.h"

using std::cout;
using std::endl;

ConstantMaturityModel::ConstantMaturityModel( int age ) :
    m_age(age) { }

ConstantMaturityModel::~ConstantMaturityModel() { }

void ConstantMaturityModel::operator()(const BirthEvent * evt, Individual * ind ) {
    if(! evt ) return;


    IntVTime tConstantMaturity = dynamic_cast< const IntVTime & >( evt->getBirthTime() ) + m_age;
    Event * mEvent = new MaturityEvent( evt->getBirthTime(), tConstantMaturity, ind, ind );

    ind->receiveEvent( mEvent );
}

void ConstantMaturityModel::operator()(const ShellBirthEvent * evt, IndividualShell * ind ) {
    if(! evt ) return;

    IntVTime tConstantMaturity = *evt->getBirthTime() + m_age;
    Event * mEvent = new ShellMaturityEvent( evt->getReceiveTime(), tConstantMaturity, ind, ind );

    ind->receiveEvent( mEvent );
}

void ConstantMaturityModel::dump( ostream & out ) {

}
