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

#ifndef COCOSTUDIO_CSARMATUREDATA_H
#define COCOSTUDIO_CSARMATUREDATA_H

#include "cocos2d.h"
#include "CSBoneData.h"

 using namespace cocos2d;

#define CS_CREATE_NO_PARAM(varType)\
public: \
	inline static varType *create(void){ \
	varType *var = new varType();\
	if (var && var->init())\
	{\
	var->autorelease();\
	return var;\
	}\
	CC_SAFE_DELETE(var);\
	return NULL;\
}

namespace cs {

#pragma region BoneData
/**
 * BoneData used to init a Bone.
 * BoneData keeps a DisplayData list, a Bone can have many display to change.
 * The display information saved in the DisplayData
 */
class CS_EXTERN BoneData : public Node
{
public:
	CS_CREATE_NO_PARAM(BoneData);
public:
	BoneData(void);
	~BoneData(void);

	virtual bool init();
    
    DisplayData *addDisplayData(DisplayData *_displayData);
    
public:
	std::string name;					//! the bone's name
	std::string parentName;		//! the bone parent's name
	CCArray displayDataList;		//! save DisplayData informations for the Bone
};
#pragma  endregion


#pragma  region ArmatureData
/**
 * ArmatureData saved the Armature name and Bonedatas needed for the Bones in this Armature
 * When we create a Armature, we need to get each Bone's BoneData as it's init information.
 * So we can get a BoneData from the CCDictionary saved in the ArmatureData.
 */
class CS_EXTERN ArmatureData : public CCObject
{
public:
	CS_CREATE_NO_PARAM(ArmatureData);
public:
	ArmatureData();
	~ArmatureData();
	
	bool init();

    BoneData *addBoneData(BoneData *boneData);
    BoneData *removeBoneData(const char *boneName);

	std::string name;
	CCDictionary boneDataDic;
	CCArray boneList;
};
#pragma  endregion

#pragma region DisplayData
class CS_EXTERN DisplayData : public CCObject
{
public:
	CS_CREATE_NO_PARAM(DisplayData);
    
    static const char *changeDisplayToTexture(const char *);
public:
    DisplayData();
    virtual ~DisplayData(void);
    
	virtual bool init(){};
    
    DisplayType displayType;	//! mark which type your display is
};
#pragma endregion

#pragma region SpriteDisplayData

class CS_EXTERN SpriteDisplayData : public DisplayData
{
public:
	CS_CREATE_NO_PARAM(SpriteDisplayData);
public:
    SpriteDisplayData();
    virtual ~SpriteDisplayData();
    
    virtual bool init();
    
	inline void setParam(const char *displayName){ displayName = displayName; }
    
    void copy(SpriteDisplayData *displayData);
    
    /**
     * If DisplayType is CS_DISPLAY_SPRITE, then Bone will use this image name to create a CCSprite from CCSpriteFrameCache.
     * It should note that when use this name to create CCSprite from CCSpriteFrameCache, you should use m_strDisplayName + ".png", because when use Texture Packer to pack single image file, the name have ".png".
     *
     * If DisplayType is CS_DISPLAY_ARMATURE, the name is the Armature's name. When Bone init display and type is CS_DISPLAY_ARMATURE,
     * then Bone will create a Armature.
     */
    std::string displayName;

};

#pragma endregion

#pragma region ArmatureDisplayData

#pragma endregion

}

#endif