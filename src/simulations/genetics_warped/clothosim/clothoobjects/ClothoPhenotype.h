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
#ifndef CLOTHOPHENOTYPE_H_
#define CLOTHOPHENOTYPE_H_

#include "ClothoGeneticMap.h"

#include "EnvironmentProperties.h"

/*
 * A genotypable object will:
 * - given a locus index a micro genotype value can be calculated
 * - given a group of loci a macro genotype value can be calculated
 */
struct genotypeable {
    /*
    * Calculates the genotype at a specified locus
    */
    virtual double operator()( size_t ) = 0;

    /*
    * Calculates genotype for an indexed set of loci
    * and allelic_dominance measure
    */
    virtual double operator()( trait_locus_group_t ) = 0;
};

/*
 * Environmental factors are capable of impacting the phenotype
 * of an individual.
 *
 */
struct environmental_effect {
    virtual double operator()() = 0;
};

/*
 * A phenotypable object will:
 * - given a Genotypable object and an environmental_effect, a phenotype must be calculable
 *
 *   P = f( G, E ), where f : ( \real, \real ) -> \real
 *   G = g( L \intersect l ), where
 *     I = { i : 0 <= i < nLoci }
 *     L_{i \in I} = <a_0, ..., a_{P-1}>; a_{0 <= i < p} \in { ALLELES }, and P = nChromosomes (ploidy)
 *     L = \union_{i \in I} L_{i}
 *     l = { x: x \in I }
 *     L \intersect l = { L_{x}: x \in l}
 *     g: ( L ) -> \real
 *   E = e( ), where
 *     e: ( {} ) - > \real
 *
 */
struct phenotypable {
    virtual double operator()( genotypeable * , environment_effect * ) = 0;
};

#endif  // CLOTHOPHENOTYPE_H_

