#ifndef VARIANT_DATABASE_H_
#define VARIANT_DATABASE_H_

struct variant_database {
    virtual void * allocateLS( ) = 0;
    virtual void freeLS( void * ) = 0;

    virtual size_t getSymbolIndex( mutation * m ) = 0;

    virtual ~variant_database() {}
};

#endif  // VARIANT_DATABASE_H_
