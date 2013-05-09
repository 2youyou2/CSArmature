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

#include "CSBone.h"
#include "CSUtilMath.h"
#include "CSArmature.h"
#include "CSArmatureDataManager.h"
#include "CSTransformHelp.h"
#include "CSDisplayManager.h"

namespace cs {

Bone* Bone::create()
{

    Bone *pBone = new Bone();
    if (pBone && pBone->init())
    {
        pBone->autorelease();
        return pBone;
    }
    CC_SAFE_DELETE(pBone);
    return NULL;

}
    
Bone* Bone::create(const char *name)
{
    
    Bone *pBone = new Bone();
    if (pBone && pBone->init(name))
    {
        pBone->autorelease();
        return pBone;
    }
    CC_SAFE_DELETE(pBone);
    return NULL;
}

Bone::Bone()
{
    m_pTweenData = NULL;
    m_pParent = NULL;
    m_pArmature = NULL;
    m_pChildArmature = NULL;
    m_pBoneData = NULL;
    m_pTween = NULL;
    m_pTween = NULL;
    m_pChildren = NULL;
    m_pDisplayManager = NULL;
	m_bIgnoreMovementBoneData = false;
	m_tWorldTransform = CCAffineTransformMake(1, 0, 0, 1, 0, 0);
	m_bTransformDirty = true;
}


Bone::~Bone(void)
{
    CC_SAFE_DELETE(m_pChildren);
    CC_SAFE_DELETE(m_pTween);
    CC_SAFE_DELETE(m_pUserData);
    CC_SAFE_DELETE(m_pDisplayManager);
    
    if(m_pBoneData)
    {
        m_pBoneData->release();
    }
    
}

bool Bone::init()
{
    return Bone::init(NULL);
}


bool Bone::init(const char *name)
{
    bool bRet = false;
    do
    {
        
		if(NULL != name)
        {
            m_strName = name;
        }

		CC_SAFE_DELETE(m_pTweenData);
		m_pTweenData = FrameData::create();
		m_pTweenData->retain();

        CC_SAFE_DELETE(m_pTween);
		m_pTween = Tween::create(this);
		m_pTween->retain();
		//m_pTweenData = m_pTween->getTweenNode();
        
        CC_SAFE_DELETE(m_pDisplayManager);
        m_pDisplayManager = DisplayManager::create(this);
        m_pDisplayManager->retain();
        
        
        bRet = true;
    }
    while (0);

    return bRet;
}

void Bone::setBoneData(BoneData *boneData)
{
    CCAssert(NULL != boneData, "_boneData must not be NULL");
	
    m_pBoneData = boneData;
    m_pBoneData->retain();
    
    m_strName = m_pBoneData->name;
	m_nZOrder = m_pBoneData->zOrder;
    
    m_pDisplayManager->initDisplayList(boneData);
}
    
BoneData *Bone::getBoneData()
{
    return m_pBoneData;
}
   
void Bone::update(float delta)
{
	if (m_pParent)
		m_bTransformDirty = m_bTransformDirty || m_pParent->isTransformDirty();

	if (m_bTransformDirty)
	{
		float cosX	= cos(m_pTweenData->skewX);
		float cosY	= cos(m_pTweenData->skewY);
		float sinX	= sin(m_pTweenData->skewX);
		float sinY  = sin(m_pTweenData->skewY);

		m_tWorldTransform.a = m_pTweenData->scaleX * cosY;
		m_tWorldTransform.b = m_pTweenData->scaleX * sinY;
		m_tWorldTransform.c = m_pTweenData->scaleY * sinX;
		m_tWorldTransform.d = m_pTweenData->scaleY * cosX;
		m_tWorldTransform.tx = m_pTweenData->x;
		m_tWorldTransform.ty = m_pTweenData->y;

		m_tWorldTransform = CCAffineTransformConcat(m_tWorldTransform, nodeToParentTransform());

		if(m_pParent)
		{
			m_tWorldTransform = CCAffineTransformConcat(m_tWorldTransform, m_pParent->m_tWorldTransform);
		}

		CS_DISPLAY_UPDATE(this, m_pDisplayManager->getCurrentDecorativeDisplay(), delta);
		// 		if(m_pChildArmature)
		// 		{
		// 			m_pChildArmature->update(dt);
		// 		}
	}
}


void Bone::updateDisplayedColor(const ccColor3B& parentColor)
{
	CCNodeRGBA::updateDisplayedColor(parentColor);
	updateColor();
}

void Bone::updateDisplayedOpacity(GLubyte parentOpacity)
{
	CCNodeRGBA::updateDisplayedOpacity(parentOpacity);
	updateColor();
}

void Bone::updateColor()
{
	CCNode *display = m_pDisplayManager->getDisplayRenderNode();
	CCRGBAProtocol *protocol = dynamic_cast<CCRGBAProtocol*>(display);
	if(protocol != NULL)
	{
		protocol->setColor(ccc3(_displayedColor.r*m_pTweenData->r/255, _displayedColor.g*m_pTweenData->g/255, _displayedColor.b*m_pTweenData->b/255));
		protocol->setOpacity(_displayedOpacity * m_pTweenData->a/255);
	}
}


void Bone::addChildBone(Bone* _child)
{
	CCAssert( NULL != _child, "Argument must be non-nil");
	CCAssert( NULL == _child->m_pParent, "child already added. It can't be added again");
    
	if(!m_pChildren)
	{
		childrenAlloc();
	}

	if (m_pChildren->indexOfObject(_child) == UINT_MAX)
    {
        m_pChildren->addObject(_child);
        _child->setParentBone(this);
    }
}

void Bone::removeChildBone(Bone *bone, bool recursion)
{
	if ( m_pChildren->indexOfObject(bone) != UINT_MAX )
	{
		if(recursion)
		{
			CCArray *_ccbones = bone->m_pChildren;
			CCObject *_object = NULL;
			CCARRAY_FOREACH(_ccbones, _object)
			{
				Bone *_ccBone = (Bone*)_object;
				bone->removeChildBone(_ccBone, recursion);
			}
		}

		bone->setParentBone(NULL);

		bone->getDisplayManager()->setDisplayRenderNode(NULL);

		m_pChildren->removeObject(bone);

	}
}

void Bone::removeFromParent(bool recursion)
{
	if (NULL != m_pParent)
	{
		m_pParent->removeChildBone(this, recursion);
	}
}

void Bone::setParentBone(Bone *_parent)
{
    m_pParent = _parent;
}

Bone *Bone::getParentBone()
{
    return m_pParent;
}

void Bone::childrenAlloc(void)
{
	CC_SAFE_DELETE(m_pChildren);
	m_pChildren = CCArray::createWithCapacity(4);
	m_pChildren->retain();
}


void Bone::setChildArmature(Armature *armature)
{
// 	if(m_pChildArmature)
// 	{
// 		
//         /*
//          *  Remove child armature from armature's bone list
//          */
//         m_pChildArmature->getRootBone()->removeFromParent(true);
// 
// 		m_pChildArmature->getRootBone()->getDisplayManager()->setVisible(false);
// 
// 		m_pChildArmature->setArmature(NULL);
// 	}
// 
// 	m_pChildArmature = armature;
// 
// 	if (m_pChildArmature)
// 	{
// 		m_pChildArmature->getRootBone()->getDisplayManager()->setVisible(true);
// 		m_pChildArmature->setArmature(m_pArmature);
// 	}
// 	
}

Armature *Bone::getChildArmature()
{
	return m_pChildArmature;
}

CCArray *Bone::getChildren()
{
    return m_pChildren;
}

Tween *Bone::getTween()
{
	return m_pTween;
}

void Bone::setZOrder(int zOrder)
{
	if (m_nZOrder != zOrder)
		CCNode::setZOrder(zOrder);
}

void Bone::addDisplay(DisplayData *_displayData, int _index)
{
    m_pDisplayManager->addDisplay(_displayData, _index);
}

void Bone::changeDisplayByIndex(int _index, bool _force)
{
    m_pDisplayManager->changeDisplayByIndex(_index, _force);
}

}
