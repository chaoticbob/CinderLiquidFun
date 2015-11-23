cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE on )
if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "Debug" CACHE FILEPATH "" FORCE )
endif()

execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE CINDER_ARCH )
execute_process( COMMAND which clang OUTPUT_VARIABLE CLANG_PATH )

if( NOT "${CLANG_PATH}" STREQUAL "" )
    set( CINDER_TOOLCHAIN_CLANG true )
endif()

if( CINDER_TOOLCHAIN_CLANG )
    set( CMAKE_C_COMPILER						"clang" 		CACHE FILEPATH "" FORCE )
    set( CMAKE_CXX_COMPILER						"clang++" 		CACHE FILEPATH "" FORCE )
    set( CMAKE_AR          						"llvm-ar"		CACHE FILEPATH "" FORCE )
    set( CMAKE_LINKER       					"llvm-link" 	CACHE FILEPATH "" FORCE )
    set( CMAKE_NM           					"llvm-nm " 		CACHE FILEPATH "" FORCE )
    set( CMAKE_RANLIB       					"llvm-ranlib"	CACHE FILEPATH "" FORCE )

    set( CMAKE_C_FLAGS_INIT						"-Wall -std=c99" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_DEBUG_INIT				"-g" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_MINSIZEREL_INIT			"-Os -DNDEBUG" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_RELEASE_INIT				"-O4 -DNDEBUG" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_RELWITHDEBINFO_INIT		"-O2 -g" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS							"${CMAKE_C_FLAGS} -fmessage-length=0 " CACHE STRING "" FORCE )

    set( CMAKE_CXX_FLAGS_INIT					"-Wall" CACHE STRING "" FORCE )
    set( CMAKE_CXX_FLAGS_DEBUG_INIT				"-g" CACHE STRING "" FORCE )
    set( CMAKE_CXX_FLAGS_MINSIZEREL_INIT		"-Os -DNDEBUG" CACHE STRING "" FORCE )
    set( CMAKE_CXX_FLAGS_RELEASE_INIT			"-O4 -DNDEBUG" CACHE STRING "" FORCE )
    set( CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT	"-O2 -g" CACHE STRING "" FORCE )
    set( CMAKE_CCC_FLAGS						"${CMAKE_C_FLAGS} -fmessage-length=0 " CACHE STRING "" FORCE )

    set( STDCXXLIB                           	"-stdlib=libstdc++" )
else()
    set( CMAKE_C_COMPILER	"gcc" 				CACHE FILEPATH "" FORCE )
    set( CMAKE_CXX_COMPILER	"g++" 				CACHE FILEPATH "" FORCE )
endif()

project( liquidfun )

