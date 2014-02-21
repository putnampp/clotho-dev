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

#ifndef _EVENT_H_
#define _EVENT_H_

#include <functional>
#include <type_traits>

#include "event_interface.h"
#include "ptr_type.h"
#include "object_handler.h"

template < class Dest, class Src, class CLOCK, class Enabled = void >
class Event;

template < class Dest, class Src = Dest, class CLOCK = unsigned int>
class Event<  Dest, Src, CLOCK, 
    std::enable_if< /*std::is_base_of< object_handler< Dest >, Dest>::value &&*/
                    std::is_base_of< ptr_type< Dest >, Dest >::value &&
                    std::is_base_of< ptr_type< Src >, Src >::value
                    >::type> >
: virtual public event< CLOCK > {
public:
    typedef Src::Ptr                    source_t;
    typedef Dest::Ptr                   destination_t;
    typedef typename Dest::handler_t    handler_t;

    Event( const source_t src, const destination_t dest,
            logical_clock_t sent, logical_clock_t received, handler_t handle ) : 
        event( received ),
        m_source( src ),
        m_destination( dest ),
        m_sent( sent )
   {}

    virtual void execute() = 0;

    source_t    getSource() const {
        return m_source;
    }

    destination_t getDestination() const {
        return m_destination;
    }

    logical_clock_t getSent() const {
        return m_sent;
    }

    logical_clock_t getReceived() const {
        return m_time;
    }

    virtual ~Event() {}
protected:
    source_t        m_source;
    destination_t   m_destination;
    logical_clock_t m_sent;         // for rollback
};

#endif  // _EVENT_H_
