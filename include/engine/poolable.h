#ifndef POOLABLE_H_
#define POOLABLE_H_

struct poolable {
    virtual size_t  getPoolIndex() const = 0;
    virtual void    setPoolIndex( size_t idx ) = 0;
    virtual ~poolable() {}
};

#endif  // POOLABLE_H_
