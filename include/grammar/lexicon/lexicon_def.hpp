#ifndef GRAMMAR_LEXICON_DEF_HPP_
#define GRAMMAR_LEXICON_DEF_HPP_

namespace grammar {
namespace lexicons {

template < typename W, typename C, typename R >
class lexicon {
public:
    typedef W   word_type;
    typedef C   container_type;
    typedef R   word_reference;

    typedef typename container_type::iterator iterator;

    static const word_reference npos;

    lexicon();
    lexicon( const lexicon< W, C, R> & o );

    //word_reference  addWord( const word_type & w );
    word_reference  reference_to( const word_type & w, bool bCreate = false );

    void            removeWord( const word_type & w );
    void            removeWord( const word_reference & r );

    bool            isWord( const word_type & w );
    bool            isWord( const word_reference & r );

    size_t          size();

    word_reference  operator[]( const word_type & w );
    word_type       operator[]( const word_reference & ref );

    virtual ~lexicon();
protected:
    container_type  m_words;
};

}   // namespace lexicon
}   // namespace grammar

#endif  // GRAMMAR_LEXICON_DEF_HPP_
