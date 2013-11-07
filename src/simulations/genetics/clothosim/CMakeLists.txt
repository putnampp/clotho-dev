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
INCLUDE_DIRECTORIES( ${Warped_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${Warped_LIBRARIES} )

LIST( APPEND OBJECT_MNGR_SRC ClothoObject.cpp )
LIST( APPEND OBJECT_MNGR_SRC ClothoModel.cpp )
#LIST( APPEND OBJECT_MNGR_SRC ClothoModelManager.cpp )

LIST( APPEND SIM_SRC clothoobjects/common_types.cpp )

LIST( APPEND SIM_SRC clothoobjects/events/DeathEvent.cpp )
LIST( APPEND SIM_SRC clothoobjects/events/ShellDeathEvent.cpp )
LIST( APPEND SIM_SRC clothoobjects/events/BirthEvent.cpp )
LIST( APPEND SIM_SRC clothoobjects/events/ShellBirthEvent.cpp )
LIST( APPEND SIM_SRC clothoobjects/events/LogEvent.cpp )
LIST( APPEND SIM_SRC clothoobjects/events/MaturityEvent.cpp )
LIST( APPEND SIM_SRC clothoobjects/events/ShellMaturityEvent.cpp )
LIST( APPEND SIM_SRC clothoobjects/events/ShellMatingEvent.cpp )

LIST( APPEND SIM_SRC clothoobjects/EnvironmentObject.cpp )
LIST( APPEND SIM_SRC clothoobjects/Environment2.cpp )
LIST( APPEND SIM_SRC clothoobjects/EnvironmentObjectState.cpp )
LIST( APPEND SIM_SRC clothoobjects/IndividualObject.cpp )
LIST( APPEND SIM_SRC clothoobjects/RandomPopulation.cpp )
LIST( APPEND SIM_SRC clothoobjects/IndividualObjectState.cpp )
LIST( APPEND SIM_SRC clothoobjects/IndividualShellObject.cpp )
LIST( APPEND SIM_SRC clothoobjects/IndividualProperties.cpp )
LIST( APPEND SIM_SRC clothoobjects/PopulatedEnvironment.cpp )
LIST( APPEND SIM_SRC clothoobjects/PopulationInitializer.cpp )

ADD_LIBRARY(cobjmanager SHARED ${OBJECT_MNGR_SRC} )

ADD_LIBRARY(cobjcommon SHARED ${SIM_SRC})

ADD_DEPENDENCIES( cobjcommon cobjmanager )

#LIST( APPEND USER_SRC user_defined_object/UDObject.cpp )

#ADD_LIBRARY(udobjects SHARED ${USER_SRC} )

#ADD_DEPENDENCIES( udobjects cobjmanager )

#LIST( APPEND MODEL_SRC models/SelectionModel.cpp )
#LIST( APPEND MODEL_SRC models/RandomSelectionModel.cpp )
LIST( APPEND MODEL_SRC models/PopulationModel.cpp )
LIST( APPEND MODEL_SRC models/PopulationModelCreator.cpp )
LIST( APPEND MODEL_SRC models/LifeExpectancyModel.cpp )
LIST( APPEND MODEL_SRC models/LifeExpectancyModelCreator.cpp )
LIST( APPEND MODEL_SRC models/MaturityModel.cpp )
LIST( APPEND MODEL_SRC models/MaturityModelCreator.cpp )
LIST( APPEND MODEL_SRC models/LogModel.cpp )
LIST( APPEND MODEL_SRC models/LogModelCreator.cpp )
LIST( APPEND MODEL_SRC models/RandomMatingModel.cpp )
LIST( APPEND MODEL_SRC models/RandomMatingModelCreator.cpp )


ADD_LIBRARY(cmodels SHARED ${MODEL_SRC} )

ADD_DEPENDENCIES(cmodels cobjcommon)
