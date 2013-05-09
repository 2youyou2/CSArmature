
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


#include "tinyxml.h"
#include "CSArmatureDataManager.h"
#include "CSTransformHelp.h"
#include "CSDataReaderHelper.h"
#include "cocos2d.h"
//#include "CSGame.h"
#include "CSSpriteFrameCacheHelper.h"

using namespace cocos2d;

namespace cs {


ArmatureDataManager *ArmatureDataManager::m_sSharedArmatureDataManager = NULL;

ArmatureDataManager *ArmatureDataManager::sharedArmatureDataManager()
{
    if (m_sSharedArmatureDataManager == NULL)
    {
        m_sSharedArmatureDataManager = new ArmatureDataManager();
        if (!m_sSharedArmatureDataManager || !m_sSharedArmatureDataManager->init())
        {
            CC_SAFE_DELETE(m_sSharedArmatureDataManager);
        }
    }
    return m_sSharedArmatureDataManager;
}

ArmatureDataManager::ArmatureDataManager(void)
{
    m_pAnimationDatas = NULL;
    m_pArmarureDatas = NULL;
    m_pTextureDatas = NULL;
    m_pArmatureFileInfoDic = NULL;
}


ArmatureDataManager::~ArmatureDataManager(void)
{
    
    CC_SAFE_DELETE(m_pAnimationDatas);
    CC_SAFE_DELETE(m_pArmarureDatas);
    CC_SAFE_DELETE(m_pTextureDatas);
    CC_SAFE_DELETE(m_pArmatureFileInfoDic);
}

bool ArmatureDataManager::init()
{
    bool bRet = false;
    do
    {

        m_pArmarureDatas = CCDictionary::create();
        CCAssert(m_pArmarureDatas, "create ArmatureDataManager::m_pArmarureDatas fail!");
        m_pArmarureDatas->retain();
        
        m_pAnimationDatas = CCDictionary::create();
        CCAssert(m_pAnimationDatas, "create ArmatureDataManager::m_pAnimationDatas fail!");
        m_pAnimationDatas->retain();

        m_pTextureDatas = CCDictionary::create();
        CCAssert(m_pTextureDatas, "create ArmatureDataManager::m_pTextureDatas fail!");
        m_pTextureDatas->retain();
        
        m_pArmatureFileInfoDic = CCDictionary::create();
        CCAssert(m_pArmatureFileInfoDic, "create ArmatureDataManager::m_pArmatureFileInfoDic fail!");
        m_pArmatureFileInfoDic->retain();
        
        bRet = true;
    }
    while (0);

    return bRet;
}

void ArmatureDataManager::addArmatureData(const char *id, ArmatureData *armatureData)
{
    if(m_pArmarureDatas)
    {
        m_pArmarureDatas->setObject(armatureData, id);
    }
}
    
ArmatureData *ArmatureDataManager::getArmatureData(const char* id)
{
    ArmatureData * armatureData = NULL;
    if (m_pArmarureDatas)
    {
        armatureData = (ArmatureData*)m_pArmarureDatas->objectForKey(id);
    }
    return armatureData;
}

void ArmatureDataManager::addAnimationData(const char *id, AnimationData *animationData)
{
    if(m_pAnimationDatas)
    {
        m_pAnimationDatas->setObject(animationData, id);
    }
}
    
void ArmatureDataManager::addTextureData(const char *id, TextureData *textureData)
{
    if(m_pTextureDatas)
    {
        m_pTextureDatas->setObject(textureData, id);
    }
}
    
AnimationData* ArmatureDataManager::getAnimationData(const char* id)
{
    AnimationData *animationData = NULL;
    if (m_pAnimationDatas)
    {
        animationData = (AnimationData*)m_pAnimationDatas->objectForKey(id);
    }
    return animationData;
}

TextureData *ArmatureDataManager::getTextureData(const char* id)
{
    TextureData *textureData = NULL;
    if (m_pTextureDatas)
    {
        textureData = (TextureData*)m_pTextureDatas->objectForKey(id);
    }
    return textureData;
}
    
ArmatureFileInfo *ArmatureDataManager::getArmatureFileInfo(const char *id)
{
    ArmatureFileInfo *armatureFileInfo = NULL;
    if (m_pArmatureFileInfoDic)
    {
        armatureFileInfo = (ArmatureFileInfo*)m_pArmatureFileInfoDic->objectForKey(id);
    }
    return armatureFileInfo;
}



void ArmatureDataManager::addArmatureFileInfo(const char *armatureName, const char *useExistFileInfo, const char *imagePath, const char *plistPath, const char *configFilePath)
{
    ArmatureFileInfo *fileInfo = (ArmatureFileInfo*)m_pArmatureFileInfoDic->objectForKey(armatureName);
    
    if(!fileInfo)
    {
        fileInfo = ArmatureFileInfo::create();
        fileInfo->armatureName = armatureName;
        fileInfo->configFilePath = configFilePath;
        fileInfo->useExistFileInfo = useExistFileInfo;
        

        if (fileInfo->useExistFileInfo.compare("") != 0)
        {
            fileInfo = (ArmatureFileInfo*)m_pArmatureFileInfoDic->objectForKey(fileInfo->useExistFileInfo);
        }
        
        
        m_pArmatureFileInfoDic->setObject(fileInfo, fileInfo->armatureName);
    }
    
    DataReaderHelper::addDataFromFile(configFilePath);
    
    for (std::vector<ImageInfo>::iterator it = fileInfo->imageInfoVector.begin(); it != fileInfo->imageInfoVector.end(); it++)
    {
        if (it->imagePath.compare(imagePath) == 0)
        {
            return;
        }
    }
    
    ImageInfo imageInfo;
    
    imageInfo.imagePath = imagePath;
    imageInfo.plistPath = plistPath;
    
    addSpriteFrameFromFile(plistPath, imagePath);
    
    fileInfo->imageInfoVector.push_back(imageInfo);
    
    
}
    
void ArmatureDataManager::addSpriteFrameFromFile(const char *plistPath, const char *imagePath)
{
//	if(Game::sharedGame()->isUsePackage())
//	{
//		SpriteFrameCacheHelper::sharedSpriteFrameCacheHelper()->addSpriteFrameFromPak(plistPath, imagePath);
//	}
//    else
//	{
//		SpriteFrameCacheHelper::sharedSpriteFrameCacheHelper()->addSpriteFrameFromFile(plistPath, imagePath);
//	}
    SpriteFrameCacheHelper::sharedSpriteFrameCacheHelper()->addSpriteFrameFromFile(plistPath, imagePath);
}


void ArmatureDataManager::removeAll()
{
    if( m_pAnimationDatas )
    {
        m_pAnimationDatas->removeAllObjects();
    }
    if( m_pArmarureDatas )
    {
        m_pArmarureDatas->removeAllObjects();
    }

    if( m_pTextureDatas )
    {
        m_pTextureDatas->removeAllObjects();
    }
    if( m_pArmatureFileInfoDic )
    {
        m_pArmatureFileInfoDic->removeAllObjects();
    }

	DataReaderHelper::clear();
}


}