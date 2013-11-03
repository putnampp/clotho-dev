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

#ifndef POPULATIONMODEL_H_
#define POPULATIONMODEL_H_

#include "../ClothoModel.h"
#include "../ClothoModelCreator.h"

#include "../clothoobjects/events/BirthEvent.h"
#include "../clothoobjects/events/DeathEvent.h"

//DECLARE_CLOTHO_MODEL( PopulationModel ) {
class PopulationModel : 
    virtual public ClothoModel< Individual, BirthEvent >, 
    virtual public ClothoModel< Individual, DeathEvent > {
public:
    PopulationModel();

    void configure( const YAML::Node & n );

/*
    void handle( const Event * evt );
    void handle( const BirthEvent * evt );
    void handle( const DeathEvent * evt );
*/
    void operator()( const BirthEvent * e, const Individual * ind );
    void operator()( const DeathEvent * e, const Individual * ind );
    void dump( ostream & out );

    virtual ~PopulationModel();
protected:
    unsigned int m_living, m_lived, m_pop_size;
};

DECLARE_REGISTERED_CLOTHO_MODEL( PopulationModel )

#endif  // POPULATIONMODEL_H_
