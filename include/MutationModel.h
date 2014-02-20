#ifndef MUTATIONMODEL_H_
#define MUTATIONMODEL_H_

struct mutation_model {
    virtual void mutation_model( AlleleGroupPtr src ) = 0;
};

#endif  // MUTATIONMODEL_H_
