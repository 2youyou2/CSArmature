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

#ifndef COCOSTUDIO_CSDISPLAYFACTORY_H
#define COCOSTUDIO_CSDISPLAYFACTORY_H

#include "cocos2d.h"
#include "CSDatas.h"

using namespace cocos2d;

namespace cs {
    
class Bone;
class DecorativeDisplay;
class DisplayData;
    
class DisplayFactory : public CCObject
{
public:
    virtual CCObject *addDisplay(Bone *bone, DecorativeDisplay *decoDisplay, DisplayData *displayData) = 0;
    virtual CCObject *createDisplay(Bone *bone, DecorativeDisplay *displayData) = 0;

    virtual void changeDisplay(Bone *bone, DecorativeDisplay *decoDisplay);
	virtual void updateDisplay(Bone *bone, DecorativeDisplay *decoDisplay, FrameData *_frameData);
	
	virtual CCNode *getRenderNode(Bone *bone, DecorativeDisplay *decoDisplay);

	virtual float getWidth(Bone *bone, DecorativeDisplay *decoDisplay);
	virtual float getHeight(Bone *bone, DecorativeDisplay *decoDisplay);

	virtual CCRect getBoundingBox(Bone *bone, DecorativeDisplay *decoDisplay);
	virtual CCSize getContentSize(Bone *bone, DecorativeDisplay *decoDisplay);
    
    virtual CCPoint getAnchorPoint(Bone *bone, DecorativeDisplay *decoDisplay);
    virtual CCPoint getAnchorPointInPoints(Bone *bone, DecorativeDisplay *decoDisplay);

	virtual void setVisible(Bone *bone, DecorativeDisplay *decoDisplay, bool visible);
protected:
	virtual void setDisplay(Bone *bone, DecorativeDisplay *decoDisplay, CCNode *_renderNode);
	virtual void createContourPoints(Bone *bone, DecorativeDisplay *decoDisplay);
};
		

}

#endif
