#ifndef IRNG_H_
#define IRNG_H_

//#include <string>
#include "common.h"


typedef unsigned int    seed_t;
typedef std::string     type_t;

struct iRNG {
    virtual shared_ptr< iRNG > clone() = 0;

    virtual const type_t & getType() const = 0;
    virtual seed_t    getSeed() const = 0;
    virtual void    reseed( seed_t ) = 0;

    virtual unsigned int    getMin() const  = 0;
    virtual unsigned int    getMax() const  = 0;

    virtual unsigned int    nextInt() = 0;
    virtual unsigned int    nextInt( unsigned int ) = 0;
    virtual bool            nextBool() = 0;
    virtual double          nextUniform() = 0;

    /*
    * Common Distribution interfaces
    */ 
    virtual unsigned int    nextPoisson( double mu ) = 0;

    virtual ~iRNG() {}
};

#endif  // IRNG_H_
