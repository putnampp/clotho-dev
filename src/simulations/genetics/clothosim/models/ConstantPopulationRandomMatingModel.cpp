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

#include "ConstantPopulationRandomMatingModel.h"
#include <iostream>

#include <time.h>

#include <cmath>
#include <algorithm>

#include "../clothoobjects/events/ShellBirthEvent.h"

#include "ConstantDistribution.h"

using std::cout;
using std::endl;
using std::random_shuffle;

/*
ConstantPopulationRandomMatingModel::ConstantPopulationRandomMatingModel( unsigned int max_offspring, unsigned int birth_delay ) : 
    m_rng( gsl_rng_alloc( gsl_rng_taus ) ),
    m_birth_delay( birth_delay ),
    m_offspring_dist( new ConstantDistribution(max_offspring) ),
    m_rand_val(0),
    m_offset(0) {
    long seed = time(NULL);
    gsl_rng_set( m_rng, seed );
}
*/

ConstantPopulationRandomMatingModel::ConstantPopulationRandomMatingModel( shared_ptr< iDistribution > offspring, shared_ptr< iDistribution> birth_delay ) :
    //m_rng( gsl_rng_alloc( gsl_rng_mt19937 ) ),
    m_offspring_dist( offspring ),
    m_birth_delay( birth_delay )
{
//    long seed = time(NULL);
//    gsl_rng_set( m_rng, seed );

//    m_uniform.setRandomNumberGenerator( m_rng );
    m_rng.set_seed_time();
}

void ConstantPopulationRandomMatingModel::operator()( const ShellMaturityEvent * e, IndividualShell * ind ) {
    if( ind->getSex() == FEMALE ) {
        // females notifies environment when ready to mate
        //
//        IntVTime date = dynamic_cast< const IntVTime & >( e->getReceiveTime() );

        Environment2 * env = ind->getEnvironment();

        // how many children should the female have?
        double nOff = floor( m_offspring_dist->nextVariate() );
        
        for( ; nOff >= 0.0; nOff -= 1.0) {
            // generate an Mating event for each child
            // if there are available individuals in the pool
            if( env->hasAvailableIndividuals() ) {
                Event * evt = new ShellMatingEvent( e->getReceiveTime(), e->getReceiveTime(), ind, env, ind );
                env->receiveEvent( evt );
            } else { break; }
        }
    }
}

void ConstantPopulationRandomMatingModel::operator()( const ShellMatingEvent * e, Environment2 * env ) {
    if( !env->hasAvailableIndividuals() ) {
        //cout << "pool empty" << endl;
        return;
    }

//    int nMales = env->getMaleCount();

    // get a random male by index
//    unsigned int rMale = m_uniform.nextVariate( nMales );
//    unsigned int rMale = m_rng.Uniform( env->getMaleCount() );

    IndividualShell * female = e->getFirstPartner();
    IndividualShell * male = env->getMaleAt( m_rng.Uniform( env->getMaleCount() ) );

    if( female && female->isAlive() ) {
        if( male && male->isAlive() ) {
            IndividualShell * offspring = env->nextAvailableIndividual();

            if( offspring ) {
                IndividualProperties * ip = offspring->getProperties();
                ip->reset();

                generateOffspringGenotype( female, male, ip->m_genos );

                IntVTime t = *dynamic_cast< IntVTime * > (e->getReceiveTime().clone() ) + (int) m_birth_delay->nextVariate();

                ip->m_dob = new IntVTime( t );
//                ip->m_sex = (m_uniform.nextBoolean() ? MALE : FEMALE );
                ip->m_sex = (m_rng.Bool() ? MALE : FEMALE );

                ip->setMother(female->getProperties());
                ip->setFather(male->getProperties());

                female->addOffspring();
                male->addOffspring();

                Event * eBorn = new ShellBirthEvent( e->getReceiveTime(), *ip->m_dob, offspring, offspring );
                offspring->receiveEvent( eBorn );
            }
        } else {
            cout << "Dead MALE" << endl;
        }
    } else {
        cout << "Dead female" << endl;
    }
}

void ConstantPopulationRandomMatingModel::dump( ostream & out ) {

}

