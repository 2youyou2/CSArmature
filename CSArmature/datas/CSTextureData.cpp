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

#include "CSTextureData.h"
#include "CSConstValue.h"

namespace cs {

    
TextureData *TextureData::createWithJson(const char *pszDescription)
{
    TextureData *pTextureData = new TextureData();
	if (pTextureData && pTextureData->initWithJson(pszDescription))
    {
        pTextureData->autorelease();
        return pTextureData;
    }
    CC_SAFE_DELETE(pTextureData);
    return NULL;
}
    
TextureData *TextureData::create()
{
	TextureData *pTextureData = new TextureData();
	if (pTextureData && pTextureData->init())
	{
		pTextureData->autorelease();
		return pTextureData;
	}
	CC_SAFE_DELETE(pTextureData);
	return NULL;
}

TextureData::TextureData()
	:m_fHeight(0.0f)
	,m_fWidth(0.0f)
	,m_fPivotX(0.5f)
	,m_fPivotY(0.5f)
	,m_strName("")
{

}


TextureData::~TextureData()
{
    CC_SAFE_DELETE(m_pContourDataList);
}
    
bool TextureData::init()
{
    m_pContourDataList = CCArray::createWithCapacity(3);
    CCAssert(m_pContourDataList, "create m_pContourDataList failed!");
    m_pContourDataList->retain();
    
    return true;
}
    
void TextureData::addContourData(ContourData *contourData)
{
    m_pContourDataList->addObject(contourData);
}


void TextureData::removeContourData(ContourData *contourData)
{
    m_pContourDataList->removeObject(contourData);
}


ContourData *TextureData::getContourData(int index)
{
    return (ContourData*)m_pContourDataList->objectAtIndex(index);
}

CCArray *TextureData::getContourDatas()
{
    return m_pContourDataList;
}
    
int TextureData::getContourCount()
{
    return m_pContourDataList->count();
}
    
void TextureData::updateJson()
{
    JsonData::updateJson();
    
    
    m_JsonDic.insertItem(A_NAME, m_strName.c_str());
    m_JsonDic.insertItem(A_WIDTH, m_fWidth);
    m_JsonDic.insertItem(A_HEIGHT, m_fHeight);
    m_JsonDic.insertItem(A_PIVOT_X, m_fPivotX);
    m_JsonDic.insertItem(A_PIVOT_Y, m_fPivotY);
    
    if(m_pContourDataList->count() <= 0)
    {
        return;
    }
    
    Json::Value array;
    
    CCObject *object = NULL;
    CCARRAY_FOREACH(m_pContourDataList, object)
    {
        ContourData *contourData = (ContourData*)object;
        m_JsonDic.insertItemToArray(CONTOUR_DATA, &contourData->getJsonDic());
    }
    
}
    
bool TextureData::initWithJson(const char *pszDescription)
{
    bool ret = false;
    
    do {
        JsonData::initWithJson(pszDescription);
        CC_BREAK_IF(!TextureData::init());
        
        
        const char * str = m_JsonDic.getItemStringValue(A_NAME);
        if(str != NULL)
        {
            m_strName = str;
        }
        
        m_fWidth = m_JsonDic.getItemFloatValue(A_WIDTH, 0);
        m_fHeight = m_JsonDic.getItemFloatValue(A_HEIGHT, 0);
        m_fPivotX = m_JsonDic.getItemFloatValue(A_PIVOT_X, 0);
        m_fPivotY = m_JsonDic.getItemFloatValue(A_PIVOT_Y, 0);
        
        int length = m_JsonDic.getArrayItemCount(CONTOUR_DATA);
        
        for (int i = 0; i<length; i++)
        {
            CSJsonDictionary *dic = m_JsonDic.getSubItemFromArray(CONTOUR_DATA, i);
            
            ContourData *contourData = ContourData::createWithJson(dic->getDescription().c_str());
            m_pContourDataList->addObject(contourData);
        }
        
        ret = true;
    } while (0);
    
    
    return ret;
}


void TextureData::setPivotX(float pivotX)
{
	m_fPivotX = pivotX;

#if CS_DEBUG_FOR_EDIT
	AnchorPointChangedSignal.emit(m_fPivotX, m_fPivotY);
#endif
}

float TextureData::getPivotX()
{
	return m_fPivotX;
}

void TextureData::setPivotY(float pivotY)
{
	m_fPivotY = pivotY;

#if CS_DEBUG_FOR_EDIT
	AnchorPointChangedSignal.emit(m_fPivotX, m_fPivotY);
#endif
}

float TextureData::getPivotY()
{
	return m_fPivotY;
}


}