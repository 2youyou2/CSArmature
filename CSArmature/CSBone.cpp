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
#include "CSBatchNodeManager.h"
#include "CSTransformHelp.h"
#include "CSArmatureDisplayFactory.h"
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
    
    m_pUserData = NULL;
    m_pTweenData = NULL;
	m_pCombinedData = NULL;
    m_pParent = NULL;
    m_pArmature = NULL;
    m_pChildArmature = NULL;
    m_pBoneData = NULL;
    m_pTween = NULL;
    m_pTween = NULL;
    m_pChildren = NULL;
    m_pDisplayManager = NULL;
    m_iZOrder = 0;
	m_bIgnoreMovementBoneData = false;
	m_bRootBone = false;
	m_tWorldTransform = CCAffineTransformMake(1, 0, 0, 1, 0, 0);
	m_tWorldTransformForChildren = CCAffineTransformMake(1, 0, 0, 1, 0, 0);
	m_bColorDirty = false;
	m_bTransformDirty = false;
    m_iZOrder = m_fActualZorder = 0;
}


Bone::~Bone(void)
{
    CC_SAFE_DELETE(m_pChildren);
    CC_SAFE_DELETE(m_pTween);
    CC_SAFE_DELETE(m_pUserData);
	CC_SAFE_DELETE(m_pCombinedData);
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

		//! Init m_pUserData
        CC_SAFE_DELETE(m_pUserData);
        m_pUserData = FrameData::create();
		m_pUserData->m_fScaleX = 0;
		m_pUserData->m_fScaleY = 0;
        m_pUserData->retain();
        
        CC_SAFE_DELETE(m_pCombinedData);
        m_pCombinedData = FrameData::create();
        m_pCombinedData->retain();

        CC_SAFE_DELETE(m_pTween);
		m_pTween = Tween::create(this);
		m_pTween->retain();
		m_pTweenData = m_pTween->getTweenNode();
        
        CC_SAFE_DELETE(m_pDisplayManager);
        m_pDisplayManager = DisplayManager::create(this);
        m_pDisplayManager->retain();
        
        
        bRet = true;
    }
    while (0);

    return bRet;
}

void Bone::setBoneData(BoneData *_boneData)
{
    CCAssert(NULL != _boneData, "_boneData must not be NULL");
	
    m_pBoneData = _boneData;
    
    m_pBoneData->retain();
    
    //! init m_strName
    m_strName = m_pBoneData->m_strName;
    //! init zorder
	m_iZOrder = m_pBoneData->m_iZOrder;
    
    m_pDisplayManager->initDisplayList(_boneData);
}
    
BoneData *Bone::getBoneData()
{
    return m_pBoneData;
}
    
    
void Bone::update(float dt)
{
    if (m_pArmature)
    {
        m_pTween->update(dt);

        updateTransform(dt);
        updateColor();

        m_pDisplayManager->updateDisplay();
    
    }

    CCObject *_object = NULL;
    CCARRAY_FOREACH(m_pChildren, _object)
    {
        Bone *bone = (Bone*)_object;
        bone->update(dt);
    }

	m_bColorDirty = m_bTransformDirty = false;
}
   
void Bone::updateTransform(float dt)
{
	bool parentTransformDirty = false;

	if(m_pParent)
		parentTransformDirty =  m_pParent->isRootBone() ? m_pArmature->isTransformDirty() : m_pParent->isTransformDirty();
	
	m_bTransformDirty = m_bTransformDirty || parentTransformDirty;

	if (m_bTransformDirty)
	{
		m_pCombinedData->m_fX = m_pUserData->m_fX + m_pTweenData->m_fX;
		m_pCombinedData->m_fY = m_pUserData->m_fY + m_pTweenData->m_fY;
		m_pCombinedData->m_fSkewX = m_pUserData->m_fSkewX + m_pTweenData->m_fSkewX;
		m_pCombinedData->m_fSkewY = m_pUserData->m_fSkewY + m_pTweenData->m_fSkewY;
		m_pCombinedData->m_fScaleX = m_pUserData->m_fScaleX + m_pTweenData->m_fScaleX;
		m_pCombinedData->m_fScaleY = m_pUserData->m_fScaleY + m_pTweenData->m_fScaleY;
		
		float cosX	= cos(m_pCombinedData->m_fSkewX);
		float cosY	= cos(m_pCombinedData->m_fSkewY);
		float sinX	= sin(m_pCombinedData->m_fSkewX);
		float sinY  = sin(m_pCombinedData->m_fSkewY);

		m_tWorldTransform.a = m_pCombinedData->m_fScaleX * cosY;
		m_tWorldTransform.b = m_pCombinedData->m_fScaleX * sinY;
		m_tWorldTransform.c = m_pCombinedData->m_fScaleY * sinX;
		m_tWorldTransform.d = m_pCombinedData->m_fScaleY * cosX;
		m_tWorldTransform.tx = m_pCombinedData->m_fX;
		m_tWorldTransform.ty = m_pCombinedData->m_fY;

		if (m_pChildren && m_pChildren->count() > 0)
		{
			/*
			*  m_tWorldTransformForChildren is used for children, and it don't contain
			*  the parent scale value
			*/
			m_tWorldTransformForChildren.a = cosY;
			m_tWorldTransformForChildren.b = sinY;
			m_tWorldTransformForChildren.c = sinX;
			m_tWorldTransformForChildren.d = cosX;
			m_tWorldTransformForChildren.tx = m_pCombinedData->m_fX;
			m_tWorldTransformForChildren.ty = m_pCombinedData->m_fY;
		}

		if(m_pParent)
		{
			CCAffineTransform parentTransform = m_pParent->isRootBone() ? m_pArmature->nodeToWorldTransform() : m_pParent->m_tWorldTransformForChildren;

			m_tWorldTransform = CCAffineTransformConcat(m_tWorldTransform, parentTransform);

			if (m_pChildren && m_pChildren->count() > 0)
			{
				m_tWorldTransformForChildren = CCAffineTransformConcat(m_tWorldTransformForChildren, parentTransform);
			}
		}

        
        if(m_pChildArmature)
        {
            m_pChildArmature->setTransformDirty(true);
            m_pChildArmature->update(dt);
        }

	}
}

