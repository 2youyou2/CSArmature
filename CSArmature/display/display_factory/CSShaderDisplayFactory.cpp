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
#include "CSShaderDisplayFactory.h"
#include "CSDatas.h"
#include "CSArmature.h"
#include "CSDisplayFactoryManager.h"
#include "CSShaderNode.h"
#include "CSBatchNodeManager.h"

namespace cs {
    
    
CCObject *ShaderDisplayFactory::addDisplay(Bone *bone, DecorativeDisplay *decoDisplay, DisplayData *_DisplayData)
{
    ShaderDisplayData *_adp = ShaderDisplayData::create();
    _adp->copy((ShaderDisplayData*)_DisplayData);
    
    decoDisplay->setDisplayData(_adp);
    
    return createDisplay(bone, decoDisplay);
}
    
CCObject *ShaderDisplayFactory::createDisplay(Bone *bone, DecorativeDisplay *decoDisplay)
{
    CCObject *_display = NULL;
    
    ShaderDisplayData *_displayData = (ShaderDisplayData*)decoDisplay->getDisplayData();
    

	ShaderNode *_sn = ShaderNode::shaderNodeWithVertex(_displayData->vert.c_str(), _displayData->frag.c_str());

	_display = _sn;

    decoDisplay->setDisplay(_display);
    
    return _display;
}
 

}