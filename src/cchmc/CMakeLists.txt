################################################################################
# Copyright (c) 2013, Patrick P. Putnam (putnampp@gmail.com)
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met: 
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer. 
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution. 
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are those
# of the authors and should not be interpreted as representing official policies,
# either expressed or implied, of the FreeBSD Project.
################################################################################
INCLUDE_DIRECTORIES( ${${PROJECT_NAME}_INCLUDE_DIR} )

#LIST( APPEND SRCS StatisticFactory.cpp)
#LIST( APPEND SRCS GenomeFactory.cpp)
#LIST( APPEND SRCS ChromosomeMap.cpp)
#LIST( APPEND SRCS Chromosome.cpp)
#LIST( APPEND SRCS Sequence.cpp)
#LIST( APPEND SRCS Individual.cpp)
#LIST( APPEND SRCS ChromosomeCopies.cpp)
#LIST( APPEND SRCS ChromosomeSegments.cpp)
#LIST( APPEND SRCS AncestralChromosome.cpp)
#LIST( APPEND SRCS MutatedChromosome.cpp)
#LIST( APPEND SRCS InsertedChromosome.cpp)
#LIST( APPEND SRCS DeletedChromosome.cpp)
#LIST( APPEND SRCS GeneticEventManager.cpp)

LIST( APPEND CCHMC_SRCS pword_allele_masks.cpp)
LIST( APPEND CCHMC_SRCS AlleleTuple.cpp )
LIST( APPEND CCHMC_SRCS GeneticMap.cpp )
LIST( APPEND CCHMC_SRCS Locus.cpp )
LIST( APPEND CCHMC_SRCS DefaultTrait.cpp )
LIST( APPEND CCHMC_SRCS DefaultAllelicEffect.cpp )
LIST( APPEND CCHMC_SRCS DefaultLocusGenotyper.cpp )
LIST( APPEND CCHMC_SRCS genetic_mapping.cpp )
LIST( APPEND CCHMC_SRCS genetic_region.cpp )
LIST( APPEND CCHMC_SRCS variant_base.cpp )

ADD_LIBRARY(cchmc SHARED ${CCHMC_SRCS})
