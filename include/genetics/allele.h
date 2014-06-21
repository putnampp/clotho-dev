#ifndef ALLELE_H_
#define ALLELE_H_

struct allele {
    double dominance, selection;
    allele( double d = 1.0, double s = 1.0 ) : dominance(d), selection(s) {}
    allele( const allele & a ) : dominance(a.d), selection(a.s) {}

    virtual ~allele() {}
};

inline bool operator==( const allele & lhs, const allele & rhs ) {
    return lhs.dominance == rhs.dominance && lhs.selection == rhs.selection;
}

inline bool operator<( const allele & lhs, const allele & rhs ) {
    return lhs.dominance < rhs.dominance || 
            (lhs.dominance == rhs.dominance && lhs.selection < rhs.selection);
    // return (lhs.dominance <= rhs.dominance) && (lhs.selection < rhs.selection || lhs.dominance == rhs.dominance );
}

}

#endif  // MUTATION_H_
