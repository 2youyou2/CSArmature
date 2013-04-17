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
 
#ifndef COCOSTUDIO_CSEDITORARMATURE_H
#define COCOSTUDIO_CSEDITORARMATURE_H

#include "cocos2d.h"
#include "sigslot.h"
#include "CSArmature.h"
#include "CSEditorBone.h"

using namespace cocos2d;


namespace cs {
    
class CS_EXTERN EditorArmature : public Armature, public sigslot::has_slots<>
{

public:
    EditorArmature();
    ~EditorArmature(void);

   
    
    void editName(const char *name);
    
    /**
     * Add a Bone to this Armature for edit, this will modify ArmatureData
     *
     * @param bone  the Bone you want to add to Armature
     * @param parentName   the parent Bone's name you want to add to . If it's  NULL, then set Armature to it's parent
     */
    void addEditBone(Bone *bone, const char* parentName);
    
    
    /**
     * Remove a edit bone with the specified name. It will also remove child Bone recursionly.
     *
     * @param name  the edit bone's name you want to remove
     */
    void removeEditBone(Bone *bone, bool recursion);
    
    
    void changeEditBoneParent(Bone *bone, const char *_parentName);
    
    /**
     * Save the ArmatureData of this Armature to file 
     *
     * @param fileName the file'name save to 
     *
     */
    void saveJsonToFile(const char *fileName);

	virtual void moveIKBoneToTarget(Bone *ikBone, CCPoint &targetPoint);

	virtual void update(float dt);

	virtual void setVisible(bool var);
protected:
	Bone *m_pIKBone;
	CCPoint m_pTargetPoint;

	CC_SYNTHESIZE(int, m_iInterationsPerUpdate, InterationsPerUpdate);
	
protected:
    void connectBoneSignal(EditorBone *bone);
    void disconnectBoneSignal(EditorBone *bone);
    
	virtual void updateIK();
public:
	sigslot::signal1<bool> VisibleChangedSignal;

    void boneNameChangedSlot(Bone *bone, const char *newName);
    
};


}

#endif
