#ifndef GRAMMMAR_WORD_DEF_HPP_
#define GRAMMMAR_WORD_DEF_HPP_

namespace grammar {
namespace words {

template < typename A, typename SYM, typename G, typename ATTR, typename S >
class word {
public:
    typedef A       alphabet_type;
    typedef SYM     symbol_type;
    typedef G       symbol_group_type;
    typedef ATTR    attribute_type;
    typedef S       syntax_type;

    typedef typename symbol_group_type::iterator iterator;

protected:
    static  alphabet_type   m_alphabet;
};

}   // namespace word
}   // namespace grammar

#endif  // GRAMMMAR_WORD_DEF_HPP_
