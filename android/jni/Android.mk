
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifdef DEBUG
	LOCAL_MODULE := liquidfun_d
else
	LOCAL_MODULE := liquidfun
endif

LOCAL_SRC_FILES 	:= \
../../src/Box2D/Collision/b2BroadPhase.cpp \
../../src/Box2D/Collision/b2CollideCircle.cpp \
../../src/Box2D/Collision/b2CollideEdge.cpp \
../../src/Box2D/Collision/b2CollidePolygon.cpp \
../../src/Box2D/Collision/b2Collision.cpp \
../../src/Box2D/Collision/b2Distance.cpp \
../../src/Box2D/Collision/b2DynamicTree.cpp \
../../src/Box2D/Collision/b2TimeOfImpact.cpp \
../../src/Box2D/Collision/Shapes/b2ChainShape.cpp \
../../src/Box2D/Collision/Shapes/b2CircleShape.cpp \
../../src/Box2D/Collision/Shapes/b2EdgeShape.cpp \
../../src/Box2D/Collision/Shapes/b2PolygonShape.cpp \
../../src/Box2D/Common/b2BlockAllocator.cpp \
../../src/Box2D/Common/b2Draw.cpp \
../../src/Box2D/Common/b2FreeList.cpp \
../../src/Box2D/Common/b2Math.cpp \
../../src/Box2D/Common/b2Settings.cpp \
../../src/Box2D/Common/b2StackAllocator.cpp \
../../src/Box2D/Common/b2Stat.cpp \
../../src/Box2D/Common/b2Timer.cpp \
../../src/Box2D/Dynamics/b2Body.cpp \
../../src/Box2D/Dynamics/b2ContactManager.cpp \
../../src/Box2D/Dynamics/b2Fixture.cpp \
../../src/Box2D/Dynamics/b2Island.cpp \
../../src/Box2D/Dynamics/b2WorldCallbacks.cpp \
../../src/Box2D/Dynamics/b2World.cpp \
../../src/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp \
../../src/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp \
../../src/Box2D/Dynamics/Contacts/b2CircleContact.cpp \
../../src/Box2D/Dynamics/Contacts/b2Contact.cpp \
../../src/Box2D/Dynamics/Contacts/b2ContactSolver.cpp \
../../src/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp \
../../src/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp \
../../src/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp \
../../src/Box2D/Dynamics/Contacts/b2PolygonContact.cpp \
../../src/Box2D/Dynamics/Joints/b2DistanceJoint.cpp \
../../src/Box2D/Dynamics/Joints/b2FrictionJoint.cpp \
../../src/Box2D/Dynamics/Joints/b2GearJoint.cpp \
../../src/Box2D/Dynamics/Joints/b2Joint.cpp \
../../src/Box2D/Dynamics/Joints/b2MotorJoint.cpp \
../../src/Box2D/Dynamics/Joints/b2MouseJoint.cpp \
../../src/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp \
../../src/Box2D/Dynamics/Joints/b2PulleyJoint.cpp \
../../src/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp \
../../src/Box2D/Dynamics/Joints/b2RopeJoint.cpp \
../../src/Box2D/Dynamics/Joints/b2WeldJoint.cpp \
../../src/Box2D/Dynamics/Joints/b2WheelJoint.cpp \
../../src/Box2D/Particle/b2ParticleAssembly.cpp \
../../src/Box2D/Particle/b2Particle.cpp \
../../src/Box2D/Particle/b2ParticleGroup.cpp \
../../src/Box2D/Particle/b2ParticleSystem.cpp \
../../src/Box2D/Particle/b2VoronoiDiagram.cpp \
../../src/Box2D/Rope/b2Rope.cpp

LOCAL_C_INCLUDES	:= ../../include

LOCAL_CPPFLAGS := -std=c++11

TARGET_OUT := ../../lib/android/$(TARGET_ARCH_ABI)

include $(BUILD_STATIC_LIBRARY)


