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

#ifndef COCOSTUDIO_CSNODE_H
#define COCOSTUDIO_CSNODE_H

#include "cocos2d.h"

#include "reader.h"
#include "CSJsonData.h"
#include "CSArmatureDefine.h"

using namespace cocos2d;

namespace cs {

/**
 * the base node include a lot of attribute.
 */
class CS_EXTERN Node : public CCObject, public JsonData
{
public:
    static Node *createWithJson(const char *pszDescription);
	static Node *create();
public:
	Node();
	~Node(void);

    virtual bool initWithJson(const char *pszDescription);

    /*
     * Update data's json value
     */
    virtual void updateJson();


    /*
     * Copy datas from node
     * @param  node A Node to copy datas
     */
	virtual void copy(const Node *_node);

    /*
     * Calculate two Node's between value(_to - _from) and set to self
     *
     * @param  from   from Node
     * @param  to     to Node
     */
	virtual void subtract(Node *_from, Node *_to);


public:

	
    /**
     * m_fX m_fY m_fSkewX m_fSkewY m_fScaleX m_fScaleY used to calculate transform matrix
     * m_fSkewX, m_fSkewY can have rotation effect
     * To get more matrix information, you can have a look at this pape : http://www.senocular.com/flash/tutorials/transformmatrix/
     */
	float m_fSkewX;
	float m_fSkewY;
	float m_fScaleX;
	float m_fScaleY;
    
    float m_fTweenRotate;		//! SkewX, SkewY, and TweenRotate effect the rotation

	float m_bUseColorInfo;		//! Whether or not this frame have the color changed Info
	int m_iA, m_iR, m_iG, m_iB;

	float m_fX;					//! position x attribute
	float m_fY;					//! position y attribute
	int m_iZOrder;				//! zorder attribute, used to order the Bone's depth order
};


}

#endif
