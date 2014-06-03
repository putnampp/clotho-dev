#ifndef SYNCHRONIZER_H_
#define SYNCHRONIZER_H_

struct synchronizer {
    virtual void sync() = 0;
    virtual ~synchronizer() {}
};

#endif  // SYNCHRONIZER_H_
