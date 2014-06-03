#ifndef MUTATION_H_
#define MUTATION_H_

struct mutation {
    virtual void mutate( genome * ) = 0;
};

#endif  // MUTATION_H_
