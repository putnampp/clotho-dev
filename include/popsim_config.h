/*******************************************************************************
 * Copyright (c) 2013, Patrick P. Putnam
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
#ifndef POPSIM_CONFIG_H_
#define POPSIM_CONFIG_H_

#include "log_levels.h"

#define MAJOR_VERSION  "0.1"
#define MINOR_VERSION  "0"
#define VERSION        "POPSIM 0.1.0"

/* #undef LOG */
/* #undef LOG_LEVEL */

#ifdef LOG_LEVEL  // log_level defined
    #if LOG_LEVEL == LOG_OFF // but it is set to be off
        #ifdef LOG
            #warning "Turning Logging OFF"
            #undef LOG      // make sure logging is off
        #endif 
    #else   // LOG_LEVEL
        #ifndef LOG   // debug not turned
            #define LOG   // turn it on
        #endif

        #if LOG_LEVEL == LOG_CRIT
        #elif LOG_LEVEL == LOG_ERROR
        #elif LOG_LEVEL == LOG_WARN
        #elif LOG_LEVEL == LOG_STATUS
        #elif LOG_LEVEL == LOG_NOTE
        #elif LOG_LEVEL == LOG_DEBUG
        #elif LOG_LEVEL == LOG_ALL
        #else
            #error "UNKNOWN LOG LEVEL DEFINED."
        #endif // LOG_LEVEL
    #endif  // LOG_LEVEL
#elif defined LOG     // debugging turned on, but debug_level not defined
    #define LOG_LEVEL LOG_DEFAULT
#endif  // end verifications

#endif  // POPSIM_CONFIG_H_
