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

#include "CSMovementData.h"
#include "CSConstValue.h"
#include "CSEditorBone.h"

using namespace std;

namespace cs {

MovementData *MovementData::createWithJson(const char *pszDescription)
{
    MovementData *pMovementData = new MovementData();
    if (pMovementData->initWithJson(pszDescription))
    {
        pMovementData->autorelease();
        return pMovementData;
    }
    CC_SAFE_DELETE(pMovementData);
    return NULL;
}

MovementData *MovementData::create()
{
    MovementData *pMovementData = new MovementData();
    if (pMovementData->init())
    {
        pMovementData->autorelease();
        return pMovementData;
    }
    CC_SAFE_DELETE(pMovementData);
    return NULL;
}

MovementData::MovementData(void)
    :m_pMovBoneDataDic(NULL)
    ,m_pMovFrameDataArr(NULL)
    ,m_strName("")
    ,m_iDuration(0)
    ,m_iDurationTo(0)
    ,m_iDurationTween(0)
    ,m_bLoop(true)
    ,m_eTweenEasing(Linear)
{
}


MovementData::~MovementData(void)
{
    CC_SAFE_DELETE( m_pMovBoneDataDic );
    CC_SAFE_DELETE( m_pMovFrameDataArr );
}

bool MovementData::init()
{
    return initWithJson(NULL);
}

void MovementData::updateJson()
{
    JsonData::updateJson();
    
    
    m_JsonDic.insertItem(A_LOOP, m_bLoop);
    m_JsonDic.insertItem(A_DURATION_TWEEN, m_iDurationTween);
    m_JsonDic.insertItem(A_DURATION_TO, m_iDurationTo);
    m_JsonDic.insertItem(A_DURATION, m_iDuration);
    m_JsonDic.insertItem(A_TWEEN_EASING, m_eTweenEasing);
    m_JsonDic.insertItem(A_NAME, m_strName.c_str());
    
    if(m_pMovBoneDataDic->count() <= 0)
    {
        return;
    }
    
    Json::Value array;
    
    CCDictElement *element = NULL;
    CCDICT_FOREACH(m_pMovBoneDataDic, element)
    {
        MovementBoneData *movBoneData = (MovementBoneData*)element->getObject();
        m_JsonDic.insertItemToArray(MOVEMENT_BONE_DATA, &movBoneData->getJsonDic());
    }
    
}

bool MovementData::initWithJson(const char *pszDescription)
{
    bool ret = false;
    
    do {
        CC_SAFE_DELETE(m_pMovBoneDataDic);
        m_pMovBoneDataDic = new CCDictionary();
        CCAssert(m_pMovBoneDataDic, "create MovementData::m_pMovBoneDataDic fail!");
        
        CC_SAFE_DELETE(m_pMovFrameDataArr);
        m_pMovFrameDataArr = new CCArray();
        CCAssert(m_pMovFrameDataArr, "create MovementData::m_pMovFrameDataArr fail!");
        
        
        JsonData::initWithJson(pszDescription);
        
        
        m_bLoop = m_JsonDic.getItemBoolvalue(A_LOOP, true);
        m_iDurationTween = m_JsonDic.getItemIntValue(A_DURATION_TWEEN, 0);
        m_iDurationTo = m_JsonDic.getItemIntValue(A_DURATION_TO, 0);
        m_iDuration = m_JsonDic.getItemIntValue(A_DURATION, 0);
        m_eTweenEasing = (TweenType)m_JsonDic.getItemIntValue(A_TWEEN_EASING, Linear);
        
        const char * str = m_JsonDic.getItemStringValue(A_NAME);
        if(str != NULL)
        {
            m_strName = str;
        }
        
        
        int _length = m_JsonDic.getArrayItemCount(MOVEMENT_BONE_DATA);
        
        for (int i = 0; i<_length; i++)
        {
            CSJsonDictionary *_dic = m_JsonDic.getSubItemFromArray(MOVEMENT_BONE_DATA, i);
            
            MovementBoneData *movBoneData = MovementBoneData::createWithJson(_dic->getDescription().c_str());
            
            m_pMovBoneDataDic->setObject(movBoneData, movBoneData->getName().c_str());
        }
        
        ret = true;
    } while (0);
    
    
    return ret;
}
    
void MovementData::setName(const std::string &var)
{
	NameChanedSignal.emit(this, var.c_str());
    m_strName = var;
}

const std::string &MovementData::getName()
{
    return m_strName;
}
    
MovementBoneData *MovementData::addMovementBoneData(Bone *bone, MovementBoneData *movBoneData)
{
    
    if (movBoneData->getName().compare("") == 0 &&  bone)
    {
        movBoneData->setName(bone->getName());
    }
    
    m_pMovBoneDataDic->setObject(movBoneData, movBoneData->getName());
    
    m_MovBoneDataVec.push_back(movBoneData);
    movBoneDataDurationChanged();
    
    connectBoneSignal(bone, movBoneData);
    connectMovBoneDataSignal(movBoneData);
    
    return movBoneData;
}
    
MovementBoneData *MovementData::addMovementBoneData(Bone *bone, const char *pszDescription)
{
    MovementBoneData *movBoneData = MovementBoneData::createWithJson(pszDescription);
   
    addMovementBoneData(bone, movBoneData);
    
    return movBoneData;
}
    
MovementBoneData *MovementData::removeMovementBoneData(const char *boneName)
{
    MovementBoneData *movBoneData = getMovementBoneData(boneName);
    if(!movBoneData)
    {
        return NULL;
    }
    m_pMovBoneDataDic->removeObjectForKey(boneName);
    
    std::vector<MovementBoneData*>::iterator it;
    
    for ( it= m_MovBoneDataVec.begin(); it != m_MovBoneDataVec.end(); it++)
    {
        if (*it == movBoneData)
        {
            break;
        }
    }
    m_MovBoneDataVec.erase(it);
    
    movBoneDataDurationChanged();
    
    
    return movBoneData;
}

MovementBoneData *MovementData::getMovementBoneData(const char *boneName)
{
    return (MovementBoneData*)(m_pMovBoneDataDic->objectForKey(boneName));
}

void MovementData::addMovFrameData(MovementFrameData *movFrameData)
{
	m_pMovFrameDataArr->addObject(movFrameData);
}

MovementFrameData *MovementData::getMoveFrameData(int index)
{
	return (MovementFrameData*)m_pMovFrameDataArr->objectAtIndex(index);
}
    
CCDictionary *MovementData::getMovBoneDataDic()
{
    return m_pMovBoneDataDic;
}
    
CCArray *MovementData::getMovFrameDataArr()
{
    return m_pMovFrameDataArr;
}
    
void MovementData::connectMovBoneDataSignal(MovementBoneData *movBoneData)
{
    movBoneData->DurationChangedSignal.connect(this, &MovementData::movBoneDataDurationChanged);
    
}

void MovementData::disconnectMovBoneDataSignal(MovementBoneData *movBoneData)
{
    movBoneData->DurationChangedSignal.disconnect(this);
}

void MovementData::movBoneDataDurationChanged()
{
    
    if(m_MovBoneDataVec.size() > 0)
    {
        qsort(&m_MovBoneDataVec[0], m_MovBoneDataVec.size(), sizeof(MovementBoneData*), &MovementData::biggerDuration);
        m_iDuration = m_MovBoneDataVec.back()->getDuration();
    }
    else
    {
        m_iDuration = 0;
    }
    
    
}
    
void MovementData::connectBoneSignal(Bone *bone, MovementBoneData *movBoneData)
{
    if(!bone || !movBoneData)
    {
        return;
    }
    
	if (dynamic_cast<EditorBone*>(bone) != 0)
	{
		((EditorBone*)bone)->NameChanedSignal.connect(this, &MovementData::boneNameChanged);
	}
}

void MovementData::disconnectBoneSignal(Bone *bone)
{
    
}

void MovementData::boneNameChanged(Bone *bone, const char *boneName)
{
    const char *originBoneName = bone->getName().c_str();
    MovementBoneData *movBoneData = (MovementBoneData*)m_pMovBoneDataDic->objectForKey(originBoneName);
    movBoneData->setName(boneName);
   
	movBoneData->retain();
	m_pMovBoneDataDic->removeObjectForKey(originBoneName);
	m_pMovBoneDataDic->setObject(movBoneData, boneName);
	movBoneData->release();
    
}

int MovementData::biggerDuration(const void *a, const void *b)
{
    MovementBoneData *aMovBoneData = *((MovementBoneData**)a);
    MovementBoneData *bMovBoneData = *((MovementBoneData**)b);
    
    int aDuration = aMovBoneData->getDuration();
    int bDuration = bMovBoneData->getDuration();
    
    if(aDuration > bDuration)
        return 1;
    else if(aDuration < bDuration)
        return -1;
    return 0;
        
}

}