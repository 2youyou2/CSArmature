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
 
#ifndef COCOSTUDIO_CSARMATURE_H
#define COCOSTUDIO_CSARMATURE_H

#include "CSBatchNode.h"
#include "CSAnimation.h"
#include "CSDatas.h"
#include "CSBone.h"
#include "cocos2d.h"
#include "CSArmatureDefine.h"
#include "CSBatchNode.h"

using namespace cocos2d;


namespace cs {

class ComArmature;
    
typedef struct _ImageInfo
{
    std::string imagePath;
    std::string plistPath;
}ImageInfo;

typedef struct _ArmatureFileInfo : public CCObject
{
    static _ArmatureFileInfo *create()
    {
        _ArmatureFileInfo *_afi = new _ArmatureFileInfo();
        if(_afi)
        {
            _afi->autorelease();
            return _afi;  
        }
        CC_SAFE_DELETE(_afi);
        return NULL;
    }

    std::string armatureName;
    std::string useExistFileInfo;
    std::string configFilePath;
    std::vector<ImageInfo> imageInfoVector;
}ArmatureFileInfo;
    
class Armature : public CCNodeRGBA, public CCBlendProtocol 
{

public:
   /**
	* Allocates and initializes a armature.
	* @return A initialized armature which is marked as "autorelease".
	*/
	static Armature *create();
    
   /**
	* Allocates a armature, and use the ArmatureData named name in ArmatureDataManager to initializes the armature. 
	*
	* @param  name Armature will use the name to find to the ArmatureData to initializes it.
	* @return A initialized armature which is marked as "autorelease".
	*/
	static Armature *create(const char* name);
    
#if CS_TOOL_PLATFORM
    /**
	 * Create a empty armature with Component Armature
	 * @param comArmature Component Armature bind to
	 */
	static Armature *createWithComArmature(ComArmature *comArmature);
	     
    /**
	 * Create a armature with Component Armature
	 *
	 * @param name Armature name
	 * @param comArmature Component Armature bind to
	 */
	static Armature *createWithComArmature(const char* name, ComArmature *comArmature);

    /**
	 * Init the empty armature with Component Armature
	 * @param comArmature Component Armature bind to
	 */
	virtual bool initWithComArmature(ComArmature *comArmature);
    
    /**
	 * Init a armature with Component Armature
	 *
	 * @param name Armature name
	 * @param comArmature Component Armature bind to
	 */
	virtual bool initWithComArmature(const char *_name, ComArmature *_comArmature);
#endif


public:
    Armature();
    ~Armature(void);

    /**
     * Init the empty armature
     */
    virtual bool init();
    
    /**
     * Init a armature with specified name
     * @param name Armature name
     */
    virtual bool init(const char *name);

    /**
     * Add a Bone to this Armature, 
     *
     * @param bone  The Bone you want to add to Armature
     * @param parentName   The parent Bone's name you want to add to . If it's  NULL, then set Armature to it's parent
     */
    virtual void addBone(Bone *bone, const char* parentName);
    /**
     * Get a bone with the specified name
     *
     * @param name The bone's name you want to get
     */
    virtual Bone *getBone(const char *_name);
    /**
     * Change a bone's parent with the specified parent name.
     *
     * @param bone The bone you want to change parent
	 * @param parentName The new parent's name.
     */
    virtual void changeBoneParent(Bone *bone, const char *parentName);
    /**
     * Remove a bone with the specified name. If recursion it will also remove child Bone recursionly.
     *
     * @param bone The bone you want to remove
	 * @param recursion Determine whether remove the bone's child  recursion.
     */
    virtual void removeBone(Bone *bone, bool recursion);

    /**
     * Get Armature's bone dictionary
     * @return Armature's bone dictionary
     */
	CCDictionary *getBoneDic();

	virtual CCRect boundingBox();
    
    Bone *getBoneAtPoint(float _x, float _y);
    
    /**
     * Animation event call back, When Animation is start, complete or other, Animation will call this method to dispatch Animation event.
     * @param  eventType 
     */
    void onMovementEvent(const char *eventType, const char *movementID);
    
	virtual void visit();
    virtual void update(float dt);
	virtual void draw();

	inline void setBlendFunc(ccBlendFunc blendFunc) { m_sBlendFunc = blendFunc; }
	inline ccBlendFunc getBlendFunc(void) { return m_sBlendFunc; }

protected:
    
    /*
     * Used to create Bone internal
     */
	Bone *createBone(const char *_boneName );
    
	
	CC_SYNTHESIZE_RETAIN(Animation *, m_pAnimation, Animation);

	CC_SYNTHESIZE_PASS_BY_REF(bool, m_bBonesIndexChanged, BonesIndexChanged);
    
    CC_SYNTHESIZE(ArmatureData *, m_pArmatureData, ArmatureData);

	CC_SYNTHESIZE(Armature*, m_pArmature, Armature);

	CC_SYNTHESIZE(BatchNode*, m_pBatchNode, BatchNode);
protected:
    CCDictionary *m_pBoneDic;		//! The dictionary of the bones, include all bones in the armature, no matter it is the direct bone or the indirect bone. It is different from m_pChindren.

    static std::map<int, Armature*> m_sArmatureIndexDic;	//! Use to save armature zorder info, 

	ccBlendFunc        m_sBlendFunc;            /// It's required for CCTextureProtocol inheritance

	CC_SYNTHESIZE_PASS_BY_REF(std::string, m_strName, Name);

	CC_SYNTHESIZE(CCTextureAtlas*, m_pAtlas, TextureAtlas);
    
    ComArmature *m_pComArmature;
};


}

#endif