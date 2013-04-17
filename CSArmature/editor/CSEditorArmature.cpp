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

#include "CSEditorArmature.h"
#include "CSDataReaderHelper.h"
#include "CSUtilMath.h"

namespace cs {


EditorArmature::EditorArmature()
	:m_pIKBone(NULL)
	,m_iInterationsPerUpdate(1)
{
}


EditorArmature::~EditorArmature(void)
{
}

void EditorArmature::editName(const char *name)
{
	setName(name);

	m_pArmatureData->setName(name);
	getAnimation()->getAnimationData()->setName(name);
}

void EditorArmature::addEditBone(Bone *bone, const char *parentName)
{
	m_pArmatureData->addBoneData(bone->getBoneData());
	bone->getBoneData()->m_strParent = parentName;

	addBone(bone, parentName);

}

void EditorArmature::removeEditBone(Bone *bone, bool recursion)
{
	CCAssert(bone != NULL, "bone must be added to the bone dictionary!");

	EditorBone *editorBone = (EditorBone *)bone;

	editorBone->setArmature(NULL);
	editorBone->removeEditFromParent(recursion);
	m_pArmatureData->removeBoneData(editorBone->getName().c_str());
	editorBone->getBoneData()->m_strParent = "";

	m_pBoneDic->removeObjectForKey(bone->getName());
	m_pBoneList->removeObject(bone);

}

void EditorArmature::changeEditBoneParent(Bone *bone, const char *parentName)
{
	changeBoneParent(bone, parentName);

	bone->getBoneData()->m_strName = bone->getName();
	if(!bone->getParentBone()->isRootBone())
	{
		bone->getBoneData()->m_strParent = bone->getParentBone()->getBoneData()->m_strName;
	}


}

void EditorArmature::saveJsonToFile(const char *fileName)
{
	DataReaderHelper::saveArmatureDataToJson(fileName, m_pArmatureData);
	DataReaderHelper::saveAnimationDataToJson(fileName, getAnimation()->getAnimationData());
}

void EditorArmature::moveIKBoneToTarget(Bone *ikBone, CCPoint &targetPoint)
{
	m_pIKBone = ikBone;
	m_pTargetPoint = targetPoint;
}


void EditorArmature::update(float dt)
{
	Armature::update(dt);

	if (m_pIKBone)
	{
		updateIK();
	}
}

void EditorArmature::setVisible(bool var)
{
	Armature::setVisible(var);
	VisibleChangedSignal.emit(var);
}


void EditorArmature::updateIK()
{

	Bone *rootBone = m_pIKBone->getParentBone();

	while(!rootBone->getChildren()->count() > 1 || (!rootBone->isRootBone() && !rootBone->getParentBone()->isRootBone()))
	{
		rootBone = rootBone->getParentBone();
	}

	for(int i = 0; i < m_iInterationsPerUpdate; i++)
	{

		CCD_Result result = CalcIK_CCD(m_pIKBone, rootBone, m_pTargetPoint, 1);
		if (result == Success || result == Failure)
		{
			m_pIKBone = NULL;
			return;
		}
	}
}

void EditorArmature::connectBoneSignal(EditorBone *bone)
{
	bone->NameChanedSignal.connect(this, &EditorArmature::boneNameChangedSlot);
}

void EditorArmature::disconnectBoneSignal(EditorBone *bone)
{
	bone->NameChanedSignal.disconnect(this);
}


void EditorArmature::boneNameChangedSlot(Bone *bone, const char *newName)
{
	bone->retain();
	m_pBoneDic->removeObjectForKey(bone->getName());
	m_pBoneDic->setObject(bone, newName);
	bone->release();

	m_pArmatureData->removeBoneData(bone->getName().c_str());

	bone->getBoneData()->m_strName = newName;
	m_pArmatureData->addBoneData(bone->getBoneData());

}
    
    
}
