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

DEFINE_REGISTERED_CLOTHO_MODEL( PopulationModel )

const string POP_SIZE_K = "popsize";

#define DEFAULT_POP_SIZE 100

template <>
void ClothoModelCreator< PopulationModel >::createModel() {
    shared_ptr< PopulationModel> pm( new PopulationModel() );

    ClothoModelCoordinator< Individual, BirthEvent >::getInstance()->addEventHandler(
            static_pointer_cast< ClothoModel< Individual, BirthEvent > >( pm )  );

    ClothoModelCoordinator< Individual, DeathEvent >::getInstance()->addEventHandler(
            static_pointer_cast< ClothoModel< Individual, DeathEvent > >( pm ) ); 

}

template<>
void ClothoModelCreator< PopulationModel >::createModelFrom( const YAML::Node & n ) {
    shared_ptr< PopulationModel > pm( new PopulationModel() );
    pm->configure( n );

    ClothoModelCoordinator< Individual, BirthEvent >::getInstance()->addEventHandler(
            static_pointer_cast< ClothoModel< Individual, BirthEvent > >( pm )  );

    ClothoModelCoordinator< Individual, DeathEvent >::getInstance()->addEventHandler(
            static_pointer_cast< ClothoModel< Individual, DeathEvent > >( pm ) ); 
}

PopulationModel::PopulationModel() : m_living(0), m_lived(0), m_pop_size( DEFAULT_POP_SIZE ) {}

PopulationModel::~PopulationModel() {}

void PopulationModel::configure( const YAML::Node & n ) {
    if( n[ POP_SIZE_K ] ) {
        m_pop_size = n[ POP_SIZE_K ].as< unsigned int >();   
    }
}

void PopulationModel::operator()( const BirthEvent * e, const Individual * ind ) {
    cout << "Increasing population" << endl;
    if( ++m_living >= m_pop_size ) {
        // trigger termination?
    }
}

void PopulationModel::operator()( const DeathEvent * e, const Individual * ind ) {
    --m_living;
    ++m_lived;
}

void PopulationModel::dump( ostream & out ) {

}
