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

#ifndef COCOSTUDIO_CSEDITORBONE_H
#define COCOSTUDIO_CSEDITORBONE_H

#include "CSBone.h"
#include "CSEditorTween.h"
#include "CSEditorDisplayManager.h"
#include "CSBoneChain.h"

using namespace cocos2d;

namespace cs {


class Armature;

class CS_EXTERN EditorBone : public Bone
{
public:
    /**
	 * Create a Bone
     */
    static EditorBone *create();
    /**
	 * Create a Bone with specified name
     *
	 * @param name Bone's name.
     */
	static EditorBone *create(const char *name);
	
	void setArmature(Armature *armature);

public:
    EditorBone();
	virtual ~EditorBone(void);

	virtual bool init();
	virtual bool init(const char *name);


    void removeEditChildBone(Bone *bone, bool recursion);
	void removeEditFromParent(bool  recursion);

    void editName(const char *name);
	void editZorder(int zorder);
    

	EditorTween *getTween(){ return (EditorTween*)m_pTween; }
	EditorDisplayManager *getDisplayManager(){ return (EditorDisplayManager*)m_pDisplayManager; }


	void convertNodeToParentSpace();
	void convertNodeToArmatureSpace();

	virtual void updateTransform();

	CCAffineTransform m_tSelfTransformMatrix;
protected:
	void convertNodeSpace(bool toParentSpace);
public:
    sigslot::signal2<Bone*, const char*> NameChanedSignal;
    sigslot::signal2<const char*, CCDictionary*> RemoveFromParentSignal;
};


}

#endif
