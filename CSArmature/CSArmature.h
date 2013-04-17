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
#include "CSBoneData.h"
#include "CSDisplayData.h"
#include "CSArmatureData.h"
#include "CSBone.h"
#include "cocos2d.h"
#include "CSArmatureDefine.h"

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
    
class Armature : public CCNodeRGBA
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
     * Sets whether the node is visible
     * The default value is true, a node is default to visible
     *
     * @param visible   true if the node is visible, false if the node is hidden.
     */
    virtual void setVisible(bool visible);
    /**
     * Determines if the node is visible
     *
     * @see setVisible(bool)
     *
     * @return true if the node is visible, false if the node is hidden.
     */
    virtual bool isVisible();

    
    /**
     * Reorder the Armature. This will redistribution all child bones vertexz or zorder.
     * You should note that this zorder is different from zorder of cocos2dx.
     * It is used to calculate child bones vertexz or zorder.
     *          
     * We do not use cocos2dx zorder, because we use batchnode to improve speed and batchnode zorder do not support two batchnode can't mixed use their sprite's zorder
     *
     * @param zOrder The zorder you want to set
     */
    virtual void setZOrder(int zOrder);


    /**
     * Get Armature's bone dictionary
     * @return Armature's bone dictionary
     */
	CCDictionary *getBoneDic();

	virtual CCRect getBoundingBox();
    
    
    Bone *getBoneAtPoint(float _x, float _y);
    
    /**
     * Animation event call back, When Animation is start, complete or other, Animation will call this method to dispatch Animation event.
     * @param  eventType 
     */
    void onMovementEvent(const char *eventType, const char *movementID);
    
    void update(float dt);
	virtual CCAffineTransform nodeToParentTransform();
    

    inline virtual void setTransformDirty(bool dirty){ m_bDirty = dirty; }
	inline virtual bool isTransformDirty(){ return m_bDirty; }


	inline virtual GLubyte getOpacity() { return m_nOpacity; }
	inline virtual const ccColor3B& getColor(){ return m_sColor; }

	virtual void setOpacity(GLubyte value);
	virtual void setColor(const ccColor3B& value);
	/**
	* Changes the premultipliedAlphaOpacity property.
	*
	* Textures with premultiplied alpha will have this property by default on true.
	* Otherwise the default value is false.
	*
	* @param   bValue  flase then opacity will be applied as: glColor(R,G,B,opacity);
	*                  true then opacity will be applied as: glColor(opacity, opacity, opacity, opacity);
	*/
	virtual void setOpacityModifyRGB(bool bValue);
	virtual bool isOpacityModifyRGB(void);
private:
    /*
     * Sort Bones in this Armature, if m_pDisPlayBatchNode used BATCHNODE_VERTEXZ, then order use vertexz, else use cocos2dx zorder 
     */
    void sortBoneHelper(int _baseVertexz, int &_index);
    
    /*
     * Used to create Bone internal
     */
	Bone *createBone(const char *_boneName );

	/*
	 *	When a new armature is created, internal sort is called. This sort will calculate the current max armature internal zorder, 
	 *	and give the zorder to the new created armature. This zorder is not be changed after.
	 *	It's different from the zorder used for users.
	 */
	void internalSort();

	void initRootBone();
    
public:
	CC_SYNTHESIZE_RETAIN(Animation *, m_pAnimation, Animation);

	CC_SYNTHESIZE_PASS_BY_REF(bool, m_bBonesIndexChanged, BonesIndexChanged);
    
    CC_SYNTHESIZE(ArmatureData *, m_pArmatureData, ArmatureData);
    
    CC_PROPERTY(RENDER_TYPE, m_eRenderType, RenderType)

	CC_SYNTHESIZE(Bone *, m_pRootBone, RootBone)

	CC_SYNTHESIZE(Armature*, m_pArmature, Armature);

protected:
    CCDictionary *m_pBoneDic;		//! The dictionary of the bones, include all bones in the armature, no matter it is the direct bone or the indirect bone. It is different from m_pChindren.
    
    CCArray *m_pBoneList;				//! A CCArray include all bones

    static std::map<int, Armature*> m_sArmatureIndexDic;	//! Use to save armature zorder info, 
    
	GLubyte m_nOpacity;				//! Goes from 0-255. 0 means fully tranparent and 255 means fully opaque.
	ccColor3B m_sColor;					//! Color: conforms with CCRGBAProtocol protocol

	bool m_bDirty;
	
	float m_fInternalZOrder;			//! This is used to sign the index of the Armature being created
	float m_fActualZOrder;				//! InternalZOrder combine ZOrder is the ActualZOrder.

	bool m_bOpacityModifyRGB;

	CC_SYNTHESIZE_PASS_BY_REF(std::string, m_strName, Name);
private:
	static int m_siMaxArmatureZorder;
    static CCDictionary *m_sArmatureMap;
    
    ComArmature *m_pComArmature;
};


}

#endif