#ifndef ALPHABET_DEF_HPP_
#define ALPHABET_DEF_HPP_

namespace grammar {
namespace alphabets {

template < typename S, typename DB, typename SR, typename TR >
class alphabet {
public:
    typedef S   symbol_type;
    typedef DB  symbol_database_type;
    typedef SR  symbol_reference;
    typedef TR  trait_type;

    typedef typename symbol_database_type::iterator iterator;

    typedef std::shared_ptr< alphabet< S, DB, SR > > pointer;

    alphabet();
    alphabet( const alphabet< S, DB, SR > & ot );

    symbol_reference    addSymbol( const symbol_type & sym );
    void                removeSymbol( symbol_reference sym );

    symbol_reference    operator[]( const symbol_type & sym );
    symbol_type         operator[]( symbol_reference ref );

    iterator begin( );
    iterator end( );

    virtual ~alphabet();
};

}   // namespace alphabet
}   // namespace grammar

#endif  // ALPHABET_DEF_HPP_