void Bone::updateColor()
{
	if(m_bColorDirty)
	{
		m_pCombinedData->m_iA = m_pUserData->m_iA  + m_pTweenData->m_iA;
		m_pCombinedData->m_iR = m_pUserData->m_iR + m_pTweenData->m_iR;
		m_pCombinedData->m_iG = m_pUserData->m_iG + m_pTweenData->m_iG;
		m_pCombinedData->m_iB = m_pUserData->m_iB + m_pTweenData->m_iB;
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
	if(m_pChildArmature)
	{
		
        /*
         *  Remove child armature from armature's bone list
         */
        m_pChildArmature->getRootBone()->removeFromParent(true);

		m_pChildArmature->getRootBone()->getDisplayManager()->setVisible(false);

		m_pChildArmature->setArmature(NULL);
	}

	m_pChildArmature = armature;

	if (m_pChildArmature)
	{
		m_pChildArmature->getRootBone()->getDisplayManager()->setVisible(true);
		m_pChildArmature->setArmature(m_pArmature);
	}
	
}

Armature *Bone::getChildArmature()
{
	return m_pChildArmature;
}

    
void Bone::setZOrder(int zorder)
{
    /*
	 *	If zorder is equal to m_iZOrder,then do nothing
	 */
    if (zorder == m_iZOrder) {
        return;
    }
    
    m_iZOrder = zorder;
    
    m_pArmature->setBonesIndexChanged(true);
}
int Bone::getZOrder()
{
    return m_iZOrder;
}
    
/**
 *  Set bone's property, used for users, this won't change data in the data pool
 */
void Bone::setPosition(float x, float y)
{
    m_pUserData->m_fX = x;
    m_pUserData->m_fY = y;

	m_bTransformDirty = true;
}
void Bone::setPositionX(float x)
{
    m_pUserData->m_fX = x;
	m_bTransformDirty = true;
}
void Bone::setPositionY(float y)
{
    m_pUserData->m_fY = y;
	m_bTransformDirty = true;
}
void Bone::setRotation(float r)
{
    m_pUserData->m_fSkewX = CC_DEGREES_TO_RADIANS(r);
    m_pUserData->m_fSkewY = CC_DEGREES_TO_RADIANS(-r);
	m_bTransformDirty = true;
}
void Bone::setScale(float scale)
{
    m_pUserData->m_fScaleX = scale;
    m_pUserData->m_fScaleY = scale;
	m_bTransformDirty = true;
}
void Bone::setScaleX(float scaleX)
{
    m_pUserData->m_fScaleX = scaleX;
	m_bTransformDirty = true;
}
void Bone::setScaleY(float scaleY)
{
    m_pUserData->m_fScaleY = scaleY;
	m_bTransformDirty = true;
}


/**
  * Get bone's user transform property
  */
CCPoint Bone::getPosition()
{
	return ccp(m_pUserData->m_fX, m_pUserData->m_fY);
}
float Bone::getPositionX()
{
    return m_pUserData->m_fX;
}
float Bone::getPositionY()
{
    return m_pUserData->m_fY;
}
float Bone::getRotation()
{
    return m_pUserData->m_fSkewX;
}
float Bone::getScaleX()
{
    return m_pUserData->m_fScaleX;
}
float Bone::getScaleY()
{
    return m_pUserData->m_fScaleY;
}


void Bone::setOpacity(GLubyte value)
{
	m_pUserData->m_iA = value;
	m_bColorDirty = true;
}
void Bone::setColor(const ccColor4B &color)
{
	m_pUserData->m_iA = color.a;
	m_pUserData->m_iR = color.r;
	m_pUserData->m_iG = color.g;
	m_pUserData->m_iB = color.b;
	m_bColorDirty = true;
}
void Bone::setColor(const ccColor3B &color)
{
	m_pUserData->m_iR = color.r;
	m_pUserData->m_iG = color.g;
	m_pUserData->m_iB = color.b;
	m_bColorDirty = true;
}


CCArray *Bone::getChildren()
{
    return m_pChildren;
}

FrameData *Bone::getCombinedData()
{
    return m_pCombinedData;
}

Tween *Bone::getTween()
{
	return m_pTween;
}

void Bone::_setActualZorder(float zorder)
{ 
	m_fActualZorder = zorder; 

	do 
	{
		CCNode *renderNode = m_pDisplayManager->getDisplayRenderNode();
		CC_BREAK_IF(!renderNode);

		if(m_pArmature->getRenderType() == BATCHNODE_VERTEXZ)
		{
			renderNode->setVertexZ(zorder/10000);
		}
		else
		{
			if (renderNode->getParent())
				renderNode->getParent()->reorderChild(renderNode, zorder);
			else
				renderNode->_setZOrder(zorder);
		}
	} while (0);
	
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
