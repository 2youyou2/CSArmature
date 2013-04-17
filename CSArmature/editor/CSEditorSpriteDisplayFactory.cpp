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
#include "CSEditorSpriteDisplayFactory.h"
#include "CSDisplayData.h"
#include "CSArmatureDataManager.h"
#include "CSDisplayFactoryManager.h"
#include "CSBatchNodeManager.h"
#include "CSSpriteDisplayData.h"
#include "CSEditorTween.h"

namespace cs {


CCObject *EditorSpriteDisplayFactory::createDisplay(Bone *bone, DecorativeDisplay *decoDisplay)
{
    CCSprite *display = NULL;
    
    do {
        
        SpriteDisplayData *displayData = (SpriteDisplayData*)decoDisplay->getDisplayData();
        
        // remove .xxx
        std::string textureName = displayData->getDisplayName();
        size_t startPos = textureName.find_last_of(".");
        
        if(startPos != std::string::npos)
        {
            textureName = textureName.erase(startPos);
        }


		//! create display
		display = CCSprite::create();
		CCSprite *sprite = NULL;

		if(textureName.compare("") != 0)
		{
			sprite = CCSprite::create((textureName + ".png").c_str());
		}
		else
		{
			decoDisplay->setDisplay(display);
			break;
		}

		display->addChild(sprite, 0, 0);
		decoDisplay->setDisplay(display);
		

		//! get TextureData 
        TextureData *textureData = ArmatureDataManager::sharedArmatureDataManager()->getTextureData(textureName.c_str());
        
        if(textureData)
        {
            /*
             *  init display anchorPoint， every Texture have a anchor point
             */
			((CCSprite*)display)->setAnchorPoint(ccp( textureData->getPivotX(), textureData->getPivotY()));
        }
        
		//! create CCTexture2DMutable
		CCSize size = ((CCSprite*)sprite)->getContentSize();

		CCTexture2DMutable *textureMutable = createTexture2DMutable(size.width, size.height, (textureName + ".png").c_str());
		textureMutable->autorelease();

		decoDisplay->setTextureMutable(textureMutable);

        
		//! create ContourPoints
        createContourPoints(bone, decoDisplay);

    } while (0);
    
    return display;
}
    
void EditorSpriteDisplayFactory::changeDisplay(Bone *bone, DecorativeDisplay *decoDisplay)
{
    CCSprite *renderNode = (CCSprite*)getRenderNode(bone, decoDisplay);
    
	BatchNodeManager::sharedBatchNodeManager()->getCurrentLayer()->addChild(renderNode);

    bone->setChildArmature(NULL);
    

	setDisplay(bone, decoDisplay, renderNode);
}

CCNode *EditorSpriteDisplayFactory::getRenderNode(Bone *bone, DecorativeDisplay *decoDisplay)
{
	return (CCNode*)decoDisplay->getDisplay();
}

void EditorSpriteDisplayFactory::updateDisplay(Bone *bone, DecorativeDisplay *decoDisplay, FrameData *frameData)
{
	CS_RETURN_IF(!decoDisplay);
	CCSprite *renderNode = (CCSprite*)decoDisplay->getDisplay();

	do 
	{
		CC_BREAK_IF(!bone->isTransformDirty());

		CCAffineTransform t = bone->m_tWorldTransform;
    
		t.a /= frameData->m_fScaleX;
		t.b /= frameData->m_fScaleX;
		t.c /= frameData->m_fScaleY;
		t.d /= frameData->m_fScaleY;

		renderNode->setAdditionalTransform(t);
		renderNode->setDirty(true);

		
		renderNode = (CCSprite*)renderNode->getChildByTag(0);
		renderNode->setScaleX(frameData->m_fScaleX);
		renderNode->setScaleY(frameData->m_fScaleY);

		ContourPoints *contourPoints = decoDisplay->getContourPoints();
		contourPoints->getRenderNode()->setAdditionalTransform(t);
		contourPoints->setBoundingBox(renderNode->boundingBox());

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

CCRect EditorSpriteDisplayFactory::getBoundingBox(Bone *bone, DecorativeDisplay *decoDisplay)
{
	CCRect boundingBox;
	CCNode *renderNode = getRenderNode(bone, decoDisplay);

	boundingBox = renderNode->getChildByTag(0)->boundingBox();
	boundingBox = CCRectApplyAffineTransform(boundingBox, renderNode->nodeToParentTransform());

	return boundingBox;
}

CCSize EditorSpriteDisplayFactory::getContentSize(Bone *bone, DecorativeDisplay *decoDisplay)
{
	return getRenderNode(bone, decoDisplay)->getChildByTag(0)->getContentSize();
}
    
CCPoint EditorSpriteDisplayFactory::getAnchorPoint(Bone *bone, DecorativeDisplay *decoDisplay)
{
	return getRenderNode(bone, decoDisplay)->getChildByTag(0)->getAnchorPoint();
}

CCPoint EditorSpriteDisplayFactory::getAnchorPointInPoints(Bone *bone, DecorativeDisplay *decoDisplay)
{
	return getRenderNode(bone, decoDisplay)->getChildByTag(0)->getAnchorPointInPoints();
}

}
