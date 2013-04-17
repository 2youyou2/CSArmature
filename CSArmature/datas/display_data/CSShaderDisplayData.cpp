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

#include "CSShaderDisplayData.h"
#include "CSConstValue.h"
#include "CSDisplayFactory.h"

namespace cs {

ShaderDisplayData *ShaderDisplayData::createWithJson(const char *pszDescription)
{
    ShaderDisplayData *pDisplayData = new ShaderDisplayData();
    if (pDisplayData && pDisplayData->initWithJson(pszDescription))
    {
        pDisplayData->autorelease();
        return pDisplayData;
    }
    CC_SAFE_DELETE(pDisplayData);
    return NULL;
}
    
ShaderDisplayData *ShaderDisplayData::create()
{
	ShaderDisplayData *pDisplayData = new ShaderDisplayData();
	if (pDisplayData && pDisplayData->init()){
		pDisplayData->autorelease();
		return pDisplayData;
	}
	CC_SAFE_DELETE(pDisplayData);
	return NULL;
}
    


ShaderDisplayData::ShaderDisplayData(void)
    :m_strVert("")
    ,m_strFrag("")
{
    m_eDisplayType = CS_DISPLAY_SHADER;
    
//    CREATE_JS_OBJECT(ShaderDisplayData);
}

//ShaderDisplayData::ShaderDisplayData(DEFINE_JS obj)
//    :m_strVert("")
//    ,m_strFrag("")
//{
//	 m_eDisplayType = CS_DISPLAY_SHADER;
//}

    
bool ShaderDisplayData::init()
{
    return true;
}
 
void ShaderDisplayData::updateJson()
{
    DisplayData::updateJson();
    
    m_JsonDic.insertItem(A_VERT, m_strVert.c_str());
	m_JsonDic.insertItem(A_FRAG, m_strFrag.c_str());
}

bool ShaderDisplayData::initWithJson(const char *pszDescription)
{
    bool ret = false;
    
    do {
        CC_BREAK_IF(!DisplayData::initWithJson(pszDescription));
        
        const char * str = m_JsonDic.getItemStringValue(A_VERT);
        if(str != NULL)
        {
            m_strVert = str;
        }

		str = m_JsonDic.getItemStringValue(A_FRAG);
        if(str != NULL)
        {
            m_strFrag = str;
        }
        
        ret = true;
    } while (0);
    
    
    return ret;
}

void ShaderDisplayData::copy(ShaderDisplayData *_displayData)
{
    m_strVert = _displayData->m_strVert;
    m_strFrag = _displayData->m_strFrag;
    m_eDisplayType = _displayData->m_eDisplayType;
}

  
}