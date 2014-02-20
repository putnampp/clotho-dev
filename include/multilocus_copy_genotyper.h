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

#ifndef MULTILOCUSCOPYGENOTYPE_H_
#define MULTILOCUSCOPYGENOTYPE_H_

#include "common.h"
#include "locus_copy_genotyper.h"

#include "AlleleCopyGroup.h"

template < ploidy_t P >
class MultilocusCopyGenotyper {
public:
    typedef AlleleCopyGroup< P >    allele_group;
    typedef allele_group::copy_t    allele_copy;
    typedef allele_group::copy_group_t allele_copy_group;
    typedef LocusCopyGenotyper< P > locus_genotyper;

    virtual bool   addIndexedGenotyper( size_t idx, locus_genotyper * lg ) = 0;
    virtual size_t getLociCount() const = 0;
    /*
     * compute the genotype an indexed set of allele_tuples
     */
    virtual double genotype( const allele_copy_group * t ) const = 0;

    virtual ~MultilocusCopyGenotyper() {}
protected:
    MultilocusCopyGenotyper( ) {}

};
#endif  // MULTILOCUSGENOTYPE_H_
