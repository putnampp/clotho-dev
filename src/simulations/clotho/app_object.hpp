#ifndef APP_OBJECT_HPP_
#define APP_OBJECT_HPP_

#include "clotho_object.hpp"

class AppObject : public event_performer< ClothoEvent > {
public:
    AppObject( ClothoObject * co = NULL) :
        m_clotho_object( co ) {
    }

    AppObject( const AppObject & ao ) :
        m_clotho_object( ao.m_clotho_object ) {
    }

    ClothoObject * getClothoObject() const {
        return m_clotho_object;
    }

    virtual void perform( const ClothoEvent * evt ) {}

    virtual ~AppObject() {}
protected:
    ClothoObject *  m_clotho_object;
};

#endif  // APP_OBJECT_HPP_
