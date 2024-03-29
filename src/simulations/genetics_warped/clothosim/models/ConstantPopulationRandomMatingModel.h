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

#ifndef CONSTANTPOPULATIONRANDOMMATINGMODEL_H_
#define CONSTANTPOPULATIONRANDOMMATINGMODEL_H_

#include "../ClothoModel.h"
#include "Distribution.h"

#include "../clothoobjects/events/ShellMaturityEvent.h"
#include "../clothoobjects/events/ShellMatingEvent.h"

#include "gsl/gsl_rng.h"

#include "random.h"
//#include "QuantiNemoRandomAlleleInheritance.h"
#include "RandomChromosomalInheritance.h"
//#include "pword_allele_masks.hpp"
//#include "UniformDistribution.h"
//

class ConstantPopulationRandomMatingModel :
    virtual public ClothoModel< IndividualShell, ShellMaturityEvent >,
    virtual public ClothoModel< Environment2 , ShellMatingEvent > {
public:
//    ConstantPopulationRandomMatingModel (unsigned int max_offspring, unsigned int birth_delay);
    ConstantPopulationRandomMatingModel( shared_ptr< iDistribution > offspring, shared_ptr< iDistribution > birth_delay );

    void operator()( const ShellMaturityEvent * e, IndividualShell * ind );
    void operator()( const ShellMatingEvent * e, Environment2 * env );
    void dump( ostream & out );

    virtual ~ConstantPopulationRandomMatingModel();
protected:
//    void generateOffspringGenotype( IndividualShell * female, IndividualShell * male, AlleleGroupPtr genos );

//    gsl_rng * m_rng;
//    unsigned int m_birth_delay;
//    distribution_params m_offspring_dist;
    shared_ptr< iDistribution >     m_offspring_dist;
    shared_ptr< iDistribution >     m_birth_delay;

//    UniformDistribution     m_uniform;
//
    RAND m_rng;

//    RandomAlleleInheritance< RAND, ALLELE_COPIES > m_inheritance;
    ChromosomalInheritance< RAND, ALLELE_COPIES > m_inheritance;

//    static pword_allele_mask< pword_t, allele_t, ALLELE_COPIES > m_pword_masks;

private:
    unsigned int m_rand_val, m_offset;
};

#endif  // CONSTANTPOPULATIONRANDOMMATINGMODEL_H_
