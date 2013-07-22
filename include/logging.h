#ifndef _LOGGING_H_
#define _LOGGING_H_

#include "logger.h"

#include <iostream>
#include <fstream>

#ifdef  LOG

bool init_logger( const std::string & log );
void release_logger( ); 

#endif  // LOG

#endif  //  _LOGGING_H_
