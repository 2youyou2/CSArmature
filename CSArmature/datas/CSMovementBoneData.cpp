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

#include "CSMovementBoneData.h"
#include "CSConstValue.h"

namespace cs {

MovementBoneData *MovementBoneData::createWithJson(const char *pszDescription)
{
    MovementBoneData *pMovementBoneData = new MovementBoneData();
    if (pMovementBoneData && pMovementBoneData->initWithJson(pszDescription))
    {
        pMovementBoneData->autorelease();
        return pMovementBoneData;
    }
    CC_SAFE_DELETE(pMovementBoneData);
    return NULL;
}
    
MovementBoneData *MovementBoneData::create()
{
    MovementBoneData *pFrameNodeList = new MovementBoneData();
    if (pFrameNodeList && pFrameNodeList->init())
    {
        pFrameNodeList->autorelease();
        return pFrameNodeList;
    }
    CC_SAFE_DELETE(pFrameNodeList);
    return NULL;
}

MovementBoneData::MovementBoneData()
    :m_fDelay(0.0f)
    ,m_fScale(1.0f)
    ,m_iDuration(0)
    ,m_pFrameList(NULL)
    ,m_strName("")
{
}


MovementBoneData::~MovementBoneData(void)
{
	DeletedSignal.emit();

    CC_SAFE_DELETE(m_pFrameList);
}

bool MovementBoneData::init()
{
    return initWithJson(NULL);
}


void MovementBoneData::updateJson()
{
    JsonData::updateJson();
    
    
    m_JsonDic.insertItem(A_MOVEMENT_DELAY, m_fDelay);
    m_JsonDic.insertItem(A_MOVEMENT_SCALE, m_fScale);
    //    m_JsonDic.insertItem(A_DURATION, m_iDuration);
    m_JsonDic.insertItem(A_NAME, m_strName.c_str());
    
    
    if(m_pFrameList->count() <= 0)
    {
        return;
    }
    
    Json::Value array;
    
    CCObject *object = NULL;
    CCARRAY_FOREACH(m_pFrameList, object)
    {
        FrameData *frameData = (FrameData*)object;
        m_JsonDic.insertItemToArray(FRAME_DATA, &frameData->getJsonDic());
    }
    
}


bool MovementBoneData::initWithJson(const char *pszDescription)
{
    bool ret = false;
    
    do {
        
        m_pFrameList = new CCArray();
        CCAssert(m_pFrameList->init(), "create MovementBoneData::m_pFrameList fail!");
        
        if(pszDescription == NULL)
        {
            return true;
        }
        
        JsonData::initWithJson(pszDescription);
        
        m_fDelay = m_JsonDic.getItemFloatValue(A_MOVEMENT_DELAY, 0);
        m_fScale = m_JsonDic.getItemFloatValue(A_MOVEMENT_SCALE, 1);
        //        m_iDuration = m_JsonDic.getItemFloatValue(A_DURATION, 0);
        
        const char * str = m_JsonDic.getItemStringValue(A_NAME);
        if(str != NULL)
        {
            m_strName = str;
        }
        
        
        int _length = m_JsonDic.getArrayItemCount(FRAME_DATA);
        
        for (int i = 0; i<_length; i++)
        {
            CSJsonDictionary *dic = m_JsonDic.getSubItemFromArray(FRAME_DATA, i);
            
            FrameData *frameData = FrameData::createWithJson(dic->getDescription().c_str());
            
            addFrameData(frameData);
        }
        
        ret = true;
    } while (0);
    
    
    return ret;
}

FrameData *MovementBoneData::addFrameData(FrameData* frameData)
{
    m_pFrameList->addObject(frameData);
    connectFrameDataSignal(frameData);
    
    m_iDuration += frameData->getDuration();
    
    DurationChangedSignal.emit();
    
    return frameData;
}
    
FrameData *MovementBoneData::addFrameData(const char *pszDescription)
{
    FrameData *frameData = FrameData::createWithJson(pszDescription);
    addFrameData(frameData);
    
    return frameData;
}

FrameData *MovementBoneData::insertFrameDataAtIndex(int index, FrameData* frameData)
{
    m_pFrameList->insertObject(frameData, index);
    
    connectFrameDataSignal(frameData);
    
    m_iDuration += frameData->getDuration();
    
    DurationChangedSignal.emit();
    
    return frameData;
}
    
FrameData *MovementBoneData::insertFrameDataAtIndex(int index, const char *pszDescription)
{
    FrameData *frameData = FrameData::createWithJson(pszDescription);
    insertFrameDataAtIndex(index, frameData);
    
    return frameData;
}

FrameData *MovementBoneData::removeFrameData(int index)
{
    FrameData *frameData = getFrameData(index);
    
    disconnectFrameDataSignal(frameData);
    m_pFrameList->removeObjectAtIndex(index);
    
    m_iDuration -= frameData->getDuration();
    
    DurationChangedSignal.emit();
    
    return frameData;
}

void MovementBoneData::exchangeFrameData(int index, int index2)
{
    m_pFrameList->exchangeObjectAtIndex(index, index2);
}

FrameData* MovementBoneData::getFrameData(int index)
{
    return (FrameData*)m_pFrameList->objectAtIndex(index);
}

CCArray *MovementBoneData::getFrameDatas()
{
    return m_pFrameList;
}

int MovementBoneData::getFrameCount()
{
    return m_pFrameList->count();
}
 
void MovementBoneData::connectFrameDataSignal(FrameData *frameData)
{
    frameData->DurationChangedSignal.connect(this, &MovementBoneData::frameDataDurationChanged);
}
    
void MovementBoneData::disconnectFrameDataSignal(FrameData *frameData)
{
    frameData->DurationChangedSignal.disconnect(this);
}
    
void MovementBoneData::frameDataDurationChanged(int preDurantion, int duration)
{
    m_iDuration = m_iDuration - preDurantion + duration;
    DurationChangedSignal.emit();
}
    

}