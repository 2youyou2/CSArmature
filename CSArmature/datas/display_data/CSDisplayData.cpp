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

#include "CSDisplayData.h"
#include "CSConstValue.h"

namespace cs {

DisplayData *DisplayData::createWithJson(const char *pszDescription)
{
    DisplayData *pDisplayData = new DisplayData();
    if (pDisplayData && pDisplayData->initWithJson(pszDescription))
    {
        pDisplayData->autorelease();
        return pDisplayData;
    }
    CC_SAFE_DELETE(pDisplayData);
    return NULL;
}
    
DisplayData *DisplayData::create()
{
	DisplayData *pDisplayData = new DisplayData();
	if (pDisplayData && pDisplayData->init()){
		pDisplayData->autorelease();
		return pDisplayData;
	}
	CC_SAFE_DELETE(pDisplayData);
	return NULL;
}
    
const char *DisplayData::changeDisplayToTexture(const char * displayName)
{
    // remove .xxx
    std::string _textureName = displayName;
    size_t startPos = _textureName.find_last_of(".");
    
    if(startPos != std::string::npos)
    {
        _textureName = _textureName.erase(startPos);
    }
    
    return _textureName.c_str();
}

DisplayData::DisplayData(void)
    :m_eDisplayType(CS_DISPLAY_SPRITE)
{
}


DisplayData::~DisplayData(void)
{
}
    
bool DisplayData::init()
{
    return initWithJson(NULL);
}
 
void DisplayData::updateJson()
{
    JsonData::updateJson();
    
    m_JsonDic.insertItem(A_DISPLAY_TYPE, m_eDisplayType);
}
    
bool DisplayData::initWithJson(const char *pszDescription)
{
    bool ret = false;
    
    do {
        
        
		JsonData::initWithJson(pszDescription);
        
        m_eDisplayType = (DisplayType)m_JsonDic.getItemIntValue(A_DISPLAY_TYPE, CS_DISPLAY_SPRITE);
        
        ret = true;
    } while (0);
    
    
    return ret;
}
    

}