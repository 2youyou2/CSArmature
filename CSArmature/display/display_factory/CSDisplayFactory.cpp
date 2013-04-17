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

#include "CSDisplayFactory.h"
#include "CSDisplayData.h"
#include "CSBone.h"
#include "CSBatchNodeManager.h"
#include "CSArmature.h"


namespace cs {

void DisplayFactory::changeDisplay(Bone *bone, DecorativeDisplay *decoDisplay)
{
	CCNode *_display = getRenderNode(bone, decoDisplay);
	BatchNodeManager::sharedBatchNodeManager()->getCurrentLayer()->addChild(_display);

	setDisplay(bone, decoDisplay, _display);
}

void DisplayFactory::updateDisplay(Bone *bone, DecorativeDisplay *decoDisplay, FrameData *_frameData)
{
	CCNode *renderNode = getRenderNode(bone, decoDisplay);

    do
	{
		CC_BREAK_IF(!renderNode || !bone->isTransformDirty());
        
		CCAffineTransform &t = bone->getWorldTransform();
        
		renderNode->setAdditionalTransform(t);
        
#if ENABLE_PHYSICS_DETECT
		ColliderDetector *detector = decoDisplay->getColliderDetector();
		if (detector)
		{
			detector->updateTransform(t);
		}
#endif
        
	} while (0);
	
}


CCNode *DisplayFactory::getRenderNode(Bone *bone, DecorativeDisplay *decoDisplay)
{
	return (CCNode*)decoDisplay->getDisplay();
}
   
float DisplayFactory::getWidth(Bone *bone, DecorativeDisplay *decoDisplay)
{
	return getContentSize(bone, decoDisplay).width;
}

float DisplayFactory::getHeight(Bone *bone, DecorativeDisplay *decoDisplay)
{
	return getContentSize(bone, decoDisplay).height;
}

CCRect DisplayFactory::getBoundingBox(Bone *bone, DecorativeDisplay *decoDisplay)
{
	return getRenderNode(bone, decoDisplay)->boundingBox();
}

CCSize DisplayFactory::getContentSize(Bone *bone, DecorativeDisplay *decoDisplay)
{
	return getRenderNode(bone, decoDisplay)->getContentSize();
}

CCPoint DisplayFactory::getAnchorPoint(Bone *bone, DecorativeDisplay *decoDisplay)
{
    return getRenderNode(bone, decoDisplay)->getAnchorPoint();
}

CCPoint DisplayFactory::getAnchorPointInPoints(Bone *bone, DecorativeDisplay *decoDisplay)
{
    return getRenderNode(bone, decoDisplay)->getAnchorPointInPoints();
}


void DisplayFactory::setDisplay(Bone *bone, DecorativeDisplay *decoDisplay, CCNode *renderNode)
{
    CS_RETURN_IF(!renderNode);
	Armature *armature = bone->getArmature();
    
//	CCNode *originDisplayRenderNode = bone->getDisplayManager()->getDisplayRenderNode();
    /*
     *  if already have a display we can just use the display's vertexz, or we should update the armature's children order
     */
//    if (originDisplayRenderNode)
//    {
//
//    }else{
//        if(armature)
//        {
//            armature->setBonesIndexChanged(true);
//        }
//    }

    if (armature && armature->getRenderType() == BATCHNODE_VERTEXZ)
    {
        renderNode->setVertexZ(bone->getActualZorder()/10000);
    }
    else
    {
        renderNode->setZOrder(bone->getActualZorder());
    }
    
    bone->getDisplayManager()->setDisplayRenderNode(renderNode);
}


void DisplayFactory::createContourPoints(Bone *bone, DecorativeDisplay *decoDisplay)
{
#if CS_DEBUG_FOR_EDIT

	//! Create ContourPoints
	ContourPoints *contourPoints = new ContourPoints("Armature/BindingBoxBorder.png");
	contourPoints->setBoundingBox(getBoundingBox(bone, decoDisplay));
	BatchNodeManager::sharedBatchNodeManager()->getCurrentLayer()->addChild(contourPoints->getRenderNode());

	contourPoints->getRenderNode()->setZOrder(INT_MAX);
    CCPoint p = getAnchorPoint(bone, decoDisplay);
    contourPoints->setAnchorPoint(p);
    
	decoDisplay->setContourPoints(contourPoints);

#endif
}

void DisplayFactory::setVisible(Bone *bone, DecorativeDisplay *decoDisplay, bool visible)
{
	getRenderNode(bone, decoDisplay)->setVisible(visible);
}

}