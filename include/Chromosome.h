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

#ifndef CHROMOSOME_H_
#define CHROMOSOME_H_

#include "common.h"
#include <cassert>
#include "Configurable.h"

#include <set>

using std::set;

#define DEFAULT_CHROMOSOME_LEN 100

typedef unsigned char chromid_t;
#define UNKNOWN_CHROM -1

class ChromosomeMap;

class Chromosome {
public:
    friend class ChromosomeMap;

    Chromosome( const String & n, size_t s = DEFAULT_CHROMOSOME_LEN);

    void    add_site( size_t pos );

    chromid_t id() const;
    String  name() const;
    size_t  length() const;

    size_t  loci() const;
    bool    is_locus( size_t pos ) const;

    virtual ~Chromosome();
protected:
    String      m_name;
    chromid_t   m_id;
    size_t      m_size;

    set< size_t > m_sites;

private:
    static chromid_t nextID();
};

typedef shared_ptr< Chromosome > ChromosomePtr;

#endif  // CHROMOSOME_H_
