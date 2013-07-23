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
