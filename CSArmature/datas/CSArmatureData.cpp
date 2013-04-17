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

#include "CSArmatureData.h"
#include "reader.h"
#include "CSConstValue.h"
#include "CSBoneData.h"

namespace cs {

ArmatureData *ArmatureData::createWithJson(const char *pszDescription)
{
    ArmatureData *pArmatureData = new ArmatureData();
    if (pArmatureData && pArmatureData->initWithJson(pszDescription))
    {
        pArmatureData->autorelease();
        return pArmatureData;
    }
    CC_SAFE_DELETE(pArmatureData);
    return NULL;
}
    
ArmatureData *ArmatureData::create()
{
    ArmatureData *pArmatureData = new ArmatureData();
    if (pArmatureData && pArmatureData->init())
    {
        pArmatureData->autorelease();
        return pArmatureData;
    }
    CC_SAFE_DELETE(pArmatureData);
    return NULL;
}

ArmatureData::ArmatureData()
    :m_pBoneDataDic(NULL)
	,m_pBoneList(NULL)
{

}

ArmatureData::~ArmatureData()
{
    CC_SAFE_DELETE(m_pBoneDataDic);
}

bool ArmatureData::init()
{
    return initWithJson(NULL);
}


void ArmatureData::findParentBoneDataAndInsertToJson(BoneData *boneData, int *flag)
{
	BoneData *_parentBoneData = (BoneData*)m_pBoneDataDic->objectForKey(boneData->m_strParent);
	CCArray *keys = m_pBoneDataDic->allKeys();

	if(_parentBoneData)
	{
		findParentBoneDataAndInsertToJson(_parentBoneData, flag);
	}

	int index = m_pBoneList->indexOfObject(boneData);

	if(flag[index] == 0)
	{
		m_JsonDic.insertItemToArray(BONE_DATA, &boneData->getJsonDic());
		flag[index] = 1;
	}

}
    
void ArmatureData::updateJson()
{
	JsonData::updateJson();

	m_JsonDic.insertItem(A_NAME, m_strName.c_str());

	if(m_pBoneDataDic->count() <= 0)
	{
		return;
	}


	int length = m_pBoneDataDic->count();
	int *flag = new int[length];
	for (int i = 0; i<length; i++)
	{
		flag[i] = 0;
	}

	CCDictElement *_element = NULL;
	CCDICT_FOREACH(m_pBoneDataDic, _element)
	{
		BoneData *_boneData = (BoneData*)_element->getObject();
		//m_JsonDic.insertItemToArray(BONE_DATA, &_boneData->getJsonDic());
		findParentBoneDataAndInsertToJson(_boneData, flag);
	}

}

bool ArmatureData::initWithJson(const char *pszDescription)
{
    bool ret = false;
    
    do {
                
        m_pBoneDataDic = new CCDictionary();
        CCAssert(m_pBoneDataDic, "ArmatureData::init m_pBoneDataDic can't be null.");
        
		m_pBoneList = CCArray::create();
		m_pBoneList->retain();
		CCAssert(m_pBoneList, "ArmatureData::init m_pBoneList can't be null.");
        
		JsonData::initWithJson(pszDescription);
        
        
        const char * str = m_JsonDic.getItemStringValue(A_NAME);
        if(str != NULL)
        {
            m_strName = str;
        }
        
        int _length = m_JsonDic.getArrayItemCount(BONE_DATA);
        
        for (int i = 0; i<_length; i++)
        {
            CSJsonDictionary *_dic = m_JsonDic.getSubItemFromArray(BONE_DATA, i);
            
			addBoneData(NULL, _dic->getDescription().c_str());
        }
        
        ret = true;
    } while (0);
    
    
    return ret;
}
    
BoneData *ArmatureData::addBoneData(const char *boneName, const char *pszDescription)
{
    BoneData *boneData = BoneData::createWithJson(pszDescription);
    
    if (boneData->m_strName.compare("") == 0)
    {
        CCAssert(boneName != NULL, "you should provide a valid name for this bone");
        
        boneData->m_strName = boneName;
    }
    
    addBoneData(boneData);
    return boneData;
}
    
BoneData *ArmatureData::addBoneData(BoneData *boneData)
{
    m_pBoneDataDic->setObject(boneData, boneData->m_strName);
	m_pBoneList->addObject(boneData);
    return boneData;
}

BoneData *ArmatureData::removeBoneData(const char *_boneName)
{
    BoneData *boneData = (BoneData*)m_pBoneDataDic->objectForKey(_boneName);
    m_pBoneDataDic->removeObjectForKey(_boneName);
	m_pBoneList->removeObject(boneData);
    return boneData;
}

BoneData *ArmatureData::getBoneData(const char *_boneName)
{
    return (BoneData*)m_pBoneDataDic->objectForKey(_boneName);
}
    
CCDictionary *ArmatureData::getBoneDic()
{
    return m_pBoneDataDic;
}
    
int ArmatureData::getBoneCount()
{
    return m_pBoneDataDic->count();
}
 
}