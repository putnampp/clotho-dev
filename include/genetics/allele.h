#ifndef ALLELE_H_
#define ALLELE_H_

struct allele {
    double dominance, selection;
    allele( double d = 1.0, double s = 1.0 ) : dominance(d), selection(s) {}
    virtual ~allele() {}
};

#endif  // MUTATION_H_
