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

#include "CSJsonData.h"
#include "CSBoneData.h"

using namespace cocos2d;

namespace cs {

/**
 * ArmatureData saved the Armature name and Bonedatas needed for the Bones in this Armature
 * When we create a Armature, we need to get each Bone's BoneData as it's init information.
 * So we can get a BoneData from the CCDictionary saved in the ArmatureData.
 */
class CS_EXTERN ArmatureData : public CCObject, public JsonData
{
public:
    static ArmatureData *createWithJson(const char *pszDescription);
	static ArmatureData *create();
public:
	ArmatureData();
	~ArmatureData();
	

	bool init();

    virtual bool initWithJson(const char *pszDescription);
    
    /*
     * Update data's json value
     */
    virtual void updateJson();
    
    BoneData *addBoneData(const char *boneName, const char *pszDescription = NULL);
    BoneData *addBoneData(BoneData *boneData);
    BoneData *removeBoneData(const char *boneName);
    
    BoneData *getBoneData(const char *boneName);
    CCDictionary *getBoneDic();
    int getBoneCount();
    
private:
    void findParentBoneDataAndInsertToJson(BoneData *boneData, int *flag);
    
    /**
	 * Armature name. Used for Armature to find it's ArmatureData when create a Armature,
     */
    CC_SYNTHESIZE_PASS_BY_REF(std::string, m_strName, Name)
    
private:
    /**
	 *	@brief	save movment datas
	 *  @key	std::string
	 *  @value	BoneData*
     */
	CCDictionary *m_pBoneDataDic;

	CCArray *m_pBoneList;
};

}

#endif