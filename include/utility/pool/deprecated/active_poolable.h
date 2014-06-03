#ifndef ACTIVE_POOLABLE_H_
#define ACTIVE_POOLABLE_H_

struct active_poolable {
    virtual size_t getActiveIndex() const = 0;
    virtual void setActiveIndex( size_t idx ) = 0;

    virtual ~active_poolable() {}
};

#endif  // ACTIVE_POOLABLE_H_
