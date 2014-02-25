#ifndef MUTATIONMODEL_H_
#define MUTATIONMODEL_H_

struct mutation_model {
    virtual void mutate( AlleleGroupPtr src ) = 0;
};

#endif  // MUTATIONMODEL_H_
