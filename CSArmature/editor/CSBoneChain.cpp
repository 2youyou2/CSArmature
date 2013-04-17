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

#include "CSBoneChain.h"
#include "CSBatchNodeManager.h"
#include "CSEditorDisplayManager.h"
#include "CSEditorBone.h"
#include "CSDecorativeDisplay.h"
#include "CSContourPoints.h"

namespace cs {

BoneChain *BoneChain::s_CurrentBoneChain = NULL;

BoneChain *BoneChain::create()
{
	BoneChain *pBoneChain = new BoneChain();
	if (pBoneChain)
	{
		pBoneChain->autorelease();
		return pBoneChain;
	}
	CC_SAFE_DELETE(pBoneChain);
	return NULL;
}
	
BoneChain *BoneChain::create(const char *fileName)
{
	BoneChain *pBoneChain = new BoneChain();
	if (pBoneChain && pBoneChain->initWithFile(fileName))
	{
		pBoneChain->autorelease();
		return pBoneChain;
	}
	CC_SAFE_DELETE(pBoneChain);
	return NULL;
}

BoneChain::BoneChain()
	:m_bConnectBone(false)
	,m_bSetChildPoint(false)
	,m_pChildBone(NULL)
	,m_pParentBone(NULL)
	,m_pCircleSprite(NULL)
	,m_pArrowSprite(NULL)
{
	s_CurrentBoneChain = this;
}

BoneChain::~BoneChain()
{
	m_pCircleSprite->removeFromParentAndCleanup(true);
	m_pArrowSprite->removeFromParentAndCleanup(true);
}

bool BoneChain::initWithFile(const char *dirPath)
{
	std::string dirPathStr = dirPath;

	
	m_pArrowSprite = CCSprite::create((dirPathStr + "/arrow.png").c_str());
	m_pArrowSprite->setOpacity(150);
	m_pArrowSprite->_setZOrder(10000);
	m_pArrowSprite->setAnchorPoint(ccp(0, 0.5f));
	m_pArrowSprite->setVisible(false);
	BatchNodeManager::sharedBatchNodeManager()->getCurrentLayer()->addChild(m_pArrowSprite);

	m_pCircleSprite = CCSprite::create((dirPathStr + "/circle.png").c_str());
	m_pCircleSprite->setOpacity(150);
	m_pCircleSprite->_setZOrder(10000);
	BatchNodeManager::sharedBatchNodeManager()->getCurrentLayer()->addChild(m_pCircleSprite);


	scheduleUpdate();

	return true;
}

void BoneChain::setParentChainPoint(CCPoint &parentPoint)
{
	m_pParantPoint = parentPoint;
}
void BoneChain::setChildChainPoint(CCPoint &childPoint)
{
	m_pChildPoint = childPoint;

	m_bSetChildPoint = true;
	m_pArrowSprite->setVisible(true);
}

void BoneChain::setChildBone(Bone* var)
{
	m_pChildBone = var;
	m_bConnectBone = true;

	connectBoneSignal(var);

	m_pOldChildAnchorPoint = m_pChildBone->getDisplayManager()->getDisplayRenderNode()->getAnchorPoint();
}

Bone *BoneChain::getChildBone()
{
	return m_pChildBone;
}

void BoneChain::setParentBone(Bone* var)
{
	m_pParentBone = var;
	
	m_pParantPoint.setPoint(m_pParentBone->m_tWorldTransform.tx, m_pParentBone->m_tWorldTransform.ty);

	connectBoneSignal(var);

	m_pOldParentAnchorPoint = m_pParentBone->getDisplayManager()->getDisplayRenderNode()->getAnchorPoint();
}

Bone *BoneChain::getParentBone()
{
	return m_pParentBone;
}


void BoneChain::update(float dt)
{
	if(m_pParentBone)
	{
		ContourPoints *parentContourPoints = m_pParentBone->getDisplayManager()->getCurrentDecorativeDisplay()->getContourPoints();
		m_pParantPoint = parentContourPoints->getCenterPoint()->convertToWorldSpaceAR(ccp(0, 0));
	}
	
	if (m_bConnectBone)
	{
		ContourPoints *childContourPoints = m_pChildBone->getDisplayManager()->getCurrentDecorativeDisplay()->getContourPoints();
		m_pChildPoint = childContourPoints->getCenterPoint()->convertToWorldSpaceAR(ccp(0, 0));
	}


	m_pCircleSprite->setPosition(m_pParantPoint);

	if(!m_bSetChildPoint)
	{
		return;
	}

	m_pArrowSprite->setPosition(m_pParantPoint);

	float r = -atan2f(m_pChildPoint.y - m_pParantPoint.y, m_pChildPoint.x - m_pParantPoint.x);
	m_pArrowSprite->setRotation(CC_RADIANS_TO_DEGREES(r));
	
	float distance = ccpDistance(m_pParantPoint, m_pChildPoint);
	m_pArrowSprite->setScaleX(distance / m_pArrowSprite->getTextureRect().size.width);
}

void BoneChain::draw()
{
// 	glEnable(GL_LINE_SMOOTH);
// 	glLineWidth(4);
// 
// 	ccDrawColor4B(255, 255, 255, 255);
// 	ccDrawCircle(m_pParantPoint, 15, CC_DEGREES_TO_RADIANS(90), 50, false);
// 
// 	ccDrawLine(m_pParantPoint, m_pChildPoint);
// 
// 	glLineWidth(2);
}

void BoneChain::setCurrentBoneChain(BoneChain *boneChain)
{
	s_CurrentBoneChain = boneChain;
}

BoneChain *BoneChain::getCurrentBoneChain()
{
	return s_CurrentBoneChain;
}

void BoneChain::connectBoneSignal(Bone *bone)
{
	EditorBone *editorBone = (EditorBone*)bone;
	EditorDisplayManager *displayManager = (EditorDisplayManager*)editorBone->getDisplayManager();

	editorBone->RemoveFromParentSignal.connect(this, &BoneChain::boneRemovedFromParent);

	displayManager->AnchorPointChangeTemporarySignal.connect(this, &BoneChain::boneAnchorPointChanged);
	displayManager->VisibleChangedSignal.connect(this, &BoneChain::boneVisibleChanged);

}

void BoneChain::boneRemovedFromParent(const char *name, CCDictionary *dict)
{
	removeFromParentAndCleanup(true);
	s_CurrentBoneChain = NULL;
}

void BoneChain::boneAnchorPointChanged(Bone *bone, CCPoint oldAnchorPoint, bool restoreAnchorPoint)
{

	if(bone == m_pParentBone)
	{
		if (!restoreAnchorPoint)
		{
			m_pOldParentAnchorPoint = oldAnchorPoint;
		}
		
	}
	else if(bone == m_pChildBone)
	{
		if(!restoreAnchorPoint)
		{
			m_pOldChildAnchorPoint = oldAnchorPoint;
		}
		
	}

}

void BoneChain::boneVisibleChanged(bool visible)
{
	//setVisible(visible);
}

}
