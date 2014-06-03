#ifndef INHERITANCEMODEL_H_
#define INHERITANCEMODEL_H_

struct inheritance_model {
    virtual void inherit( AlleleGroupPtr p0, AlleleGroupPtr p1, AlleleGroupPtr offspring ) = 0;
};

#endif  // INHERITANCEMODEL_H_
