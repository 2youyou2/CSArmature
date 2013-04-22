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

#ifndef COCOSTUDIO_CSEDITORTWEEN_H
#define COCOSTUDIO_CSEDITORTWEEN_H

#include "CSDatas.h"
#include "CSTweenFunction.h"
#include "sigslot.h"
#include "CSTween.h"
#include "CSArmatureDefine.h"

using namespace cocos2d;

namespace cs {

class EditorBone;
class CS_EXTERN EditorTween : public Tween, public sigslot::has_slots<>
{
public:
    /**
	 *	@brief	create with a Bone
     *
     *  @param  bone the Bone Tween will bind to
     */
	static EditorTween* create(EditorBone *bone);
public:
	EditorTween(void);
	~EditorTween(void);

    
    virtual void gotoFrame(int frameIndex);

	void setMoveBoneData(MovementBoneData *_movBoneData);


	int getFromKeyFrameIndex();
	int getToKeyFrameIndex();

	void setKeyFrameDisplayIndex(int index);
	int getKeyFrameDisplayIndex();

	bool isAtKeyFrame();
    
    /*
     *  @brief  Used for editor, this function will modify the data in the data pool
     *          The param is relative value
     */
    void editRelativePosition(float x, float y);
    void editPosition(float x, float y);
    void editRelativePositionX(float x);
    void editPositionX(float x);
    void editRelativePositionY(float y);
    void editPositionY(float y);
    
   
    void editRelativeRotation(float r);
    void editRotation(float r);
    
    
    void editRelativeScale(float scaleX, float scaleY);
    void editScale(float scaleX, float scaleY);
    void editRelativeScaleX(float scaleX);
    void editScaleX(float scaleX);
    void editRelativeScaleY(float scaleY);
    void editScaleY(float scaleY);

protected:

	/**
	 * Calculate which frame arrived, and if current frame have event, then call the event listener
     */
    virtual float updateFrameData(float currentPrecent, bool activeFrame = false);
    
	/**
	 * Update display index and process the key frame event when arrived a key frame
     */
    virtual void arriveKeyFrame(FrameData *keyFrameData);

protected:
    //! whether or not current state is a key frame state, or it's a tween state
    bool m_bIsAtKeyFrame;

//!slot
private:
	void movementBoneDataDeleted();
};


}

#endif