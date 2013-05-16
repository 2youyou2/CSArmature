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

#ifndef COCOSTUDIO_CSCOLLIDERDETECTOR_H
#define COCOSTUDIO_CSCOLLIDERDETECTOR_H

#include "cocos2d.h"
#include "CSDatas.h"
#include "Box2D/Box2D.h"

namespace cs {

class Bone;

class ColliderBody : public cocos2d::CCObject
{
public:
	ColliderBody(b2Body *b2b, ContourData *contourData)
		:m_pB2b(NULL)
		,m_pContourData(NULL)
	{
		this->m_pB2b = b2b;
		this->m_pContourData = contourData;
		CC_SAFE_RETAIN(m_pContourData);
	}

	~ColliderBody()
	{
		CC_SAFE_RELEASE(m_pContourData);
	}

	inline b2Body *getB2Body()
	{
		return m_pB2b;
	}

	inline ContourData *getContourData()
	{
		return m_pContourData;
	}

private:
	b2Body *m_pB2b;
	ContourData *m_pContourData;
};

/*
 *  @brief  ContourSprite used to draw the contour of the display
 */
class ColliderDetector : public cocos2d::CCObject
{
public:
	static ColliderDetector *create();
    static ColliderDetector *create(Bone *bone);
public:
	ColliderDetector();
	~ColliderDetector(void);
    
    virtual bool init();
	virtual bool init(Bone *bone);
    
    void addContourData(ContourData *contourData);
    void addContourDataList(cocos2d::CCArray *contourDataList);
    
	void removeContourData(ContourData *contourData);
	void removeAll();
    
    void updateTransform(cocos2d::CCAffineTransform &t);

	void setColliderFilter(b2Filter &filter);
    
private:
    cocos2d::CCArray *m_pColliderBodyList;
    
	CC_SYNTHESIZE(Bone*, m_pBone, Bone);

};
		

}

#endif