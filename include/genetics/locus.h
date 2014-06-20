#ifndef LOCUS_H_
#define LOCUS_H_

#include <vector>
#include "allele.h"

class Locus {
public:
    typedef double key_t;
    typedef allele allele_t;
    typedef std::vector< allele_t * > alleles_t;

    Locus( key_t k );

    inline key_t getKey() const { return m_key; }

    allele * operator[]( size_t idx );

    void addAllele( allele_t * a );

    inline size_t allele_count() const { return m_alleles.size(); }

    virtual ~Locus();
protected:
    key_t       m_key;
    alleles_t   m_alleles;
};

#endif  // LOCUS_H_
