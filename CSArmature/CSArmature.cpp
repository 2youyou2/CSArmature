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

#include "CSArmature.h"
#include "CSArmatureDataManager.h"
#include "CSDatas.h"
#include "CSBatchNodeManager.h"
#include "CSArmatureDefine.h"
#include "CSDataReaderHelper.h"

#include "CSEditorArmature.h"


namespace cs {

    
std::map<int, Armature*> Armature::m_sArmatureIndexDic;
CCDictionary *Armature::m_sArmatureMap = new CCDictionary();
    
int Armature::m_siMaxArmatureZorder = 0;

    
Armature *Armature::create()
{
#if CS_DEBUG_FOR_EDIT
	Armature *armature = new EditorArmature();
#else
	Armature *armature = new Armature();
#endif
    if (armature && armature->init())
    {
        armature->autorelease();
        return armature;
    }
    CC_SAFE_DELETE(armature);
    return NULL;
}

    
Armature *Armature::create(const char* _name)
{
#if CS_DEBUG_FOR_EDIT
	Armature *armature = new EditorArmature();
#else
	Armature *armature = new Armature();
#endif
    if (armature && armature->init(_name))
    {
        armature->autorelease();
        return armature;
    }
    CC_SAFE_DELETE(armature);
    return NULL;
}
    
#if CS_TOOL_PLATFORM

Armature *Armature::createWithComArmature(const char* _name, ComArmature *_comArmature)
{
#if CS_DEBUG_FOR_EDIT
	Armature *armature = new EditorArmature();
#else
	Armature *armature = new Armature();
#endif
	if (armature && armature->initWithComArmature(_name, _comArmature))
	{
		armature->autorelease();
		return armature;
	}
	CC_SAFE_DELETE(armature);
	return NULL;
}

bool Armature::initWithComArmature(ComArmature *_comArmature)
{
	return initWithComArmature(NULL, _comArmature);
}


bool Armature::initWithComArmature(const char *_name, ComArmature *_comArmature)
{
	bool bRet = false;
	do
	{
		if (!Armature::init(_name))
		{
			break;
		}

		m_pComArmature = _comArmature;

		bRet = true;
	}while (0);

	return bRet;
}

#endif


Armature::Armature()
    :m_pAnimation(NULL)
    ,m_pBoneDic(NULL)
    ,m_pBoneList(NULL)
	,m_fInternalZOrder(0)
	,m_fActualZOrder(0)
    ,m_bBonesIndexChanged(false)
    ,m_pComArmature(NULL)
	,m_pRootBone(NULL)
	,m_pArmature(NULL)
	,m_bOpacityModifyRGB(true)
{
}


Armature::~Armature(void)
{
    // remove this Armature's m_fInternalZOrder from m_sArmatureMap, so other Armature can use this internal zorder
    m_sArmatureMap->removeObjectForKey(m_fInternalZOrder);

    if(NULL != m_pBoneDic)
    {
        m_pBoneDic->removeAllObjects();
        CC_SAFE_DELETE(m_pBoneDic);
    }
    if(NULL != m_pBoneList)
    {
        m_pBoneList->removeAllObjects();
        CC_SAFE_DELETE(m_pBoneList);
    }
    
    CC_SAFE_DELETE(m_pAnimation);

	CC_SAFE_DELETE(m_pRootBone);
}

    
bool Armature::init()
{
    return init(NULL);
}


bool Armature::init(const char *_name)
{
    bool bRet = false;
    do
    {
        cocos2d::CCLog("Armature (%s)  create.", _name);

		CC_SAFE_DELETE(m_pAnimation);
        m_pAnimation = Animation::create(this);
        CCAssert(m_pAnimation, "create Armature::m_pAnimation fail!");
        m_pAnimation->retain();

		CC_SAFE_DELETE(m_pBoneDic);
        m_pBoneDic	= CCDictionary::create();
        CCAssert(m_pBoneDic, "create Armature::m_pBoneDic fail!");
        m_pBoneDic->retain();
        
		CC_SAFE_DELETE(m_pBoneList);
        m_pBoneList = CCArray::createWithCapacity(4);
        CCAssert(m_pBoneList, "create Armature::m_pBoneList fail!");
        m_pBoneList->retain();

        
        if(_name == NULL)
        {
           m_strName = ""; 
        }
        else
        {
            m_strName = _name;
        }

		initRootBone();

        ArmatureDataManager *armatureDataManager = ArmatureDataManager::sharedArmatureDataManager();
        
        if(m_strName.compare("") != 0)
        {
            m_strName = _name;
            
            AnimationData* animationData = armatureDataManager->getAnimationData(_name);
            CCAssert(animationData, "AnimationData not exist! ");
            
            m_pAnimation->setAnimationData(animationData);
            
            
            ArmatureData *armatureData = armatureDataManager->getArmatureData(_name);
            CCAssert(armatureData, "");
            
            m_pArmatureData = armatureData;
            
            
            CCDictElement *_element = NULL;
			CCDictionary *boneDataDic = &armatureData->boneDataDic;
            CCDICT_FOREACH(boneDataDic, _element)
            {
                Bone *bone = createBone(_element->getStrKey());
                
                //! init bone's  Tween to 1st movement's 1st frame
                do {
                    
                    MovementData *movData = animationData->getMovement(animationData->movementNames.at(0).c_str());
                    CC_BREAK_IF(!movData);
                    
                    MovementBoneData *movBoneData = movData->getMovementBoneData(bone->getName().c_str());
                    CC_BREAK_IF(!movBoneData);
                    
                    FrameData *_frameData = movBoneData->getFrameData(0);
                    CC_BREAK_IF(!_frameData);
                    
                    bone->getTween()->getTweenNode()->copy(_frameData);
                } while (0);
            }
            
        }
        else
        {
            m_strName = "new_armature";
            m_pArmatureData = ArmatureData::create();
            m_pArmatureData->name = m_strName;
            
            AnimationData *animationData = AnimationData::create();
			animationData->name = m_strName;
            
            armatureDataManager->addArmatureData(m_strName.c_str(), m_pArmatureData);
            armatureDataManager->addAnimationData(m_strName.c_str(), animationData);
            
            m_pAnimation->setAnimationData(animationData);
           
        }

		internalSort();

		setZOrder(0);

        unscheduleUpdate();
		scheduleUpdate();
        
        bRet = true;
    }
    while (0);

    return bRet;
}

Bone *Armature::createBone(const char *boneName)
{
    
	if(getBone(boneName) != NULL)
	{
		return getBone(boneName);
	}
    
    BoneData *boneData = (BoneData*)m_pArmatureData->getBoneData(boneName);
	std::string parentName = boneData->parentName;

    Bone *bone = NULL;
    
    if( parentName.compare("") !=0 )
    {
        createBone(parentName.c_str());
        bone = Bone::create(boneName);
        addBone(bone, parentName.c_str());
    }else{
        bone = Bone::create(boneName);
        addBone(bone, "");
    }
    
    bone->setBoneData(boneData);
	bone->getDisplayManager()->changeDisplayByIndex(-1, false);
    
    return bone;
}


void Armature::addBone(Bone *bone, const char *parentName)
{
    CCAssert( bone != NULL, "Argument must be non-nil");
    CCAssert(m_pBoneDic->objectForKey(bone->getName()) == NULL, "bone already added. It can't be added again");

    if (NULL == parentName)
    {
        m_pRootBone->addChildBone(bone);
    }
    else
    {
        Bone *_boneParent = (Bone*)m_pBoneDic->objectForKey(parentName);
        
        if (_boneParent)
        {
            _boneParent->addChildBone(bone);
        }
        else
        {
            m_pRootBone->addChildBone(bone);
        }
    }
    
    
    bone->setArmature(this);
    
    
    m_pBoneDic->setObject(bone, bone->getName());
    m_pBoneList->addObject(bone);
}


void Armature::removeBone(Bone *bone, bool recursion)
{
    CCAssert(bone != NULL, "bone must be added to the bone dictionary!");
    
    bone->setArmature(NULL);
    bone->removeFromParent(recursion);
    
    m_pBoneDic->removeObjectForKey(bone->getName());
    m_pBoneList->removeObject(bone);
    
    
}


Bone *Armature::getBone(const char* _name)
{
	return (Bone*)m_pBoneDic->objectForKey(_name);
}


void Armature::changeBoneParent(Bone *bone, const char *parentName)
{
	CCAssert(bone != NULL, "bone must be added to the bone dictionary!");

	bone->getParentBone()->getChildren()->removeObject(bone);
	bone->setParentBone(NULL);


	if (parentName != NULL)
	{
		Bone *_boneParent = (Bone*)m_pBoneDic->objectForKey(parentName);

		if (_boneParent)
		{
			_boneParent->addChildBone(bone);
		}
		else
		{
			m_pRootBone->addChildBone(bone);
		}
	}
	else
	{
		m_pRootBone->addChildBone(bone);
	}
}

void Armature::setVisible(bool _visible)
{
	m_bVisible = _visible;

	CCObject *object = NULL;
	CCARRAY_FOREACH(m_pBoneList, object)
	{
		Bone *bone = (Bone*)object;
		bone->getDisplayManager()->setVisible(_visible);
	}
}

bool Armature::isVisible()
{
	return m_bVisible;
}


void Armature::onMovementEvent(const char *_eventType, const char *_movementID)
{
    if(NULL != m_pComArmature)
    {
        //m_pComArmature->onMovementEvent(_eventType, _movementID);
    }
}

    
void Armature::setZOrder(int _zOrder)
{
	CCNode::_setZOrder(_zOrder);

	/*
	 *	Calculate the actial zorder of this armature every time the m_iZOrder be set.
     *  Every m_fZOrder have at most ARMATURE_MAX_COUNT Armatures
     *  It's like a two dimensional table, m_fInternalZOrder is column number, and m_fZOrder is row number. Then m_fActualZOrder is the actual index.
     *  It can ensure every Armature have different vertexZ
	 */
	if(m_nZOrder >= 0)
	{
		m_fActualZOrder = m_nZOrder * ARMATURE_MAX_COUNT;
		m_fActualZOrder += m_fInternalZOrder;
	}else{
		m_fActualZOrder = m_nZOrder * ARMATURE_MAX_COUNT;
		m_fActualZOrder -= m_fInternalZOrder;
	}
	
    /*
	 *  Also every Armature have max child count, we allocate ARMATURE_MAX_CHILD vertexz number for each Armature
	 */
	m_pRootBone->_setActualZorder(m_fActualZOrder * ARMATURE_MAX_CHILD);

	int index = 0;
	sortBoneHelper(m_pRootBone->getActualZorder(), index);
    
    return;
}

CCDictionary *Armature::getBoneDic()
{
    return m_pBoneDic;
}
    
void Armature::update(float dt)
{
    m_pAnimation->update(dt);
    m_pRootBone->update(dt);

	m_bDirty = false;

    if (m_bBonesIndexChanged)
    {
		if(m_pArmature)
		{
			m_pArmature->setBonesIndexChanged(true);
			m_bBonesIndexChanged = false;
		}
		else
		{
			int i = 0;
			sortBoneHelper(m_pRootBone->getActualZorder(), i);

			m_bBonesIndexChanged = false;
		}
    }
}


CCAffineTransform Armature::nodeToParentTransform()
{
	m_bDirty = m_bTransformDirty;
	return CCNode::nodeToParentTransform();
}


void Armature::initRootBone()
{
	CC_SAFE_DELETE(m_pRootBone);
	m_pRootBone = Bone::create((m_strName + "_root_bone").c_str());
	CCAssert(m_pRootBone, "create Armature::m_pRootBone fail!");
	m_pRootBone->retain();

	m_pRootBone->setIgnoreMovementBoneData(true);
	m_pRootBone->setArmature(this);
    m_pRootBone->setRootBone(true);
}

void Armature::internalSort()
{
    if (m_sArmatureMap->count() >= ARMATURE_MAX_COUNT)
    {
        cocos2d::CCLog("warnning : current Armature count is more than ARMATURE_MAX_COUNT(%i), we will do not sort this Armature!!! ", ARMATURE_MAX_COUNT);
        m_fInternalZOrder = 0;
        return;
    }
    
    /*
     *  if m_siMaxArmatureZorder is more than ARMATURE_MAX_COUNT, we will find from 0
     */
    if(m_siMaxArmatureZorder >= ARMATURE_MAX_COUNT)
    {
        m_siMaxArmatureZorder = 0;
    }
    
    /*
     *  Stop until find out a index is not used. 
     */
    while (m_sArmatureMap->objectForKey(m_siMaxArmatureZorder) != NULL) {
        m_siMaxArmatureZorder ++;
    }
    
	m_fInternalZOrder = m_siMaxArmatureZorder;
    
    cocos2d::CCLog("Armature [%s] internal zorder : %f", m_strName.c_str(), m_fInternalZOrder);
    
    m_sArmatureMap->setObject(this, m_fInternalZOrder);
}

void Armature::setOpacity(GLubyte value)
{
	CCObject *object = NULL;
	CCARRAY_FOREACH(m_pBoneList, object)
	{
		Bone *bone = (Bone*)object;
		bone->setOpacity(value);
	}
}
void Armature::setColor(const ccColor3B& value)
{
	CCObject *object = NULL;
	CCARRAY_FOREACH(m_pBoneList, object)
	{
		Bone *bone = (Bone*)object;
		bone->setColor(m_sColor);
	}
}

void Armature::setOpacityModifyRGB(bool bValue)
{
	m_bOpacityModifyRGB = bValue;
}
bool Armature::isOpacityModifyRGB(void)
{
	return m_bOpacityModifyRGB;
}
    
void Armature::sortBoneHelper(int baseVertexz, int &index)
{
    int i,j,length = m_pBoneList->data->num;
    Bone **x = (Bone**)m_pBoneList->data->arr;
    Bone *tempItem;
    
    // insertion sort
    for(i=1; i<length; i++)
    {
        tempItem = x[i];
        j = i-1;
        
        int tempItemZ = tempItem->getZOrder();
        int jZ = x[j]->getZOrder();
        
        //continue moving element downwards while zOrder is smaller or when zOrder is the same but mutatedIndex is smaller
        while(j>=0 && ( tempItemZ<jZ || tempItemZ==jZ  ) )
        {
            x[j+1] = x[j];
            j = j-1;
     
            if (j >= 0) {
                jZ = x[j]->getZOrder();
            }
        }
        x[j+1] = tempItem;
    }
    
    // sort children children recursively
    for (i=0; i<length; i++)
    {
        if(x[i]->getChildArmature() != NULL)
        {
            x[i]->getChildArmature()->sortBoneHelper(baseVertexz, index);
            continue;
        }

		x[i]->_setActualZorder(baseVertexz + index++);
        
		cocos2d::CCLog("%s : actual zorder - %f", x[i]->getName().c_str(), x[i]->getActualZorder());
                
    }
    
}

CCRect Armature::getBoundingBox()
{
	float minx, miny, maxx, maxy = 0;

	bool first = true;

	CCRect boundingBox = CCRectMake(0, 0, 0, 0);

	CCObject *object = NULL;
	CCARRAY_FOREACH(m_pBoneList, object)
	{
		Bone *bone = (Bone*)object;
		CCRect r = bone->getDisplayManager()->getBoundingBox();
		

		if(first)
		{
			minx = r.getMinX();
			miny = r.getMinY();
			maxx = r.getMaxX();
			maxy = r.getMaxY();

			first = false;
		}
		else
		{
			minx = r.getMinX() < boundingBox.getMinX() ? r.getMinX() : boundingBox.getMinX();
			miny = r.getMinY() < boundingBox.getMinY() ? r.getMinY() : boundingBox.getMinY();
			maxx = r.getMaxX() > boundingBox.getMaxX() ? r.getMaxX() : boundingBox.getMaxX();
			maxy = r.getMaxY() > boundingBox.getMaxY() ? r.getMaxY() : boundingBox.getMaxY();
		}
		

		boundingBox.setRect(minx, miny, maxx - minx, maxy - miny);
	}

	return boundingBox;
}

Bone *Armature::getBoneAtPoint(float _x, float _y)
{
    int _length = m_pBoneList->data->num;
    Bone **_bs = (Bone**)m_pBoneList->data->arr;
    
    for(int i = _length-1; i>=0; i--)
    {
        if(_bs[i]->getDisplayManager()->containPoint(_x, _y))
        {
            return _bs[i];
        }
    }    
    return NULL;
}
    
void Armature::setRenderType(RENDER_TYPE _renderType)
{
    m_eRenderType = _renderType;
    
    ArmatureFileInfo *fileInfo = ArmatureDataManager::sharedArmatureDataManager()->getArmatureFileInfo(m_strName.c_str());
    for (std::vector<ImageInfo>::iterator it = fileInfo->imageInfoVector.begin(); it != fileInfo->imageInfoVector.end(); it++)
    {
        CCLOG(it->imagePath.c_str());
        BatchNode  *batchNode = BatchNodeManager::sharedBatchNodeManager()->getBatchNode(it->imagePath);
        batchNode->setRenderType((RENDER_TYPE)m_eRenderType);
    }
    
    cocos2d::CCLog("%s  change rendertype to %i", m_strName.c_str(), (int)m_eRenderType);
}
    
RENDER_TYPE Armature::getRenderType()
{
    return m_eRenderType;
}

}