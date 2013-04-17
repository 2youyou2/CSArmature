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

#include "CSAnimationData.h"
#include "CSConstValue.h"

namespace cs {

AnimationData *AnimationData::createWithJson(const char *pszDescription)
{
    AnimationData *pArmatureAniData = new AnimationData();
    if (pArmatureAniData && pArmatureAniData->initWithJson(pszDescription))
    {
        pArmatureAniData->autorelease();
        return pArmatureAniData;
    }
    CC_SAFE_DELETE(pArmatureAniData);
    return NULL;
}
    
AnimationData *AnimationData::create()
{
    AnimationData *pArmatureAniData = new AnimationData();
    if (pArmatureAniData && pArmatureAniData->init())
    {
        pArmatureAniData->autorelease();
        return pArmatureAniData;
    }
    CC_SAFE_DELETE(pArmatureAniData);
    return NULL;
}

AnimationData::AnimationData(void)
{
}


AnimationData::~AnimationData(void)
{
    CC_SAFE_DELETE( m_pMovDataDic );
}

bool AnimationData::init()
{
    return initWithJson(NULL);
}

    
MovementData *AnimationData::addMovement(const char *movName, const char *pszDescription)
{
    MovementData *movData = MovementData::createWithJson(pszDescription);
    
    if (movData->getName().compare("") == 0)
    {
        CCAssert(movName != NULL, "you should provide a valid name for this movement");
        
        movData->setName(movName);
    }
    
    addMovement(movData);
    return movData;
}

    
MovementData *AnimationData::addMovement(MovementData* movData)
{
    m_pMovDataDic->setObject(movData, movData->getName());
    m_vecMovNames.push_back(movData->getName());
    
    connectMovementDataSignal(movData);
    return movData;
}
    
MovementData *AnimationData::removeMovement(const char* movName)
{
	std::vector<std::string>::iterator it = m_vecMovNames.begin();
	for(; it != m_vecMovNames.end(); it++)
	{
		if(it->compare(movName) == 0)
		{
			break;
		}
	}
	if(it != m_vecMovNames.end())
	{
		m_vecMovNames.erase(it);
	}

	MovementData *movData = getMovement(movName);
	m_pMovDataDic->removeObjectForKey(movName);

	return movData;
}

MovementData *AnimationData::getMovement(const char* movName)
{
    return (MovementData *)m_pMovDataDic->objectForKey( movName );
}

CCDictionary *AnimationData::getMovementDic()
{
    return m_pMovDataDic;
}

int AnimationData::getMovementCount()
{
    return m_pMovDataDic->count();
}

std::vector<std::string>  &AnimationData::getMovNames()
{
    return m_vecMovNames;
}


void AnimationData::updateJson()
{
    JsonData::updateJson();
    
    m_JsonDic.insertItem(A_NAME, m_strName.c_str());
    
    if(m_pMovDataDic->count() <= 0)
    {
        return;
    }
    
    Json::Value array;
    
    
    CCDictElement *_element = NULL;
    CCDICT_FOREACH(m_pMovDataDic, _element)
    {
        MovementData *movementData = (MovementData*)_element->getObject();
        m_JsonDic.insertItemToArray(MOVEMENT_DATA, &movementData->getJsonDic());
    }
}

bool AnimationData::initWithJson(const char *pszDescription)
{
    bool ret = false;
    
    do {
        
        m_pMovDataDic = new CCDictionary();
        CCAssert(m_pMovDataDic, "create AnimationData::m_pMovDataDic fail!");
        
        JsonData::initWithJson(pszDescription);
        
        const char * str = m_JsonDic.getItemStringValue(A_NAME);
        if(str != NULL)
        {
            m_strName = str;
        }
        
        int _length = m_JsonDic.getArrayItemCount(MOVEMENT_DATA);
        
        for (int i = 0; i<_length; i++)
        {
            CSJsonDictionary *_dic = m_JsonDic.getSubItemFromArray(MOVEMENT_DATA, i);
            
            addMovement(NULL, _dic->getDescription().c_str());
        }
        
        ret = true;
    } while (0);
    
    
    return ret;
}

void AnimationData::connectMovementDataSignal(MovementData *movementData)
{
    movementData->NameChanedSignal.connect(this, &AnimationData::movementNameChanged);
}

void AnimationData::disconnectMovementDataSignal(MovementData *movementData)
{
    movementData->NameChanedSignal.disconnect(this);
}

void AnimationData::movementNameChanged(MovementData *movData, const char *movName)
{
	const char *originMovnName = movData->getName().c_str();

	movData->retain();
	m_pMovDataDic->removeObjectForKey(originMovnName);
	m_pMovDataDic->setObject(movData, movName);
	movData->release();

	std::vector<std::string>::iterator it = m_vecMovNames.begin();
	for(; it != m_vecMovNames.end(); it++)
	{
		if(it->compare(movName) == 0)
		{
			*it = movName;
			break;
		}
	}
}

}