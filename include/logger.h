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
