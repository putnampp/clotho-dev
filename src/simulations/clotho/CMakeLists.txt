################################################################################
# Copyright (c) 2013, Patrick P. Putnam (putnampp@gmail.com)
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met: 
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer. 
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution. 
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are those
# of the authors and should not be interpreted as representing official policies,
# either expressed or implied, of the FreeBSD Project.
################################################################################
INCLUDE_DIRECTORIES( ${${PROJECT_NAME}_INCLUDE_DIR} )

#LIST( APPEND CLOTHOLIBS event )
#LIST( APPEND CLOTHOLIBS object )

#FOREACH( D ${CLOTHOLIBS} )
#    ADD_SUBDIRECTORY( ${D} )
#ENDFOREACH(D)

#LIST( APPEND EVENT_SRCS event/birth_event.cpp )
#LIST( APPEND EVENT_SRCS event/death_event.cpp )
#LIST( APPEND EVENT_SRCS event/mate_event.cpp )
#LIST( APPEND EVENT_SRCS event/inherit_event.cpp )

#ADD_LIBRARY( event SHARED ${EVENT_SRCS} )

LIST( APPEND OBJECT_SRCS event/birth_event.cpp )
LIST( APPEND OBJECT_SRCS event/death_event.cpp )
LIST( APPEND OBJECT_SRCS event/mate_event.cpp )
LIST( APPEND OBJECT_SRCS event/maturity_event.cpp )
LIST( APPEND OBJECT_SRCS event/inherit_event.cpp )
LIST( APPEND OBJECT_SRCS object/environment.cpp )
LIST( APPEND OBJECT_SRCS object/individual.cpp )
LIST( APPEND OBJECT_SRCS object/individual_properties.cpp )
LIST( APPEND OBJECT_SRCS models/SimpleReproduction.cpp )
LIST( APPEND OBJECT_SRCS models/simple_random_selection.cpp )
LIST( APPEND OBJECT_SRCS genomes/human_genome.cpp)
LIST( APPEND OBJECT_SRCS genomes/human_zygote.cpp)

ADD_LIBRARY( object SHARED ${OBJECT_SRCS} )

LIST( APPEND SRCS common_strings.cpp )
LIST( APPEND SRCS sex.cpp )
LIST( APPEND SRCS clotho_application.cpp )
LIST( APPEND SRCS main.cpp )

ADD_EXECUTABLE(clotho ${SRCS} )

#TARGET_LINK_LIBRARIES( clotho ${Boost_LIBRARIES} warped jsoncpp ${YAML_LIB} ${GSL_LIBRARIES} cchmc csimulation )
#TARGET_LINK_LIBRARIES( clotho ${COMMON_LIBRARIES} cchmc engine event object timer )
TARGET_LINK_LIBRARIES( clotho ${COMMON_LIBRARIES} cchmc engine object timer )
