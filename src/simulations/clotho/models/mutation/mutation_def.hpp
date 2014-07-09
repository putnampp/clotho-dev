#ifndef MUTATION_DEF_HPP_
#define MUTATION_DEF_HPP_

namespace model {
namespace mutation {

template < class Category, class A, class AS >
class mutate {
public:
    typedef A           allele_type;
    typedef AS          allele_set_type;
    typedef Category    mutation_category;

    typedef allele_set_type *   allele_set_pointer;

    allele_set_pointer operator()( allele_set_pointer as );

    virtual ~mutate();
protected:

};

}   // namespace mutation
}   // namespace model

#endif  // MUTATION_DEF_HPP_
