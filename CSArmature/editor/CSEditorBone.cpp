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

#include "CSEditorBone.h"
#include "CSArmature.h"
#include "CSEditorTween.h"
#include "CSEditorDisplayManager.h"
#include "CSEditorArmature.h"
#include "CSTransformHelp.h"
#include "CSAnimation.h"
#include "CSAnimationData.h"
#include "CSEditorTween.h"

namespace cs {
 

EditorBone* EditorBone::create()
{

	EditorBone *pBone = new EditorBone();
	if (pBone && pBone->init())
	{
		pBone->autorelease();
		return pBone;
	}
	CC_SAFE_DELETE(pBone);
	return NULL;

}

EditorBone* EditorBone::create(const char *name)
{

	EditorBone *pBone = new EditorBone();
	if (pBone && pBone->init(name))
	{
		pBone->autorelease();
		return pBone;
	}
	CC_SAFE_DELETE(pBone);
	return NULL;
}

EditorBone::EditorBone()
{
	m_tSelfTransformMatrix = CCAffineTransformMake(1, 0, 0, 1, 0, 0);
}


EditorBone::~EditorBone(void)
{
	RemoveFromParentSignal.emit(getName().c_str(), NULL);
}

bool EditorBone::init()
{
	return init(NULL);
}

bool EditorBone::init(const char *name)
{
	bool bRet = false;
	do
	{

		if(NULL != name)
		{
			m_strName = name;
		}

		// init m_pUserData
		CC_SAFE_DELETE(m_pUserData);
		m_pUserData = FrameData::create();
		m_pUserData->retain();

		CC_SAFE_DELETE(m_pCombinedData);
		m_pCombinedData = FrameData::create();
		m_pCombinedData->retain();

		CC_SAFE_DELETE(m_pTween);
		m_pTween = EditorTween::create(this);
		m_pTween->retain();
		m_pTweenData = m_pTween->getTweenNode();

		CC_SAFE_DELETE(m_pDisplayManager);
		m_pDisplayManager = EditorDisplayManager::create(this);
		m_pDisplayManager->retain();

		bRet = true;
	}
	while (0);

	return bRet;
}

void EditorBone::updateTransform()
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

		m_tSelfTransformMatrix = m_tWorldTransform;

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

	}
}


void EditorBone::removeEditChildBone(Bone *bone, bool recursion)
{
	if ( m_pChildren->indexOfObject(bone) != UINT_MAX )
	{

		CCArray *_ccbones = bone->getChildren();
		CCObject *_object = NULL;
		CCARRAY_FOREACH(_ccbones, _object)
		{
			EditorBone *_ccBone = (EditorBone*)_object;
			if(recursion)
			{
				bone->removeChildBone(_ccBone, recursion);
			}
			else
			{
				_ccBone->convertNodeSpace(false);
				((EditorArmature*)_ccBone->getArmature())->changeEditBoneParent(_ccBone, "");
			}

		}


		((EditorBone*)bone)->RemoveFromParentSignal.emit(bone->getName().c_str(), NULL);

		bone->setParentBone(NULL);

		bone->getDisplayManager()->setDisplayRenderNode(NULL);

		m_pChildren->removeObject(bone);

	}
}

void EditorBone::removeEditFromParent(bool  recursion)
{
	if (NULL != m_pParent)
	{
		((EditorBone*)m_pParent)->removeEditChildBone(this, recursion);
	}
}

void EditorBone::setArmature(Armature *armature)
{
	if(m_pArmature)
	{
		NameChanedSignal.disconnect((EditorArmature*)m_pArmature);
	}

	m_pArmature = armature;

	if (m_pArmature)
	{
		NameChanedSignal.connect((EditorArmature*)m_pArmature, &EditorArmature::boneNameChangedSlot);
	}
}


void EditorBone::editName(const char *name)
{

	NameChanedSignal.emit(this, name);

	setName(name);

	m_pBoneData->m_strName = name;

	CCObject *_object = NULL;

	CCARRAY_FOREACH(m_pChildren, _object)
	{
		Bone *bone = (Bone*)_object;
		bone->getBoneData()->m_strParent = name;
	}

}

void EditorBone::editZorder(int zorder)
{
	setZOrder(zorder);

	m_pBoneData->m_iZOrder = zorder;
}

void EditorBone::convertNodeSpace(bool toParentSpace)
{
	CCDictionary *movementDic = getArmature()->getAnimation()->getAnimationData()->getMovementDic();

	CCDictElement *element = NULL;
	CCDICT_FOREACH(movementDic, element)
	{
		MovementData *movementData = (MovementData*)element->getObject();
		MovementBoneData *movBoneData = movementData->getMovementBoneData(m_strName.c_str());

		if(!movBoneData)
		{
			continue;
		}

		CCArray *frameList = movBoneData->getFrameDatas();

		int totalDuration = 0;

		CCObject *object = NULL;
		CCARRAY_FOREACH(frameList, object)
		{
			FrameData *frameData = (FrameData*)object;
			FrameData *parentFrameData = NULL;

			Bone *parentBone = getParentBone();
			MovementBoneData *movParentBoneData = movementData->getMovementBoneData(m_pParent->getName().c_str());

			FrameData helpParentData;

			//! transform node util parent bone is root bone
			while(!parentBone->isRootBone())
			{
				int length = movParentBoneData->getFrameCount();
				int parentTotalDuration = 0;
				int index = 0;
				int currentDuration = 0;

				while(index<length && (movParentBoneData ? (totalDuration<parentTotalDuration || totalDuration >= parentTotalDuration + currentDuration) : true))
				{
					parentFrameData = movParentBoneData->getFrameData(index);

					parentTotalDuration += currentDuration;
					currentDuration = parentFrameData->getDuration();
					index ++;
				}

				totalDuration += frameData->getDuration();


				parentBone = parentBone->getParentBone();
				movParentBoneData = movementData->getMovementBoneData(parentBone->getName().c_str());

				TransformHelp::transformToParentWithoutScale(helpParentData, *parentFrameData);
			}

			if(toParentSpace)
			{
				TransformHelp::transformFromParentWithoutScale(*frameData, helpParentData);
			}
			else
			{
				TransformHelp::transformToParentWithoutScale(*frameData, helpParentData);
			}

		}
	}

	((EditorTween*)m_pTween)->gotoFrame(m_pTween->getCurrentFrameIndex());
}

void EditorBone::convertNodeToParentSpace()
{
	convertNodeSpace(true);
}

void EditorBone::convertNodeToArmatureSpace()
{
	convertNodeSpace(false);
}


}

