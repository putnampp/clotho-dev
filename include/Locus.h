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

#ifndef LOCUS_H_
#define LOCUS_H_

#include "common.h"
#include "Chromosome.h"
#include "Mutate.h"

#include <vector>

using std::vector;

/**
 * Implementation specific definition of a locus structure
 *
 * From a simulation perspective only the the pair <chromosome, position> are important. For extensibility reasons, position is
 * considered to be an open ended region [start, end)
 * All other fields are generally optional, depending upon simulation models.
 *
 * Other optional properties are:
 *  - properties (name, alleles, ... )
 *
 */
struct Locus {
    String      name;
    size_t      start, end;
    size_t      alt_count;
    mutrate_t   mutation_rate;
    shared_ptr< allele_t [] > alleles;
    allele_t    dominant_allele;
    chromid_t   chrom;
    ploidy_t    ploid;
};

typedef shared_ptr< Locus > LocusPtr;
typedef vector< LocusPtr > Loci;
typedef Loci::iterator  LocusIterator;

#endif  // LOCUS_H_
