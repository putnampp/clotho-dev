#ifndef REPRODUCTION_H_
#define REPRODUCTION_H_

struct reproduction {
    virtual void reproduce( AlleleGroupPtr p0, AlleleGroupPtr offspring ) = 0;
};

#endif  // REPRODUCTION_H_
