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

#include "CSDisplayManager.h"
#include "CSDisplayFactoryManager.h"
#include "CSBone.h"
#include "CSUtilMath.h"
#include "CSArmature.h"

namespace cs {

DisplayManager *DisplayManager::create(Bone *bone)
{
    DisplayManager *pDisplayManager = new DisplayManager();
    if (pDisplayManager && pDisplayManager->init(bone))
    {
        pDisplayManager->autorelease();
        return pDisplayManager;
    }
    CC_SAFE_DELETE(pDisplayManager);
    return NULL;
}
    
    
DisplayManager::DisplayManager()
    :m_pDecoDisplayList(NULL)
    ,m_pCurrentDecoDisplay(NULL)
    ,m_pDisplayRenderNode(NULL)
    ,m_iDisplayIndex(-1)
    ,m_bForceChangeDisplay(false)
    ,m_pBone(NULL)
    ,m_bVisible(true)
{
}
    
DisplayManager::~DisplayManager()
{
    CC_SAFE_DELETE(m_pDecoDisplayList);
    
    if( m_pDisplayRenderNode )
    {
        m_pDisplayRenderNode->removeFromParentAndCleanup(true);
        if(m_pDisplayRenderNode->retainCount() >0)
            CC_SAFE_RELEASE_NULL(m_pDisplayRenderNode);
    }

}
    
bool DisplayManager::init(Bone *bone)
{
    bool ret = false;
    
    do {
        
        m_pBone = bone;
        
        initDisplayList(bone->getBoneData());
        
        ret = true;
    } while (0);
    
    return ret;
}

    
void DisplayManager::addDisplay(DisplayData *_displayData, int _index)
{
    DecorativeDisplay *decoDisplay = NULL;
    
    if(_index >= 0 && _index < m_pDecoDisplayList->count())
    {
        decoDisplay = (DecorativeDisplay*)m_pDecoDisplayList->objectAtIndex(_index);
    }
    else
    {
        decoDisplay = DecorativeDisplay::create();
        m_pDecoDisplayList->addObject(decoDisplay);
    }
    
    
    DisplayFactoryManager::getFactory((DisplayType)_displayData->displayType)->addDisplay(m_pBone, decoDisplay, _displayData);
    
    //! if changed display index is current display index, then change current display to the new display
    if(_index == m_iDisplayIndex)
    {
        m_iDisplayIndex = -1;
        changeDisplayByIndex(_index, false);
    }
}



void DisplayManager::insertDisplay(DisplayData *displayData, int index)
{
    DecorativeDisplay *decoDisplay = NULL;
    
    decoDisplay = DecorativeDisplay::create();
    m_pDecoDisplayList->insertObject(decoDisplay, index);
    
    
	DisplayFactoryManager::getFactory((DisplayType)displayData->displayType)->addDisplay(m_pBone, decoDisplay, displayData);
    
    //! if changed display index is current display index, then change current display to the new display
    if(index == m_iDisplayIndex)
    {
        m_iDisplayIndex = -1;
        changeDisplayByIndex(index, false);
    }
}


void DisplayManager::removeDisplay(int index)
{
    m_pDecoDisplayList->removeObjectAtIndex(index);
    
    if(index == m_iDisplayIndex)
    {
        setDisplayRenderNode(NULL);
    }
    
}
    
void DisplayManager::changeDisplayByIndex(int index, bool force)
{
    CCAssert( (m_pDecoDisplayList ? index<(int)m_pDecoDisplayList->count() : true), "the _index value is out of range");
    
    
    m_bForceChangeDisplay = force;
    
    /*
     *	if _index is equal to current display index,then do nothing
     */
    if ( m_iDisplayIndex == index)
    {
        return;
    }
    
    
    m_iDisplayIndex = index;
    
    /*
     *	m_iDisplayIndex == -1, it means you want to hide you display
     */
    if (m_iDisplayIndex < 0)
    {
        if(m_pDisplayRenderNode)
        {
            m_pDisplayRenderNode->removeFromParentAndCleanup(true);
            setDisplayRenderNode(NULL);
            
        }
        return;
    }
    
    
    DecorativeDisplay *decoDisplay = (DecorativeDisplay*)m_pDecoDisplayList->objectAtIndex(m_iDisplayIndex);
    
    m_pCurrentDecoDisplay = decoDisplay;
    
    DisplayFactoryManager::getFactory((DisplayType)decoDisplay->getDisplayData()->displayType)->changeDisplay(m_pBone, decoDisplay);
    
}
    
void DisplayManager::setDisplayRenderNode(CCNode *displayRenderNode)
{
    if (m_pDisplayRenderNode)
    {
        m_pDisplayRenderNode->removeFromParentAndCleanup(true);
        m_pDisplayRenderNode->release();
    }
    
    m_pDisplayRenderNode = displayRenderNode;
    if(m_pDisplayRenderNode)
    {
        m_pDisplayRenderNode->retain();
    }
}
    
CCNode *DisplayManager::getDisplayRenderNode()
{
    return m_pDisplayRenderNode;
}
    
int DisplayManager::getCurrentDisplayIndex()
{
    return m_iDisplayIndex;
}

DecorativeDisplay *DisplayManager::getCurrentDecorativeDisplay()
{
    return m_pCurrentDecoDisplay;
}

DecorativeDisplay *DisplayManager::getDecorativeDisplayByIndex( int index)
{
	return (DecorativeDisplay*)m_pDecoDisplayList->objectAtIndex(index);
} 

void DisplayManager::initDisplayList(BoneData *boneData)
{
    CC_SAFE_DELETE(m_pDecoDisplayList);
    m_pDecoDisplayList = CCArray::create();
    m_pDecoDisplayList->retain();
    
    if(!boneData)
    {
        return;
    }
    
    CCObject *object = NULL;
    CCArray *displayDataList = &boneData->displayDataList;
    CCARRAY_FOREACH(displayDataList, object)
    {
        DisplayData *displayData = (DisplayData*)object;
        
        DecorativeDisplay *decoDisplay = DecorativeDisplay::create();
        decoDisplay->setDisplayData(displayData);
        
        CCObject *display = NULL;
        display = DisplayFactoryManager::getFactory((DisplayType)displayData->displayType)->createDisplay(m_pBone, decoDisplay);
        
        m_pDecoDisplayList->addObject(decoDisplay);
    }
}
    
void DisplayManager::updateDisplay()
{
	CS_RETURN_IF(!m_pCurrentDecoDisplay);
    DisplayData *displayData = m_pCurrentDecoDisplay->getDisplayData();
    DisplayFactoryManager::getFactory((DisplayType)displayData->displayType)->updateDisplay(m_pBone, m_pCurrentDecoDisplay, m_pBone->getCombinedData());
}
    
    
bool DisplayManager::containPoint(CCPoint &point)
{
	if(!m_bVisible || m_iDisplayIndex < 0)
	{
		return false;
	}
    
    bool ret = false;
    
    switch (m_pCurrentDecoDisplay->getDisplayData()->displayType)
    {
        case CS_DISPLAY_SPRITE:
        {
            /*
             *  First we first check if the point is in the sprite content rect. If false, then we continue to check
             *  the contour point. If this step is also false, then we can say the bone not contain this point.
             *
             */
            
            CCPoint outPoint = ccp(0, 0);
            
            CCSprite *sprite = (CCSprite*)m_pCurrentDecoDisplay->getDisplay();
            sprite = (CCSprite*)sprite->getChildByTag(0);
            
            ret = CS_SPRITE_CONTAIN_POINT_WITH_RETURN(sprite, point, outPoint);
            
        }
            break;
            
        default:
            break;
    }
    return ret;
}

bool DisplayManager::containPoint(float x, float y)
{
    CCPoint p = ccp(x, y);
    return containPoint(p);
}


void DisplayManager::setVisible(bool visible)
{
	if(!m_pCurrentDecoDisplay)
	{
		return;
	}

	m_bVisible = visible;

	DisplayFactoryManager::getFactory((DisplayType)m_pCurrentDecoDisplay->getDisplayData()->displayType)->setVisible(m_pBone, m_pCurrentDecoDisplay, m_bVisible);
}

bool DisplayManager::isVisible()
{
    return m_bVisible;
}


CCSize DisplayManager::getContentSize()
{
	CS_RETURN_IF(!m_pCurrentDecoDisplay) CCSizeMake(0,0);
    DisplayData *_displayData = m_pCurrentDecoDisplay->getDisplayData();
    return DisplayFactoryManager::getFactory((DisplayType)_displayData->displayType)->getContentSize(m_pBone, m_pCurrentDecoDisplay);
}

float DisplayManager::getWidth()
{
	CS_RETURN_NULL_IF(!m_pCurrentDecoDisplay);
    DisplayData *_displayData = m_pCurrentDecoDisplay->getDisplayData();
    return DisplayFactoryManager::getFactory((DisplayType)_displayData->displayType)->getWidth(m_pBone, m_pCurrentDecoDisplay);
}

float DisplayManager::getHeight()
{
	CS_RETURN_IF(!m_pCurrentDecoDisplay) 0;
    DisplayData *_displayData = m_pCurrentDecoDisplay->getDisplayData();
    return DisplayFactoryManager::getFactory((DisplayType)_displayData->displayType)->getHeight(m_pBone, m_pCurrentDecoDisplay);
}

CCRect DisplayManager::getBoundingBox()
{
	CS_RETURN_IF(!m_pCurrentDecoDisplay) CCRectMake(0,0,0,0);
    DisplayData *_displayData = m_pCurrentDecoDisplay->getDisplayData();
    return DisplayFactoryManager::getFactory((DisplayType)_displayData->displayType)->getBoundingBox(m_pBone, m_pCurrentDecoDisplay);
}


CCPoint DisplayManager::getAnchorPoint()
{
	CS_RETURN_IF(!m_pCurrentDecoDisplay) ccp(0,0);
    DisplayData *_displayData = m_pCurrentDecoDisplay->getDisplayData();
    return DisplayFactoryManager::getFactory((DisplayType)_displayData->displayType)->getAnchorPoint(m_pBone, m_pCurrentDecoDisplay);;
}

CCPoint DisplayManager::getAnchorPointInPoints()
{
	CS_RETURN_IF(!m_pCurrentDecoDisplay) ccp(0,0);
    DisplayData *_displayData = m_pCurrentDecoDisplay->getDisplayData();
    return DisplayFactoryManager::getFactory((DisplayType)_displayData->displayType)->getAnchorPointInPoints(m_pBone, m_pCurrentDecoDisplay);
}


}