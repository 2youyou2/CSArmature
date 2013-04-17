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
#include "CSParticleDisplayFactory.h"
#include "CSDisplayData.h"
#include "CSArmature.h"
#include "CSDisplayFactoryManager.h"
#include "CSParticleDisplayData.h"
#include "CSShaderNode.h"
#include "CSBatchNodeManager.h"

namespace cs {
    
    
CCObject *ParticleDisplayFactory::addDisplay(Bone *bone, DecorativeDisplay *decoDisplay, DisplayData *_DisplayData)
{
    ParticleDisplayData *_adp = ParticleDisplayData::create(); ;
    _adp->copy((ParticleDisplayData*)_DisplayData);
    
    decoDisplay->setDisplayData(_adp);
    
    return createDisplay(bone, decoDisplay);
}
    
CCObject *ParticleDisplayFactory::createDisplay(Bone *bone, DecorativeDisplay *decoDisplay)
{
    CCObject *_display = NULL;
    
    ParticleDisplayData *_displayData = (ParticleDisplayData*)decoDisplay->getDisplayData();
    
	CCParticleSystem *_system = CCParticleSystemQuad::create(_displayData->getPlist().c_str());

	_display = _system;

    decoDisplay->setDisplay(_display);
    
    return _display;
}

void ParticleDisplayFactory::updateDisplay(Bone *bone, DecorativeDisplay *decoDisplay, FrameData *_frameData)
{
	CCNode *_renderNode = getRenderNode(bone, decoDisplay);

	if(_renderNode)
	{
		_renderNode->setPosition(bone->m_tWorldTransform.tx, bone->m_tWorldTransform.ty);
		_renderNode->setScaleX(_frameData->m_fScaleX);
		_renderNode->setScaleY(_frameData->m_fScaleY);
	}

}

    
}