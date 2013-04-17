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
#include "CSArmatureDisplayFactory.h"
#include "CSDisplayData.h"
#include "CSArmature.h"
#include "CSDisplayFactoryManager.h"
#include "CSArmatureDisplayData.h"

namespace cs {
    
    
CCObject *ArmatureDisplayFactory::addDisplay(Bone *bone, DecorativeDisplay *decoDisplay, DisplayData *displayData)
{
    ArmatureDisplayData *_adp = ArmatureDisplayData::create(); ;
    _adp->copy((ArmatureDisplayData*)displayData);
    
    decoDisplay->setDisplayData(_adp);
    
    return createDisplay(bone, decoDisplay);
}
    
CCObject *ArmatureDisplayFactory::createDisplay(Bone *bone, DecorativeDisplay *decoDisplay)
{
    CCObject *_display;
    
    ArmatureDisplayData *_displayData = (ArmatureDisplayData*)decoDisplay->getDisplayData();
    
    Armature *armature = Armature::create(_displayData->getDisplayName().c_str());
    
    /*
     *  here use the whole Armature to be a display
     */
    _display = armature;
    
    /*
     *  because this bone have called this name, so armature should change it's name, or it can't add to
     *  Armature's bone children.
     */
    armature->setName((bone->getName() + "_armatureChild").c_str());
    
    decoDisplay->setDisplay(_display);
    
    return _display;
}
    
void ArmatureDisplayFactory::changeDisplay(Bone *bone, DecorativeDisplay *decoDisplay)
{
    
    Armature *armature = (Armature*)decoDisplay->getDisplay();
    
    armature->getAnimation()->playByIndex(0);
    
	CCNode * _renderNode = getRenderNode(bone, decoDisplay);
    
    bone->setChildArmature(armature);
    
    /*
     *  add child armature to armature's bone list
     */
    if(bone->getArmature())
    {
        bone->getArmature()->addBone(armature->getRootBone(), bone->getName().c_str());
    }
    
    setDisplay(bone, decoDisplay, _renderNode);
}
    
CCNode *ArmatureDisplayFactory::getRenderNode(Bone *bone, DecorativeDisplay *decoDisplay)
{
	Armature *armature = (Armature*)decoDisplay->getDisplay();
	return armature->getRootBone()->getDisplayManager()->getDisplayRenderNode();
}
    
void ArmatureDisplayFactory::updateDisplay(Bone *bone, DecorativeDisplay *decoDisplay, FrameData *_frameData)
{
    do
	{
        Armature *childArmature = bone->getChildArmature();
		CC_BREAK_IF(!bone->isTransformDirty() || !childArmature);
        
		CCAffineTransform &t = bone->getWorldTransform();
        
		childArmature->setAdditionalTransform(t);
        
#if ENABLE_PHYSICS_DETECT
		ColliderDetector *detector = decoDisplay->getColliderDetector();
		if (detector)
		{
			detector->updateTransform(t);
		}
#endif
        
	} while (0);
}

void ArmatureDisplayFactory::setVisible(Bone *bone, DecorativeDisplay *decoDisplay, bool visible)
{
	if(bone->getChildArmature())
	{
		bone->getChildArmature()->setVisible(visible);
	}
}

}