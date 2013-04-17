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

#include "CSSpriteDisplayData.h"
#include "CSConstValue.h"
#include "CSDisplayFactory.h"

namespace cs {

    
SpriteDisplayData *SpriteDisplayData::createWithJson(const char *pszDescription)
{
    SpriteDisplayData *pDisplayData = new SpriteDisplayData();
    if (pDisplayData && pDisplayData->initWithJson(pszDescription))
    {
        pDisplayData->autorelease();
        return pDisplayData;
    }
    CC_SAFE_DELETE(pDisplayData);
    return NULL;
}
    
SpriteDisplayData *SpriteDisplayData::create()
{
	SpriteDisplayData *pDisplayData = new SpriteDisplayData();
	if (pDisplayData && pDisplayData->init()){
		pDisplayData->autorelease();
		return pDisplayData;
	}
	CC_SAFE_DELETE(pDisplayData);
	return NULL;
}
    


SpriteDisplayData::SpriteDisplayData(void)
    :m_strDisplayName("")
    //,m_strImageName("")
{
    m_eDisplayType = CS_DISPLAY_SPRITE;
}

SpriteDisplayData::~SpriteDisplayData()
{
}

bool SpriteDisplayData::init()
{
    return true;
}
 
void SpriteDisplayData::updateJson()
{
    DisplayData::updateJson();
    
    m_JsonDic.insertItem(A_NAME, m_strDisplayName.c_str());
}

bool SpriteDisplayData::initWithJson(const char *pszDescription)
{
    bool ret = false;
    
    do {
        CC_BREAK_IF(!DisplayData::initWithJson(pszDescription));
        
        const char * str = m_JsonDic.getItemStringValue(A_NAME);
        if(str != NULL)
        {
            m_strDisplayName = str;
        }
        
        ret = true;
    } while (0);
    
    
    return ret;
}

void SpriteDisplayData::copy(SpriteDisplayData *_displayData)
{
    m_strDisplayName = _displayData->m_strDisplayName;
    //m_strImageName = _displayData->m_strImageName;
    m_eDisplayType = _displayData->m_eDisplayType;
}

    

}