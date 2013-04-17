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

#ifndef COCOSTUDIO_CSBONECHAIN_H
#define COCOSTUDIO_CSBONECHAIN_H


#include "cocos2d.h"
#include "CSBone.h"
#include "sigslot.h"

using namespace cocos2d;

namespace cs {

class CS_EXTERN BoneChain : public CCSprite, public sigslot::has_slots<>
{
public:
    static BoneChain *create();
    static BoneChain *create(const char *fileName);
public:
    BoneChain();
    ~BoneChain();

	bool initWithFile(const char *dirPath);

	void setParentChainPoint(CCPoint &parentPoint);
	void setChildChainPoint(CCPoint &childPoint);

	void draw();
	void update(float dt);

	static void setCurrentBoneChain(BoneChain *boneChain);
	static BoneChain *getCurrentBoneChain();
protected:
	CCPoint m_pParantPoint;
	CCPoint m_pChildPoint;

	CCPoint m_pOldParentAnchorPoint;
	CCPoint m_pOldChildAnchorPoint;

	CC_PROPERTY(Bone*, m_pParentBone, ParentBone);
	CC_PROPERTY(Bone*, m_pChildBone, ChildBone);

	bool m_bConnectBone;
	bool m_bSetChildPoint;
public:
	void connectBoneSignal(Bone *bone);

	void boneRemovedFromParent(const char *name, CCDictionary *dict);
	void boneAnchorPointChanged(Bone *bone, CCPoint oldAnchorPoint, bool restoreAnchorPoint);
	void boneVisibleChanged(bool visible);

private:
	static BoneChain *s_CurrentBoneChain;

	CCSprite *m_pCircleSprite;
	CCSprite *m_pArrowSprite;

};
		
}

#endif
