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
#include "cchmc/DefaultTrait.h"

DefaultTrait::DefaultTrait( const string & name ) : 
    Trait( name ), 
    m_loci( new LocusGenotypers() ) 
{}

DefaultTrait::~DefaultTrait() {
    m_loci->clear();
    m_loci.reset();
}

bool DefaultTrait::addIndexedGenotyper( size_t locus_idx, LocusGenotyper * lg ) {
    (*m_loci)[ locus_idx ] = lg;
    return true;
}

size_t  DefaultTrait::getLociCount() const {
    return m_loci->size();
}

/*
 *
 *  \sum_{i \in DefaultTraitLoci} genotype( Loci_{i} )
 *  \sum_{i} G_{i}
 *
 */
double DefaultTrait::genotype( const AlleleGroupPtr ag ) const {
    double res = 0.0;
    for( LocusGenotypers::const_iterator it = m_loci->begin(); it != m_loci->end(); it++ ) {
        res += it->second->genotype( ag, it->first );
    }
    return res;
}

/*
 * P = G + E
 */
double DefaultTrait::phenotype( const AlleleGroupPtr ag, const environmental * env ) const {
    double res = env->environment_factor();

    res += genotype( ag );

    return res;
}
