/*
 * Copyright (c) 2012 Chukong Technologies, Inc.
 *
 * http://www.cocostudio.com
 * http://tools.cocoachina.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "CSPhysicsWorld.h"
#include "GLES-Render.h"
#include "CSArmatureDefine.h"

namespace cs {

PhysicsWorld *PhysicsWorld::s_PhysicsWorld = NULL;


PhysicsWorld *PhysicsWorld::sharedPhysicsWorld()
{
	if (s_PhysicsWorld == NULL)
	{
		s_PhysicsWorld = new PhysicsWorld();
		s_PhysicsWorld->initNoGravityWorld();
	}

	return s_PhysicsWorld;
}

PhysicsWorld::PhysicsWorld()
	:m_pNoGravityWorld(NULL)
{
}

PhysicsWorld::~PhysicsWorld()
{
}

void PhysicsWorld::initNoGravityWorld()
{
	b2Vec2 noGravity(0, 0);

	m_pNoGravityWorld = new b2World(noGravity);
	m_pNoGravityWorld->SetAllowSleeping(true);

	m_pContactListener = new ContactListener();
	m_pNoGravityWorld->SetContactListener(m_pContactListener);


#if ENABLE_PHYSICS_DEBUG
	GLESDebugDraw *debugDraw = new GLESDebugDraw( PT_RATIO );
	m_pNoGravityWorld->SetDebugDraw(debugDraw);

	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	//        flags += b2Draw::e_jointBit;
	//        flags += b2Draw::e_aabbBit;
	//        flags += b2Draw::e_pairBit;
	//        flags += b2Draw::e_centerOfMassBit;
	debugDraw->SetFlags(flags);
#endif
}

b2World *PhysicsWorld::getNoGravityWorld()
{
	return m_pNoGravityWorld;
}

void PhysicsWorld::update(float dt)
{
	m_pNoGravityWorld->Step(dt, 0, 0);

	for (std::list<Contact>::iterator it = m_pContactListener->contact_list.begin(); it != m_pContactListener->contact_list.end(); ++it)
	{
		Contact &contact = *it;

		b2Body *b2a = contact.fixtureA->GetBody();
		b2Body *b2b = contact.fixtureB->GetBody();

		Bone *ba = (Bone*)b2a->GetUserData();
		Bone *bb = (Bone*)b2b->GetUserData();

		BoneColliderSignal.emit(ba, bb);
	}

}

void PhysicsWorld::drawDebug()
{
	m_pNoGravityWorld->DrawDebugData();
}

}