#ifndef ALLELE_H_
#define ALLELE_H_

struct allele {
    double dominance, selection;
    allele( double d = 1.0, double s = 1.0 ) : dominance(d), selection(s) {}
    allele( const allele & a ) : dominance(a.dominance), selection(a.selection) {}

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

template < typename A, typename P >
struct allele_generator {
    typedef A return_type;
    typedef P parameter_type;

    allele_generator() {}
    allele_generator( parameter_type p ) : m_params(p) {}

    return_type operator()() {
        return allele();
    }

    parameter_type m_params;
};

template < class A >
struct allele_generator< A, void > {
    typedef A return_type;

    allele_generator() {}

    inline return_type operator()() {
        return allele();
    }
};

#endif  // ALLELE_H_
