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
#ifndef LOGGER_H_
#define LOGGER_H_

#include "popsim_config.h"

#include <iostream>
#include <cassert>

#ifdef LOG

extern std::ostream * g_log;

#define Log( x ) (*g_log) << x ;

#if( LOG_CRIT <= LOG_LEVEL )
#define LogC( x ) Log( "Critical: " << x ); g_log->flush();  assert(false);
#else
#define LogC( x ) assert( false );
#endif

#if( LOG_ERROR <= LOG_LEVEL )
#define LogE( x ) Log( "Error: " << x )
#else
#define LogE( x )
#endif

#if( LOG_WARN <= LOG_LEVEL )
#define LogW( x ) Log( "Warning: " << x )
#else
#define LogW( x )
#endif

#if( LOG_STATUS <= LOG_LEVEL )
#define LogS( x ) Log( "Status: " << x )
#else
#define LogS( x )
#endif

#if( LOG_NOTE <= LOG_LEVEL )
#define LogN( x ) Log( "Note: " << x )
#else
#define LogN( x )
#endif

#if( LOG_DEBUG <= LOG_LEVEL )
#define LogD( x ) Log( "Debug: " << x )
#else
#define LogD( x )
#endif

#else   // ! LOG

#define Log( x )

#define LogC( x )   assert(false);
#define LogE( x )
#define LogW( x )
#define LogS( x )
#define LogN( x )
#define LogD( x )

#endif  // LOG

#endif  // LOGGER_H_
