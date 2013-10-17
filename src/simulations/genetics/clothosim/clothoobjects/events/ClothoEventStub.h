/*******************************************************************************
 * Copyright (c) 2013, Patrick P. Putnam (putnampp@gmail.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 ******************************************************************************/

#ifndef CLOTHOEVENTSTUB_H_
#define CLOTHOEVENTSTUB_H_

#include "common.h"
#include "DefaultEvent.h"
#include "DeserializerManager.h"
#include "SerializedInstance.h"

template < class EVT >
class ClothoEventStub {
public:
    typedef EVT event_t;
    ClothoEventStub( const char * name) : m_name( name ) {
        DeserializerManager::instance()->registerDeserializer( this->getDataType(), &ClothoEventStub< EVT >::deserialize );
    }

    const string & getDataType() const {
        return m_name;
    }

    static Serializable * deserialize( SerializedInstance * si ) {
        return new EVT( si );
    }

    virtual ~ClothoEventStub() {}

private:
    const string    m_name;
};

/*
#define REGISTERED_CLOTHO_EVENT_BEGIN( name )               \
    class name;                                             \
    const ClothoEventStub< name > evt_##name( #name );      \
    class name : public DefaultEvent {                      \
        friend class ClothoEventStub< name >;               \
    public:                                                 \
        name ( SerializedInstance * si );                   \
        const string & getDataType() const {                \
            return evt_##name.getDataType();                \
        } \
        unsigned int getEventSize() const { return sizeof( name ); }  \
        bool eventCompare( const Event * e );
        

#define REGISTERED_CLOTHO_EVENT_END( name ) };
*/

#define DECLARE_CLOTHO_EVENT( name )                                \
    class name : public DefaultEvent {                              \
    public:                                                         \
        name ( SerializedInstance * si );                           \
        const string & getDataType() const;                         \
        unsigned int getEventSize() const;                          \
        bool        eventCompare( const Event * e );

#define DEFINE_CLOTHO_EVENT_DESERIALIZATION_METHOD( name )          \
    template <> Serializable * ClothoEventStub< name >::deserialize( SerializedInstance * inst )

#define DEFINE_REGISTERED_CLOTHO_EVENT_IMPL( name )                        \
    const string & name::getDataType() const {                      \
        return evt_##name.getDataType();                            \
    }                                                               \
    unsigned int name::getEventSize() const {                       \
        return sizeof( name );                                      \
    }

#define DECLARE_REGISTERED_CLOTHO_EVENT( name )                     \
    extern const ClothoEventStub< name > evt_##name

#define DEFINE_REGISTERED_CLOTHO_EVENT( name )                      \
    const ClothoEventStub< name > evt_##name( #name );              \
    DEFINE_REGISTERED_CLOTHO_EVENT_IMPL( name )

#endif  // CLOTHOEVENTSTUB_H_
