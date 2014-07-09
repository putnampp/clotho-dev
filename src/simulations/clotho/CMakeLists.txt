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
INCLUDE_DIRECTORIES( ${${PROJECT_NAME}_SRC_DIR}/simulations/clotho )

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

#LIST( APPEND OBJECT_SRCS rng/rng_gsl.cpp )
LIST( APPEND OBJECT_SRCS event/clotho_event.cpp )
LIST( APPEND OBJECT_SRCS event/birth_event.cpp )
LIST( APPEND OBJECT_SRCS event/death_event.cpp )
LIST( APPEND OBJECT_SRCS event/mate_event.cpp )
LIST( APPEND OBJECT_SRCS event/mate_select_event.cpp )
LIST( APPEND OBJECT_SRCS event/maturity_event.cpp )
#LIST( APPEND OBJECT_SRCS event/inherit_event.cpp )
LIST( APPEND OBJECT_SRCS event/inherit_event.2.cpp )
LIST( APPEND OBJECT_SRCS event/environment_snapshot_event.cpp)
LIST( APPEND OBJECT_SRCS event/signal_mate_event.cpp)
LIST( APPEND OBJECT_SRCS event/selection_event.cpp )
#LIST( APPEND OBJECT_SRCS object/environment.cpp )
#LIST( APPEND OBJECT_SRCS object/distributed_environment.cpp )
#LIST( APPEND OBJECT_SRCS object/individual.cpp )
#LIST( APPEND OBJECT_SRCS object/individual_properties.cpp )
#LIST( APPEND OBJECT_SRCS models/default_life_cycle.cpp )
#LIST( APPEND OBJECT_SRCS models/SimpleReproduction.cpp )
#LIST( APPEND OBJECT_SRCS models/simple_random_selection.cpp )
#LIST( APPEND OBJECT_SRCS models/reproduction_models.cpp )
#LIST( APPEND OBJECT_SRCS genomes/human_genome.cpp)
#LIST( APPEND OBJECT_SRCS genomes/human_gamete.cpp)

ADD_LIBRARY( object SHARED ${OBJECT_SRCS} )

LIST( APPEND SRCS common_strings.cpp )
LIST( APPEND SRCS sex.cpp )
LIST( APPEND SRCS examples/clotho_commandline.cpp )
#LIST( APPEND SRCS clotho_application.cpp )
#LIST( APPEND SRCS main.cpp )
#LIST( APPEND SRCS clotho2.cpp )

#ADD_EXECUTABLE(clotho clotho2.cpp ${SRCS} )
#ADD_EXECUTABLE(clotho_fixed clotho_fixed.cpp ${SRCS} )
ADD_EXECUTABLE(clotho_no_sim examples/clotho_no_sim.cpp ${SRCS} )
ADD_EXECUTABLE(clotho_adjacency examples/clotho_adjacency.cpp ${SRCS} )
#ADD_EXECUTABLE(clotho_env_only clotho_env_only.cpp ${SRCS} )

#TARGET_LINK_LIBRARIES( clotho ${COMMON_LIBRARIES} ${PROJECT_NAME}_genetics engine object timer ${PROJECT_NAME}_utility)

#TARGET_LINK_LIBRARIES( clotho_fixed ${COMMON_LIBRARIES} genetics engine object timer )
TARGET_LINK_LIBRARIES( clotho_no_sim ${COMMON_LIBRARIES} ${PROJECT_NAME}_genetics engine object timer ${PROJECT_NAME}_utility)
TARGET_LINK_LIBRARIES( clotho_adjacency ${COMMON_LIBRARIES} ${PROJECT_NAME}_genetics engine object timer ${PROJECT_NAME}_utility)
#TARGET_LINK_LIBRARIES( clotho_env_only ${COMMON_LIBRARIES} genetics engine object timer )
