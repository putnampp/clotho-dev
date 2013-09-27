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

#ifndef GENETICEVENT_H_
#define GENETICEVENT_H_

#include "common.h"
#include "Allele.h"
#include "ChromosomeSegment.h"

#include <vector>

using std::vector;

enum GeneticEventType { SUBSTITUTION, INSERTION, DELETION, RECOMBINATION };

class GeneticEvent : public chromosome_segment {
public:
    virtual GeneticEventType getType() const = 0;
    virtual pos_t   getStart() const = 0;
    virtual pos_t   getEnd() const = 0;
    virtual pos_t   length() const = 0;

    virtual bool allele( pos_t & pos, allele_t & all ) const = 0;

    virtual bool operator==( const GeneticEvent & ge ) const = 0;

/**
 *  splits the GeneticEvent relative to the supplied GeneticEvent
 */
    virtual void split( GeneticEvent * ge, vector< GeneticEvent * > & events ) const = 0;
    virtual void    combine( GeneticEvent * ge ) = 0;

    virtual ~GeneticEvent() {}
protected:
    GeneticEvent( pos_t s, pos_t l ) : chromosome_segment(s, l) {}
};

#endif  //  GENETICEVENT_H_
