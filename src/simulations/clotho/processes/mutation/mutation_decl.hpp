#ifndef MUTATION_DECL_HPP_
#define MUTATION_DECL_HPP_

namespace processes {
namespace mutation {

template < class Category, class A, class AS, class ASptr = AS * >
class mutate {
public:
    typedef A           allele_type;
    typedef AS          allele_set_type;
    typedef Category    mutation_category;
    typedef ASptr   allele_set_pointer;

    allele_set_pointer operator()( allele_set_pointer as );

    virtual ~mutate();
protected:

};

}   // namespace mutation
}   // namespace processes

#endif  // MUTATION_DECL_HPP_
