#ifndef ELEMENT_ITERABLE_HPP_
#define ELEMENT_ITERABLE_HPP_

template < class C >
struct iterable_container {
    typedef C::iterator iterator;
    typedef C::const_iterator citerator;

    C m_container;

    citerator begin() const {
        return m_container.begin();
    }
    
    citerator end() const {
        return m_container.end();
    }

    iterator begin() {
        return m_container.begin();
    }

    iterator end() {
        return m_container.end();
    }
};

#endif  // ELEMENT_ITERABLE_HPP_
