#ifndef GRAMMAR_WORD_ITERATOR_HPP_
#define GRAMMAR_WORD_ITERATOR_HPP_

namespace grammar {
namespace sentence {

template < typename W, typename WG >
class word_iterator {
public:
    typedef W   word_type;
    typedef WG  word_group_type;

    word_iterator( word_group_type * wg ) : m_words( wg ) {}
protected:
    word_group_type * m_words;
};

}   // namespace sentence
}   // namespace grammar

#endif  // GRAMMAR_SENTENCE_ITERATOR_HPP_