void ConstantPopulationRandomMatingModel::generateOffspringGenotype( IndividualShell * female, IndividualShell * male, AlleleGroupPtr genos ) {
    unsigned int nLoci = female->getEnvironmentLociCount();

    resizeAlleleGroup( genos, nLoci );

    AlleleGroupPtr female_alleles = female->getProperties()->m_genos;
    AlleleGroupPtr male_alleles = male->getProperties()->m_genos;

    assert( male_alleles->size() == nLoci && female_alleles->size() == nLoci );

    if( ALLELE_COPIES == 2 ) {
        for( unsigned int i = 0; i < nLoci; ++i ) {
/*
 * Code works. However, serves as a significant bottleneck in performance
 */
            // routine only works if ploidy == 2
//            (*genos)[i][0] = (*female_alleles)[ i ][m_rng.Bool()];
//            (*genos)[i][1] = (*male_alleles)[ i ][m_rng.Bool()];
            
/*
 * Alternative method. Fewer random number generations
 */
/*
            unsigned int rnd = m_rng.Uniform( 4 );
            switch( rnd ) {
            case 0:
                (*genos)[i][0] = (*female_alleles)[ i ][0];
                (*genos)[i][1] = (*male_alleles)[ i ][0];
                break;
            case 1:
                (*genos)[i][0] = (*female_alleles)[ i ][0];
                (*genos)[i][1] = (*male_alleles)[ i ][1]; 
                break;
            case 2:
                (*genos)[i][0] = (*female_alleles)[ i ][1];
                (*genos)[i][1] = (*male_alleles)[ i ][0]; 
                break;
            default:
                (*genos)[i][0] = (*female_alleles)[ i ][1];
                (*genos)[i][1] = (*male_alleles)[ i ][1]; 
                break;
            }   // end switch
*/

            double rnd = m_rng.Uniform();
/*
            if( rnd < 0.5 ) {
                if( rnd < 0.25 ) {
                    (*genos)[i][0] = (*female_alleles)[ i ][0];
                    (*genos)[i][1] = (*male_alleles)[ i ][0];
                } else {
                    (*genos)[i][0] = (*female_alleles)[ i ][0];
                    (*genos)[i][1] = (*male_alleles)[ i ][1]; 
                }
            } else if( rnd < 0.75 ) {
                (*genos)[i][0] = (*female_alleles)[ i ][1];
                (*genos)[i][1] = (*male_alleles)[ i ][0]; 
            } else {
                (*genos)[i][0] = (*female_alleles)[ i ][1];
                (*genos)[i][1] = (*male_alleles)[ i ][1]; 
            }
*/
            (*genos)[i][0] = (*female_alleles)[i][ (rnd < 0.5) ];
            (*genos)[i][1] = (*male_alleles)[i][ ((0.25 <= rnd) && (rnd < 0.75)) ];
        } // end loop 
    } else {
        // more general outline. Note that it assumes a parent allele
        // can serve as the variant source multiple times.
        unsigned int from_male = ALLELE_COPIES / 2;

        vector< ploidy_t > males;
        vector< ploidy_t > females;

        for( ploidy_t i = 0; i < ALLELE_COPIES; ++i ) {
            males.push_back( i );
            females.push_back( i );
        }

        for( unsigned int i = 0; i < nLoci; ++i ) {
            // shuffle the chromosomes for each locus?
            // treat each locus as being independent of one another
            random_shuffle( males.begin(), males.end() );
            random_shuffle( females.begin(), females.end() );
            unsigned int k = 0;
            for( ; k < from_male; ++k ) {
                (*genos)[i][ k ] = (*male_alleles)[i][ males[k] ];
            }
            for( ; k < ALLELE_COPIES; ++k ) {
                (*genos)[i][ k ] = (*female_alleles)[ i ][ females[k - from_male] ];
            }
        }
    }
}

ConstantPopulationRandomMatingModel::~ConstantPopulationRandomMatingModel() {
    //gsl_rng_free( m_rng );
    //if( m_offspring_dist ) {
    //    m_offspring_dist.reset();
    //}
}
