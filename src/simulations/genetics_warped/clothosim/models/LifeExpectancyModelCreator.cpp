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

#include "LifeExpectancyModelCreator.h"
#include "../ClothoModelCoordinator.h"
#include "../clothoobjects/common_types.h"

#include "DistributionManager.h"

//const string DISTRIBUTION_K = "distribution";
//const string MEAN_K = "mean";
//const string STDEV_K = "stdev";

using boost::static_pointer_cast;

template <>
void ClothoModelCreator< LifeExpectancyModel, YAML::Node >::createModelFrom( const YAML::Node & n ) {
    shared_ptr< iDistribution > male;
    if( n[ MALE_K ] ) {
        YAML::Node tmp = n[MALE_K];
        /*        if( tmp[ MEAN_K ] ) {
                    male.mean = tmp[MEAN_K].as< double >();
                }
                if( tmp[ STDEV_K ] ) {
                    male.sigma = tmp[STDEV_K].as< double >();
                }
        */
        if( tmp[ DISTRIBUTION_K ] ) {
            string name = tmp[DISTRIBUTION_K].as<string>();
            male = DistributionManager< YAML::Node >::getInstance()->createDistribution( name, tmp );
        }
    }

    shared_ptr< iDistribution> female;
    if( n[ FEMALE_K ] ) {
        YAML::Node tmp = n[FEMALE_K];
        /*
                if( tmp[ MEAN_K ] ) {
                    female.mean = tmp[MEAN_K].as< double >();
                }
                if( tmp[ STDEV_K ] ) {
                    female.sigma = tmp[STDEV_K].as< double >();
                }
        */
        if( tmp[ DISTRIBUTION_K ] ) {
            string name = tmp[DISTRIBUTION_K].as<string>();
            female = DistributionManager< YAML::Node >::getInstance()->createDistribution( name, tmp );
        }
    }

    shared_ptr< iDistribution > unk;
    if( n[ UNKSEX_K ] ) {
        YAML::Node tmp = n[UNKSEX_K];
        /*        if( tmp[ MEAN_K ] ) {
                    unk.mean = tmp[MEAN_K].as< double >();
                }
                if( tmp[ STDEV_K ] ) {
                    unk.sigma = tmp[STDEV_K].as< double >();
                }
        */
        if( tmp[DISTRIBUTION_K] ) {
            string name = tmp[DISTRIBUTION_K].as<string>();
            unk = DistributionManager< YAML::Node >::getInstance()->createDistribution( name, tmp );
        }
    }
    shared_ptr< LifeExpectancyModel > pm( new LifeExpectancyModel( female, male, unk) );

    ClothoModelCoordinator< Individual, BirthEvent >::getInstance()->addEventHandler(
        static_pointer_cast< ClothoModel< Individual, BirthEvent> >(pm) );

    ClothoModelCoordinator< IndividualShell, ShellBirthEvent >::getInstance()->addEventHandler(
        static_pointer_cast< ClothoModel< IndividualShell, ShellBirthEvent > >(pm) );
}

DEFINE_REGISTERED_CLOTHO_MODEL( LifeExpectancyModel, YAML::Node )
