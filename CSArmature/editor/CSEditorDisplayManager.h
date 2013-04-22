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

#ifndef COCOSTUDIO_CSEDITORDISPLAYMANAGER_H
#define COCOSTUDIO_CSEDITORDISPLAYMANAGER_H

#include "cocos2d.h"
#include "CSDecorativeDisplay.h"
#include "CSDatas.h"
#include "CSDisplayManager.h"
#include "sigslot.h"

using namespace cocos2d;

namespace cs {
    
class EditorBone;

//! DisplayManager manages Bone's display
class CS_EXTERN EditorDisplayManager : public DisplayManager
{
public:
    static EditorDisplayManager *create(Bone *bone);
    
public:
    EditorDisplayManager();
    ~EditorDisplayManager();
    
    
    
    void addEditDisplay(DisplayData *displayData, int index);
    void insertEditDisplay(DisplayData *displayData, int index);
    void removeEditDisplay(int index);
    
	void editAnchorPoint(CCPoint &p, bool moveCenterAnchorPoint = true, bool restoreAnchorPoint = false);
    
public:
    /**
     *  @brief   check if the position is inside the bone.
     */
    virtual bool containPoint(CCPoint &point);
    
    /**
     *  @brief   check if the position is inside the bone.
     */
    virtual bool containPoint(float x, float y);
    
	virtual bool rectTest(CCRect &r);

    /**
     *  @brief   If this bone is be clicked, then test whether the anchor points is clicked.
     *           If anchor points is clicked, then return the point type.
     *           If anchor points is not clicked, then return POINT_MIN
     */
    virtual int getClickedContourPointType();

	virtual void setVisible(bool visible);
    
    /**
     *  @brief  set whether or not show the contour points and collider points.
     */
    void showContour(bool show);
    
    /**
     *  @brief  whether or not show the contour points and collider points.
     */
    bool isShowContour();
    
	
	/**
     *  @brief  Convert point to bone's transform space, and this function will modify the input value.
     */
	void convertPointToSpace(CCPoint &p);

public:
	sigslot::signal3<Bone*, CCPoint, bool> AnchorPointChangeTemporarySignal;
	sigslot::signal1<bool> VisibleChangedSignal;
};

}

#endif
