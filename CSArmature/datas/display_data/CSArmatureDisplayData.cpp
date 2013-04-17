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

#include "CSArmatureDisplayData.h"
#include "CSConstValue.h"
#include "CSDisplayFactory.h"

namespace cs {

ArmatureDisplayData *ArmatureDisplayData::createWithJson(const char *pszDescription)
{
    ArmatureDisplayData *pDisplayData = new ArmatureDisplayData();
    if (pDisplayData && pDisplayData->initWithJson(pszDescription))
    {
        pDisplayData->autorelease();
        return pDisplayData;
    }
    CC_SAFE_DELETE(pDisplayData);
    return NULL;
}
    
ArmatureDisplayData *ArmatureDisplayData::create()
{
	ArmatureDisplayData *pDisplayData = new ArmatureDisplayData();
	if (pDisplayData && pDisplayData->init()){
		pDisplayData->autorelease();
		return pDisplayData;
	}
	CC_SAFE_DELETE(pDisplayData);
	return NULL;
}


    
ArmatureDisplayData::ArmatureDisplayData(void)
    :m_strDisplayName("")
{
    m_eDisplayType = CS_DISPLAY_ARMATURE;

}

ArmatureDisplayData::~ArmatureDisplayData()
{
}
    
bool ArmatureDisplayData::init()
{
    
    return true;
}
 
void ArmatureDisplayData::updateJson()
{
    JsonData::updateJson();
    
    m_JsonDic.insertItem(A_NAME, m_strDisplayName.c_str());
}
    
bool ArmatureDisplayData::initWithJson(const char *pszDescription)
{
    bool ret = false;
    
    do {
        JsonData::initWithJson(pszDescription);
        
        const char * str = m_JsonDic.getItemStringValue(A_NAME);
        if(str != NULL)
        {
            m_strDisplayName = str;
        }
        
        ret = true;
    } while (0);
    
    
    return ret;
}
    
void ArmatureDisplayData::copy(ArmatureDisplayData *_displayData)
{
    m_strDisplayName = _displayData->m_strDisplayName;
    m_eDisplayType = _displayData->m_eDisplayType;
}
 
}