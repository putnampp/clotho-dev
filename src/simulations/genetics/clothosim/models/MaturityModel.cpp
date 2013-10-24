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
#include "../ClothoModelCoordinator.h"
#include "SimulationManager.h"

#include <iostream>

#include "../clothoobjects/events/MaturityEvent.h"
#include "gsl/gsl_randist.h"

#include <time.h>

#include "IntVTime.h"

using std::cout;
using std::endl;

const string DISTRIBUTION_K = "distribution";
const string MEAN_K = "mean";
const string STDEV_K = "stdev";

DEFINE_REGISTERED_CLOTHO_MODEL( MaturityModel )

template <>
ClothoModel * ClothoModelCreator< MaturityModel >::createModel() {
    shared_ptr< MaturityModel> pm( new MaturityModel() );
    shared_ptr<ClothoModelCoordinator> coord = ClothoModelCoordinator::getInstance();

    coord->addEventHandler( evt_BirthEvent.getDataType(), pm );

    return &*pm;
}

template<>
ClothoModel * ClothoModelCreator< MaturityModel >::createModelFrom( const YAML::Node & n ) {
    shared_ptr< MaturityModel > pm( new MaturityModel() );
    pm->configure( n );
    shared_ptr<ClothoModelCoordinator> coord = ClothoModelCoordinator::getInstance();

    coord->addEventHandler( evt_BirthEvent.getDataType(), pm );

    return &*pm;
}

MaturityModel::MaturityModel() : m_rng( gsl_rng_alloc( gsl_rng_taus ) ) {
    long seed = time(NULL);
    gsl_rng_set( m_rng, seed );
}

MaturityModel::~MaturityModel() {}

void MaturityModel::configure( const YAML::Node & n ) {
    if( n[ MALE_K ] ) {
        YAML::Node tmp = n[MALE_K];
        if( tmp[ MEAN_K ] ) {
            m_male_mean = tmp[MEAN_K].as< double >();
        }
        if( tmp[ STDEV_K ] ) {
            m_male_sigma = tmp[STDEV_K].as< double >();
        }
    }

    if( n[ FEMALE_K ] ) {
        YAML::Node tmp = n[FEMALE_K];
        if( tmp[ MEAN_K ] ) {
            m_female_mean = tmp[MEAN_K].as< double >();
        }
        if( tmp[ STDEV_K ] ) {
            m_female_sigma = tmp[STDEV_K].as< double >();
        }
    }
}

void MaturityModel::handle( const Event * evt ) {
    const string name = evt->getDataType();

    if( name == evt_BirthEvent.getDataType() ) {
        const BirthEvent * bEvt = dynamic_cast< const BirthEvent * >( evt );
        handle( bEvt );
    }
}

void MaturityModel::handle( const BirthEvent * evt ) {
    if(! evt ) return;

    double expected_age = 0.0;

    switch( evt->getSex() ) {
    case FEMALE:
        expected_age = gsl_ran_gaussian( m_rng, m_female_sigma );
        expected_age += m_female_mean;
        break;
    case MALE:
        expected_age = gsl_ran_gaussian( m_rng, m_male_sigma );
        expected_age += m_male_mean;
        break;
    default:
        expected_age = gsl_ran_gaussian( m_rng, m_unk_sigma );
        expected_age += m_unk_mean;
        break;
    };

    IntVTime tMaturity = dynamic_cast< const IntVTime & >( evt->getBirthTime() ) + (int)expected_age;
    Event * mEvent = new MaturityEvent( evt->getBirthTime(), tMaturity, evt->getSender(), evt->getSender(), evt->getEventId().getEventNum() );

    ClothoModelCoordinator::getInstance()->routeEvent( mEvent );
}

void MaturityModel::dump( ostream & out ) {

}

