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

#include "CSDatas.h"

namespace cs {

#pragma region BoneData

BoneData::BoneData(void)
    :name("")
    ,parentName("")
{
}

BoneData::~BoneData(void)
{
}

bool BoneData::init()
{
	displayDataList.init();
	return true;
}

DisplayData *BoneData::addDisplayData(DisplayData *displayData)
{
    displayDataList.addObject(displayData);
    return displayData;
}
#pragma endregion


#pragma region ArmatureData
ArmatureData::ArmatureData()
{
}

ArmatureData::~ArmatureData()
{
}

bool ArmatureData::init()
{
	boneList.init();
	return true;
}

BoneData *ArmatureData::addBoneData(BoneData *boneData)
{
    boneDataDic.setObject(boneData, boneData->name);
	boneList.addObject(boneData);
    return boneData;
}

BoneData *ArmatureData::removeBoneData(const char *boneName)
{
    BoneData *boneData = (BoneData*)boneDataDic.objectForKey(boneName);
    boneDataDic.removeObjectForKey(boneName);
	boneList.removeObject(boneData);
    return boneData;
}
#pragma endregion

#pragma region DisplayData
const char *DisplayData::changeDisplayToTexture(const char * displayName)
{
    // remove .xxx
    std::string textureName = displayName;
    size_t startPos = textureName.find_last_of(".");
    
    if(startPos != std::string::npos)
    {
        textureName = textureName.erase(startPos);
    }
    
    return textureName.c_str();
}

DisplayData::DisplayData(void)
    :displayType(CS_DISPLAY_SPRITE)
{
}


DisplayData::~DisplayData(void)
{
}

#pragma endregion

}