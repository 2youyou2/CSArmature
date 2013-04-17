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

#include "CSJsonData.h"
#include "cocos2d.h"
using namespace cocos2d;

namespace cs {

    
bool JsonData::initWithJson(const char *pszDescription)
{
    m_JsonDic.initWithDescription(pszDescription);
    return true;
}
    
std::string &JsonData::getJsonDescription()
{
    updateJson();
    m_strJson = m_JsonDic.getDescription();
    return m_strJson;
}


CSJsonDictionary &JsonData::getJsonDic()
{
    updateJson();
    m_strJson = m_JsonDic.getDescription();
    return m_JsonDic;
}

void JsonData::updateJson()
{
    m_JsonDic.initWithDescription("");
}

void JsonData::clone(JsonData *jsonData)
{
	initWithJson(jsonData->getJsonDescription().c_str());
}

}