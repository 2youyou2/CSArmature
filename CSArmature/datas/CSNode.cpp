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

#include "CSNode.h"
#include "CSUtilMath.h"
#include "reader.h"
#include "CSConstValue.h"

namespace cs {


Node *Node::createWithJson(const char *pszDescription)
{
    Node *pBaseNode = new Node();
	if (pBaseNode && pBaseNode->initWithJson(pszDescription))
    {
        pBaseNode->autorelease();
        return pBaseNode;
    }
    CC_SAFE_DELETE(pBaseNode);
    return NULL;
}
    
Node *Node::create()
{
    Node *pBaseNode = new Node();
    if (pBaseNode)
    {
        pBaseNode->autorelease();
        return pBaseNode;
    }
    CC_SAFE_DELETE(pBaseNode);
    return NULL;
}


Node::Node()
    :m_fX(0.0f)
    ,m_fY(0.0f)
    ,m_iZOrder(0)

    ,m_fSkewX(0.0f)
    ,m_fSkewY(0.0f)
    ,m_fScaleX(1.0f)
    ,m_fScaleY(1.0f)

    ,m_fTweenRotate(0.0f)

	,m_bUseColorInfo(false)
	,m_iA(255)
	,m_iR(255)
	,m_iG(255)
	,m_iB(255)
{
}

Node::~Node()
{
}


void Node::copy(const Node* node )
{
	m_fX = node->m_fX;
	m_fY = node->m_fY;
	m_iZOrder = node->m_iZOrder;

	m_fScaleX = node->m_fScaleX;
	m_fScaleY = node->m_fScaleY;
	m_fSkewX = node->m_fSkewX;
	m_fSkewY = node->m_fSkewY;

	m_fTweenRotate = node->m_fTweenRotate;

	m_bUseColorInfo = node->m_bUseColorInfo;
	m_iR = node->m_iR;
	m_iG = node->m_iG;
	m_iB = node->m_iB;
	m_iA = node->m_iA;
}


void Node::subtract(Node *from, Node *to)
{
    m_fX = to->m_fX - from->m_fX;
    m_fY = to->m_fY - from->m_fY;
    m_fScaleX = to->m_fScaleX - from->m_fScaleX;
    m_fScaleY = to->m_fScaleY - from->m_fScaleY;
    m_fSkewX = to->m_fSkewX - from->m_fSkewX;
    m_fSkewY = to->m_fSkewY - from->m_fSkewY;

	if(from->m_bUseColorInfo || to->m_bUseColorInfo)
	{
		m_iA = to->m_iA - from->m_iA;
		m_iR = to->m_iR - from->m_iR;
		m_iG = to->m_iG - from->m_iG;
		m_iB = to->m_iB - from->m_iB;

		m_bUseColorInfo = true;
	}


    if (m_fSkewX > CS_PI)
    {
        m_fSkewX -= (float)CS_DOUBLE_PI;
    }
    if (m_fSkewX < -CS_PI)
    {
        m_fSkewX += (float)CS_DOUBLE_PI;
    }

    if (m_fSkewY > CS_PI)
    {
        m_fSkewY -= (float)CS_DOUBLE_PI;
    }
    if (m_fSkewY < -CS_PI)
    {
        m_fSkewY += (float)CS_DOUBLE_PI;
    }


    if (to->m_fTweenRotate)
    {
        m_fSkewX += to->m_fTweenRotate;
        m_fSkewY -= to->m_fTweenRotate;
    }
}
  
    
void Node::updateJson()
{
    JsonData::updateJson();
    
    m_JsonDic.insertItem(A_X, m_fX);
    m_JsonDic.insertItem(A_Y, m_fY);
    m_JsonDic.insertItem(A_Z, m_iZOrder);

    m_JsonDic.insertItem(A_SKEW_X, m_fSkewX);
    m_JsonDic.insertItem(A_SKEW_Y, m_fSkewY);
    m_JsonDic.insertItem(A_SCALE_X, m_fScaleX);
    m_JsonDic.insertItem(A_SCALE_Y, m_fScaleY);

	if(m_bUseColorInfo)
	{
		CSJsonDictionary jsonDic;
		jsonDic.initWithDescription("");

		jsonDic.insertItem(A_ALPHA, m_iA);
		jsonDic.insertItem(A_RED, m_iR);
		jsonDic.insertItem(A_GREEN, m_iG);
		jsonDic.insertItem(A_BLUE, m_iB);

		m_JsonDic.insertItemToArray(COLOR_INFO, &jsonDic);
	}
    
}


bool Node::initWithJson(const char *pszDescription)
{
    bool ret = false;
    
    do {
        JsonData::initWithJson(pszDescription);
        
        
        m_fX = m_JsonDic.getItemFloatValue(A_X, 0);
        m_fY = m_JsonDic.getItemFloatValue(A_Y, 0);
        m_iZOrder = m_JsonDic.getItemIntValue(A_Z, 0);

        m_fSkewX = m_JsonDic.getItemFloatValue(A_SKEW_X, 0);
        m_fSkewY = m_JsonDic.getItemFloatValue(A_SKEW_Y, 0);
        m_fScaleX = m_JsonDic.getItemFloatValue(A_SCALE_X, 1);
        m_fScaleY = m_JsonDic.getItemFloatValue(A_SCALE_Y, 1);

		
		CSJsonDictionary *colorDic = m_JsonDic.getSubItemFromArray(COLOR_INFO, 0);

		if (colorDic)
		{
			m_iA = colorDic->getItemIntValue(A_ALPHA, 255);
			m_iR = colorDic->getItemIntValue(A_RED, 255);
			m_iG = colorDic->getItemIntValue(A_GREEN, 255);
			m_iB = colorDic->getItemIntValue(A_BLUE, 255);

			m_bUseColorInfo = true;
		}

		ret = true;
    } while (0);
    
    
    return ret;
}

}