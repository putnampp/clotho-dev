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

#include "PopulationModel.h"
#include "../ClothoModelCoordinator.h"
//#include "SimulationManager.h"

#include <iostream>

using std::cout;
using std::endl;

using boost::static_pointer_cast;


PopulationModel::PopulationModel( int pop_size ) : m_living(0), m_lived(0), m_pop_size( pop_size ) {}

PopulationModel::~PopulationModel() {}

void PopulationModel::operator()( const BirthEvent * e, Individual * ind ) {
    ++m_living;
    if( (m_pop_size > -1) && ((int) m_living >= m_pop_size) ) {
        // if pop size is bounded and there are more people
        // than the bound
        // trigger termination?
    }
}

void PopulationModel::operator()( const DeathEvent * e, Individual * ind ) {
    --m_living;
    ++m_lived;
}

void PopulationModel::dump( ostream & out ) {
    // { living, lived, max_pop_size }
    out << "{ " << m_living << ", " << m_lived << ", " << m_pop_size << " }";
}
