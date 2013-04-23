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

#ifndef COCOSTUDIO_CSBONE_H
#define COCOSTUDIO_CSBONE_H

#include "CSDatas.h"
#include "CSTween.h"

#include "CSArmatureDefine.h"
#include "CCTexture2DMutable.h"
#include "CSDatas.h"
#include "CSDecorativeDisplay.h"
#include "CSDisplayManager.h"

using namespace cocos2d;

namespace cs {


class Armature;

class CS_EXTERN Bone : public CCObject
{
public:
    /**
     * Allocates and initializes a bone.
     * @return A initialized bone which is marked as "autorelease".
     */
    static Bone *create();
    /**
     * Allocates and initializes a bone.
	 *
	 * @param  name If name is not null, then set name to the bone's name
     * @return A initialized bone which is marked as "autorelease".
     */
	static Bone *create(const char *name);

public:
    Bone();
	virtual ~Bone(void);
    
	/**
     * Initializes an empty Bone with nothing init.
     */
    virtual bool init();

    /**
	 * Initializes a Bone with the specified name
	 * @param name Bone's name.
     */
	virtual bool init(const char *name);


	    
    /**
	 * Add display and use  _DisplayData init the display.
     * If index already have a display, then replace it.
     * If index is current display index, then also change display to _index
     *
	 * @param displayData it include the display information, like DisplayType.
     *          If you want to create a sprite display, then create a SpriteDisplayData param
     *
	 *	@param 	_index the index of the display you want to replace or add to
     *          -1 : append display from back
     */
    void addDisplay(DisplayData *displayData, int index);
    
    void changeDisplayByIndex(int index, bool force);

    /**
	 * Add a child to this bone, and it will let this child call setParent(Bone *_parent) function to set self to it's parent
	 *	@param 	_child  the child you want to add
     */
	void addChildBone(Bone* _child);
    
    /**
	 * Set parent bone. 
     * If _parent is NUll, then also remove this bone from armature.
     * It will not set the Armature, if you want to add the bone to a Armature, you should use Armature::addBone(Bone *bone, const char* _parentName).
     *
	 * @param parent  the parent bone.
     *          NULL : remove this bone from armature
     */
    void setParentBone(Bone *parent);
    
    /**
	 * Get parent bone
	 *	@return parent bone
     */
    Bone *getParentBone();
    
    
    /**
	 * Remove itself from its parent Bone.
	 *	@param 	recursion    whether or not remove Child display
     */
    void removeFromParent(bool recursion);
    
    /**
	 * Removes a child Bone 
	 *	@param 	bone   the bone you want to remove
     */
	void removeChildBone(Bone *bone, bool recursion);

    /**
	 * Set the bone's zorder. the zorder is different to cocos2dx's zorder, it used to calculate the vertexz 
	 *	@param 	_zorder zorder you want to set
     */
    void setZOrder(int _zorder);
    
    /**
     * Get current bone's zorder
     * @return Current bone's zorder
     */
    int getZOrder();
    
    void update(float dt);
    

    /**
     *  Set bone's property, used for users, this won't change data in the data pool
     */
    virtual void setPosition(float x, float y);
    virtual void setPositionX(float x);
    virtual void setPositionY(float y);
    virtual void setRotation(float r);
    virtual void setScale(float scale);
	virtual void setScaleX(float scaleX);
    virtual void setScaleY(float scaleY);

	/**
     * Get bone's user property
     */
	virtual CCPoint getPosition();
	virtual float	getPositionX();
	virtual float	getPositionY();
	virtual float	getRotation();
	virtual float	getScaleX();
	virtual float	getScaleY();

	virtual void setOpacity(GLubyte value);
	virtual void setColor(const ccColor4B &color);
	virtual void setColor(const ccColor3B &color);


	CCArray *getChildren();
    FrameData *getCombinedData();
	Tween *getTween();

	/*
	 * Whether or not the bone's transform property changed. if true, the bone will update the transform.
	 */
	inline virtual void	setTransformDirty(bool dirty){ m_bTransformDirty = dirty; } 
	inline virtual bool isTransformDirty(){ return m_bTransformDirty; }

	/*
	 * Whether or not the bone's color property changed. if true, the bone will update the color.
	 */
	inline virtual void	setColorDirty(bool dirty){ m_bColorDirty = dirty; } 
	inline virtual bool isColorDirty(){ return m_bColorDirty; }

	/*
	 * Whether or not the bone is a root bone, this bone is created when a armature created. It's used for armature to update all bones
	 */
	inline virtual void setRootBone(bool isRootBone){ m_bRootBone = isRootBone; }
	inline virtual bool isRootBone(){ return m_bRootBone; }

	/*
	 * Internal method to recorder the bone's actual zorder
	 */
	virtual void _setActualZorder(float zorder);
	inline virtual float getActualZorder(){ return m_fActualZorder; }

	inline CCAffineTransform &getWorldTransform(){ return m_tWorldTransform; }
	inline CCAffineTransform &getWorldTransformForChildren(){ return m_tWorldTransformForChildren; }
protected:
    
    virtual void updateTransform(float dt);
	virtual void updateColor();
public:
    /*
     *  The origin state of the Bone. Display's state is effected by m_pBoneData, m_pNode, m_pTweenData
     *  when call setData function, it will copy from the BoneData.
     */
	CC_PROPERTY(BoneData *, m_pBoneData, BoneData);
    
    //! A weak reference to the Armature
	CC_SYNTHESIZE(Armature *, m_pArmature, Armature);
    
    //! A weak reference to the child Armature
    CC_PROPERTY(Armature *, m_pChildArmature, ChildArmature);
    
    CC_SYNTHESIZE(DisplayManager*, m_pDisplayManager, DisplayManager)

	/*
	 *	When Armature play a animation, if there is not a MovementBoneData of this bone in this MovementData, this bone will hide.
	 *	Set IgnoreMovementBoneData to true, then this bone will also show.
	 */
	CC_SYNTHESIZE_PASS_BY_REF(bool, m_bIgnoreMovementBoneData, IgnoreMovementBoneData)

protected:
	Tween *m_pTween;				//! Calculate tween effect
    
	FrameData *m_pTweenData;		//! Used for make tween effect between every frame, it's a weak reference of Tween's tweenData
    FrameData *m_pUserData;			//! Used for users to change the m_pDisplay's transform
	FrameData *m_pCombinedData;		//! Combine TweenData and UserData, then it's the final data

    /*
     *  The zorder in the Armature, when chane zorder, you need to change Armature's m_bBonesIndexChanged to 
     *  true. It is used for Armature calculate Bone's actual zorder.
     */
    int m_iZOrder;

	/*
	 *	The actual zorder will set to the display. Calculate by armature
	 */
	int m_fActualZorder;

	CC_SYNTHESIZE_PASS_BY_REF(std::string, m_strName, Name);
    
	//! Lazy allocs
	void childrenAlloc(void);
    CCArray *m_pChildren;
    
    
    Bone *m_pParent;				//! A weak reference to it's parent

	bool m_bRootBone;				//! Whether or not this bone is a root bone
	bool m_bTransformDirty;			//! Whether or not transform dirty
	bool m_bColorDirty;				//! Whether or not color property is changed.

	//! self Transform, use this to change display's state
	CCAffineTransform m_tWorldTransform;
    //! Transform for children
	CCAffineTransform m_tWorldTransformForChildren;
};

}

#endif