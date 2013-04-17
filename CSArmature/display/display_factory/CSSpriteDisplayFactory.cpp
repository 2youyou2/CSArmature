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

#include "CSArmatureDefine.h"
#include "CSTextureData.h"
#include "CSSpriteDisplayFactory.h"
#include "CSDisplayData.h"
#include "CSArmatureDataManager.h"
#include "CSDisplayFactoryManager.h"
#include "CSBatchNodeManager.h"
#include "CSSpriteDisplayData.h"
#include "CSSpriteFrameCacheHelper.h"
#include "CSTransformHelp.h"

#if ENABLE_PHYSICS_DETECT
#include "CSColliderDetector.h"
#endif

namespace cs {
  
CCObject *SpriteDisplayFactory::addDisplay(Bone *bone, DecorativeDisplay *decoDisplay, DisplayData *_DisplayData)
{
    
    SpriteDisplayData *_sdp = SpriteDisplayData::create();
    _sdp->copy((SpriteDisplayData*)_DisplayData);
    
    decoDisplay->setDisplayData(_sdp);
    
    return createDisplay(bone, decoDisplay);
}

CCObject *SpriteDisplayFactory::createDisplay(Bone *bone, DecorativeDisplay *decoDisplay)
{
    CCSprite *_display = NULL;
    
    do {
        
        SpriteDisplayData *_displayData = (SpriteDisplayData*)decoDisplay->getDisplayData();
        
        //! remove .xxx
        std::string _textureName = _displayData->getDisplayName();
        size_t startPos = _textureName.find_last_of(".");
        
        if(startPos != std::string::npos)
        {
            _textureName = _textureName.erase(startPos);
        }

		//! create display
		if(_textureName.compare("") == 0)
		{
			_display = CCSprite::create();
		}
		else
		{
			_display = CCSprite::createWithSpriteFrameName((_textureName + ".png").c_str());
		}

		decoDisplay->setDisplay(_display);
		
   
        TextureData *_textureData = ArmatureDataManager::sharedArmatureDataManager()->getTextureData(_textureName.c_str());
        
       if(_textureData)
        {
            /*
             *  init display anchorPoint， every Texture have a anchor point
             */
            ((CCSprite*)_display)->setAnchorPoint(ccp( _textureData->getPivotX(), _textureData->getPivotY()));
			_textureData->AnchorPointChangedSignal.connect(decoDisplay, &DecorativeDisplay::anchorPointChanged);
        }
        
#if ENABLE_PHYSICS_DETECT
		if (_textureData && _textureData->getContourCount() > 0)
		{

			//! create ContourSprite
			ColliderDetector *colliderDetector = ColliderDetector::create(bone);
			colliderDetector->addContourDataList(_textureData->getContourDatas());

			decoDisplay->setColliderDetector(colliderDetector);
		}
#endif

    } while (0);
    
    return _display;
}
    
void SpriteDisplayFactory::changeDisplay(Bone *bone, DecorativeDisplay *decoDisplay)
{
    CCSprite *renderNode = (CCSprite*)getRenderNode(bone, decoDisplay);
    
    
	SpriteDisplayData *_displayData = (SpriteDisplayData*)decoDisplay->getDisplayData();

	// remove .xxx
	std::string _textureName = _displayData->getDisplayName();
	size_t startPos = _textureName.find_last_of(".");

	if(startPos != std::string::npos)
	{
		_textureName = _textureName.erase(startPos);
	}

	_textureName += ".png";

	if(_textureName.compare(".png") == 0)
	{
		//BatchNodeManager::sharedBatchNodeManager()->getCurrentLayer()->addChild(renderNode);
	}
	else
	{
		std::string imagePath = SpriteFrameCacheHelper::sharedSpriteFrameCacheHelper()->getDisplayImagePath(_textureName.c_str());

		CCSpriteBatchNode *batchNode = BatchNodeManager::sharedBatchNodeManager()->getBatchNode(imagePath);
		batchNode->addChild(renderNode);
	}

    bone->setChildArmature(NULL);
    
	setDisplay(bone, decoDisplay, renderNode);
}

CCNode *SpriteDisplayFactory::getRenderNode(Bone *bone, DecorativeDisplay *decoDisplay)
{
	return (CCNode*)decoDisplay->getDisplay();
}

void SpriteDisplayFactory::updateDisplay(Bone *bone, DecorativeDisplay *decoDisplay, FrameData *_frameData)
{
	CCSprite *renderNode = (CCSprite*)decoDisplay->getDisplay();

	do 
	{
		CC_BREAK_IF(!bone->isTransformDirty());

		
        CCAffineTransform t = bone->getWorldTransform();
		CCAffineTransform t2;
		TransformHelp::nodeToMatrix(*bone->getBoneData(), t2);
		t = CCAffineTransformConcat(t2, t);
        
		renderNode->setAdditionalTransform(t);
		renderNode->setDirty(true);
        

#if ENABLE_PHYSICS_DETECT
		ColliderDetector *detector = decoDisplay->getColliderDetector();
		if (detector)
		{
			detector->updateTransform(t);
		}
#endif

	} while (0);
	
    do 
    {
		CC_BREAK_IF(!bone->isColorDirty());
		const ccColor4B &c = bone->getCombinedData()->m_cColor;
		renderNode->setColor(ccc3(c.r, c.g, c.b));
		renderNode->setOpacity(c.a);
    } while (0);
}
  
CCTexture2DMutable *SpriteDisplayFactory::createTexture2DMutable(float _width, float _height, const char *_imageFile)
{
    CCTexture2DMutable *_textureMutable = new CCTexture2DMutable();
    _textureMutable->initWithImageFile(_imageFile);
    
    return _textureMutable;
}


CCRect SpriteDisplayFactory::getBoundingBox(Bone *bone, DecorativeDisplay *decoDisplay)
{
	CCRect boundingBox;
	CCNode *renderNode = getRenderNode(bone, decoDisplay);

	boundingBox = renderNode->boundingBox();

	return boundingBox;
}

CCSize SpriteDisplayFactory::getContentSize(Bone *bone, DecorativeDisplay *decoDisplay)
{
	return getRenderNode(bone, decoDisplay)->getContentSize();
}
    
CCPoint SpriteDisplayFactory::getAnchorPoint(Bone *bone, DecorativeDisplay *decoDisplay)
{
	return getRenderNode(bone, decoDisplay)->getAnchorPoint(); 
}

CCPoint SpriteDisplayFactory::getAnchorPointInPoints(Bone *bone, DecorativeDisplay *decoDisplay)
{
	return getRenderNode(bone, decoDisplay)->getAnchorPointInPoints();   
}

}