set( BOX2D_VERSION 2.3.0 )
set( BOX2D_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" )
set( BOX2D_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" )

set(BOX2D_Collision_SRCS
	${BOX2D_SRC_DIR}/Box2D/Collision/b2BroadPhase.cpp
	${BOX2D_SRC_DIR}/Box2D/Collision/b2CollideCircle.cpp
	${BOX2D_SRC_DIR}/Box2D/Collision/b2CollideEdge.cpp
	${BOX2D_SRC_DIR}/Box2D/Collision/b2CollidePolygon.cpp
	${BOX2D_SRC_DIR}/Box2D/Collision/b2Collision.cpp
	${BOX2D_SRC_DIR}/Box2D/Collision/b2Distance.cpp
	${BOX2D_SRC_DIR}/Box2D/Collision/b2DynamicTree.cpp
	${BOX2D_SRC_DIR}/Box2D/Collision/b2TimeOfImpact.cpp
)
set(BOX2D_Collision_HDRS
	${BOX2D_INC_DIR}/Box2D/Collision/b2BroadPhase.h
	${BOX2D_INC_DIR}/Box2D/Collision/b2Collision.h
	${BOX2D_INC_DIR}/Box2D/Collision/b2Distance.h
	${BOX2D_INC_DIR}/Box2D/Collision/b2DynamicTree.h
	${BOX2D_INC_DIR}/Box2D/Collision/b2TimeOfImpact.h
)
set(BOX2D_Shapes_SRCS
	${BOX2D_SRC_DIR}/Box2D/Collision/Shapes/b2CircleShape.cpp
	${BOX2D_SRC_DIR}/Box2D/Collision/Shapes/b2EdgeShape.cpp
	${BOX2D_SRC_DIR}/Box2D/Collision/Shapes/b2ChainShape.cpp
	${BOX2D_SRC_DIR}/Box2D/Collision/Shapes/b2PolygonShape.cpp
)
set(BOX2D_Shapes_HDRS
	${BOX2D_INC_DIR}/Box2D/Collision/Shapes/b2CircleShape.h
	${BOX2D_INC_DIR}/Box2D/Collision/Shapes/b2EdgeShape.h
	${BOX2D_INC_DIR}/Box2D/Collision/Shapes/b2ChainShape.h
	${BOX2D_INC_DIR}/Box2D/Collision/Shapes/b2PolygonShape.h
	${BOX2D_INC_DIR}/Box2D/Collision/Shapes/b2Shape.h
)
set(BOX2D_Common_SRCS
	${BOX2D_SRC_DIR}/Box2D/Common/b2BlockAllocator.cpp
	${BOX2D_SRC_DIR}/Box2D/Common/b2Draw.cpp
	${BOX2D_SRC_DIR}/Box2D/Common/b2FreeList.cpp
	${BOX2D_SRC_DIR}/Box2D/Common/b2Math.cpp
	${BOX2D_SRC_DIR}/Box2D/Common/b2Settings.cpp
	${BOX2D_SRC_DIR}/Box2D/Common/b2StackAllocator.cpp
	${BOX2D_SRC_DIR}/Box2D/Common/b2Stat.cpp
	${BOX2D_SRC_DIR}/Box2D/Common/b2Timer.cpp
	${BOX2D_SRC_DIR}/Box2D/Common/b2TrackedBlock.cpp
)
set(BOX2D_Common_HDRS
	${BOX2D_INC_DIR}/Box2D/Common/b2BlockAllocator.h
	${BOX2D_INC_DIR}/Box2D/Common/b2Draw.h
	${BOX2D_INC_DIR}/Box2D/Common/b2FreeList.h
	${BOX2D_INC_DIR}/Box2D/Common/b2GrowableStack.h
	${BOX2D_INC_DIR}/Box2D/Common/b2GrowableBuffer.h
	${BOX2D_INC_DIR}/Box2D/Common/b2IntrusiveList.h
	${BOX2D_INC_DIR}/Box2D/Common/b2Math.h
	${BOX2D_INC_DIR}/Box2D/Common/b2Settings.h
	${BOX2D_INC_DIR}/Box2D/Common/b2SlabAllocator.h
	${BOX2D_INC_DIR}/Box2D/Common/b2StackAllocator.h
	${BOX2D_INC_DIR}/Box2D/Common/b2Stat.h
	${BOX2D_INC_DIR}/Box2D/Common/b2Timer.h
	${BOX2D_INC_DIR}/Box2D/Common/b2TrackedBlock.h
)
set(BOX2D_Dynamics_SRCS
	${BOX2D_SRC_DIR}/Box2D/Dynamics/b2Body.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/b2ContactManager.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/b2Fixture.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/b2Island.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/b2World.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/b2WorldCallbacks.cpp
)
set(BOX2D_Dynamics_HDRS
	${BOX2D_INC_DIR}/Box2D/Dynamics/b2Body.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/b2ContactManager.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/b2Fixture.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/b2Island.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/b2TimeStep.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/b2World.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/b2WorldCallbacks.h
)
set(BOX2D_Contacts_SRCS
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Contacts/b2CircleContact.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Contacts/b2Contact.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Contacts/b2ContactSolver.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Contacts/b2PolygonContact.cpp
)
set(BOX2D_Contacts_HDRS
	${BOX2D_INC_DIR}/Box2D/Dynamics/Contacts/b2CircleContact.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Contacts/b2Contact.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Contacts/b2ContactSolver.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Contacts/b2PolygonContact.h
)
set(BOX2D_Joints_SRCS
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Joints/b2DistanceJoint.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Joints/b2FrictionJoint.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Joints/b2GearJoint.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Joints/b2Joint.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Joints/b2MotorJoint.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Joints/b2MouseJoint.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Joints/b2MotorJoint.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Joints/b2PulleyJoint.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Joints/b2RopeJoint.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Joints/b2WeldJoint.cpp
	${BOX2D_SRC_DIR}/Box2D/Dynamics/Joints/b2WheelJoint.cpp
)
set(BOX2D_Joints_HDRS
	${BOX2D_INC_DIR}/Box2D/Dynamics/Joints/b2DistanceJoint.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Joints/b2FrictionJoint.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Joints/b2GearJoint.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Joints/b2Joint.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Joints/b2MotorJoint.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Joints/b2MouseJoint.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Joints/b2PrismaticJoint.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Joints/b2PulleyJoint.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Joints/b2RevoluteJoint.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Joints/b2RopeJoint.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Joints/b2WeldJoint.h
	${BOX2D_INC_DIR}/Box2D/Dynamics/Joints/b2WheelJoint.h
)
set(BOX2D_Particle_SRCS
	${BOX2D_SRC_DIR}/Box2D/Particle/b2Particle.cpp
	${BOX2D_SRC_DIR}/Box2D/Particle/b2ParticleGroup.cpp
	${BOX2D_SRC_DIR}/Box2D/Particle/b2ParticleSystem.cpp
	${BOX2D_SRC_DIR}/Box2D/Particle/b2VoronoiDiagram.cpp
)
set(BOX2D_Particle_HDRS
	${BOX2D_INC_DIR}/Box2D/Particle/b2Particle.h
	${BOX2D_INC_DIR}/Box2D/Particle/b2ParticleGroup.h
	${BOX2D_INC_DIR}/Box2D/Particle/b2ParticleSystem.h
	${BOX2D_INC_DIR}/Box2D/Particle/b2StackQueue.h
	${BOX2D_INC_DIR}/Box2D/Particle/b2VoronoiDiagram.h
)
set(BOX2D_Rope_SRCS
	${BOX2D_SRC_DIR}/Box2D/Rope/b2Rope.cpp
)
set(BOX2D_Rope_HDRS
	${BOX2D_INC_DIR}/Box2D/Rope/b2Rope.h
)
set(BOX2D_General_HDRS
	${BOX2D_INC_DIR}/Box2D/Box2D.h
)

# These are used to create visual studio folders.
source_group(Collision 			FILES ${BOX2D_Collision_SRCS} ${BOX2D_Collision_HDRS})
source_group(Collision\\Shapes	FILES ${BOX2D_Shapes_SRCS}    ${BOX2D_Shapes_HDRS})
source_group(Common 			FILES ${BOX2D_Common_SRCS}    ${BOX2D_Common_HDRS})
source_group(Dynamics 			FILES ${BOX2D_Dynamics_SRCS}  ${BOX2D_Dynamics_HDRS})
source_group(Dynamics\\Contacts	FILES ${BOX2D_Contacts_SRCS}  ${BOX2D_Contacts_HDRS})
source_group(Dynamics\\Joints 	FILES ${BOX2D_Joints_SRCS}    ${BOX2D_Joints_HDRS})
source_group(Include 			FILES                         ${BOX2D_General_HDRS})
source_group(Particle 			FILES ${BOX2D_Particle_SRCS}  ${BOX2D_Particle_HDRS})
source_group(Rope 				FILES ${BOX2D_Rope_SRCS}      ${BOX2D_Rope_HDRS})

set( CXX_FLAGS "-std=c++11" )
if( CINDER_TOOLCHAIN_CLANG )
    set( CXX_FLAGS "${CXX_FLAGS} ${STDCXXLIB}" )
endif()
set( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_FLAGS}" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLAGS}" )

if( UNIX )
    set( PLATFORM_DIR "linux/${CINDER_ARCH}" )
endif()

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM_DIR}" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM_DIR}" )
set( CMAKE_DEBUG_POSTFIX "_d" )

add_library(Box2D STATIC
	${BOX2D_General_HDRS}
	${BOX2D_Joints_SRCS}
	${BOX2D_Joints_HDRS}
	${BOX2D_Contacts_SRCS}
	${BOX2D_Contacts_HDRS}
	${BOX2D_Dynamics_SRCS}
	${BOX2D_Dynamics_HDRS}
	${BOX2D_Common_SRCS}
	${BOX2D_Common_HDRS}
	${BOX2D_Shapes_SRCS}
	${BOX2D_Shapes_HDRS}
	${BOX2D_Collision_SRCS}
	${BOX2D_Collision_HDRS}
	${BOX2D_Particle_SRCS}
	${BOX2D_Particle_HDRS}
	${BOX2D_Rope_SRCS}
	${BOX2D_Rope_HDRS}
)

set_target_properties(Box2D PROPERTIES
	OUTPUT_NAME "liquidfun"
	CLEAN_DIRECT_OUTPUT 1
	VERSION ${BOX2D_VERSION}
)

target_include_directories(Box2D PUBLIC ${BOX2D_INC_DIR})

if(UNIX AND NOT APPLE)
	target_link_libraries(Box2D rt)
endif(UNIX AND NOT APPLE)
