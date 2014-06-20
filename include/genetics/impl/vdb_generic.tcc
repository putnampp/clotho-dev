#ifndef VDB_GENERIC_TCC_
#define VDB_GENERIC_TCC_

#define VDB_TEMPLATE() template < class L, class LS, class A > 

#define VDB_H() VariantDB<L, LS, A>

#define VDB_PREFIX( T ) VDB_TEMPLATE() T VDB_H() ## ::
#define VDB_METHOD( T, method) (VDB_PREFIX(T)) ## method

VDB_TEMPLATE() VDB_H()::VariantDB() {}

VDB_METHOD( void *, allocateLS)() {

}

VDB_METHOD( void, freeLS )( void * ) {

}

VDB_METHOD( size_t, getAlleleIndex)( allele_t * m ) {

}

VDB_TEMPLATE() VDB_H()::~VariantDB() {
    while( !m_loci.empty() ) {
        locus_t * tmp = m_loci.begin()->second;
        m_loci.erase( m_loci.begin() );
        delete tmp;
    }

    while( !m_locus_seqs.empty() ) {
        lspointer tmp = *m_locus_seqs.begin();
        m_locus_seqs.erase( m_locus_seqs.begin() );
        delete tmp;
    }

    while( !m_alleles.empty() ) {
        allele_t * tmp = *m_alleles.begin();
        m_alleles.erase( m_alleles.begin() );
        delete tmp;
    }
}

#undef VDB_TEMPLATE
#undef VDB_H
#undef VDB_PREFIX
#undef VDB_METHOD

#endif  // VDB_GENETIC_TCC_
