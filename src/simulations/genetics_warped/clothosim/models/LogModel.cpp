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

#include "LogModel.h"
#include "../ClothoModelCoordinator.h"

#include <iostream>

using std::cout;
using std::endl;

LogModel::LogModel( const string & dirpath, int step) :
    m_cur_period(NULL), m_log_dir( dirpath ), m_step( step ) {}

LogModel::~LogModel() {
    if( m_logger.is_open() )    m_logger.close();

    if( m_cur_period )  delete m_cur_period;
}

void LogModel::operator()( const LogEvent * e, ClothoObject * obj ) {
    const IntVTime & tmpTime = static_cast< const IntVTime &>( e->getReceiveTime() );
    if( m_cur_period == NULL ||
            *m_cur_period != tmpTime ) {
        m_cur_period = dynamic_cast< IntVTime * >( tmpTime.clone() );

        if( m_logger.is_open() )    m_logger.close();

        string path = m_log_dir + m_cur_period->toString() + ".log";

        m_logger.open( path.c_str(), std::ios::app );
    }

    if( m_logger.is_open() ) {
        obj->print( m_logger );
    }

    //Event * le = new LogEvent( e->getReceiveTime(), tmpTime + m_step, obj, obj );
    //obj->receiveEvent( le );
}

void LogModel::dump( ostream & out ) {

}
