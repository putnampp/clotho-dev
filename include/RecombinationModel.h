#ifndef RECOMBINATIONMODEL_H_
#define RECOMBINATIONMODEL_H_

struct recombination_model {
    virtual void recombine( AlleleGroupPtr src, AlleleGroupPtr dest ) = 0;
};

#endif  // RECOMBINATIONMODEL_H_
