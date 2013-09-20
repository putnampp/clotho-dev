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

#ifndef STATISTIC_H_
#define STATISTIC_H_

#include "common.h"
#include "Describable.h"
#include "Configurable.h"
#include "Population.h"

#include <fstream>

using std::istream;
using std::ostream;

/*******************************************************************************
 * Statistic is an abstract class from which statistic algorithms are
 * derived.
 *
 * Every Statistic is configurable. Meaning that it may or may not accept 
 * external configuration settings.
 ******************************************************************************/
class Statistic : public Describable, public Configurable {
public:
    Statistic( const String & desc ) : Describable( desc ) {}

    virtual void configure( istream & in ) = 0;
    virtual void operator()( const Population & p ) = 0;

    friend ostream & operator<<( ostream & out, const Statistic & stat );
    virtual ~Statistic() {}
protected:
    virtual void write_stat( ostream & out ) const {
        out << m_desc;
    }
};

typedef boost::shared_ptr< Statistic > StatPtr;

std::ostream & operator<<( ostream & out, const Statistic & stat ) {
    stat.write_stat( out );
    return out;
}

#endif  // STATISTIC_H_
