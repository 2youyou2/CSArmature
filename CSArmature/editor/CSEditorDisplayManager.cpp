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

#include "CSEditorDisplayManager.h"
#include "CSDisplayFactoryManager.h"
#include "CSBone.h"
#include "CSUtilMath.h"
#include "CSArmature.h"
#include "CSEditorTween.h"
#include "CSTransformHelp.h"
#include "CSEditorBone.h"

#if CS_TOOL_PLATFORM
#include "CSOBB.h"
#endif

namespace cs {

EditorDisplayManager *EditorDisplayManager::create(Bone *bone)
{
    EditorDisplayManager *pDisplayManager = new EditorDisplayManager();
    if (pDisplayManager && pDisplayManager->init(bone))
    {
        pDisplayManager->autorelease();
        return pDisplayManager;
    }
    CC_SAFE_DELETE(pDisplayManager);
    return NULL;
}
    
    
EditorDisplayManager::EditorDisplayManager()
{
}
    
EditorDisplayManager::~EditorDisplayManager()
{
}

bool EditorDisplayManager::containPoint(CCPoint &point)
{
    if(!m_bVisible || m_iDisplayIndex < 0)
    {
        return false;
    }
    
    bool ret = false;
    
    switch (m_pCurrentDecoDisplay->getDisplayData()->getDisplayType())
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
            
            ContourPoints *contourPoints = m_pCurrentDecoDisplay->getContourPoints();
            if( contourPoints && contourPoints->isVisible())
            {
                //! check whether or not click on a anchor point
                ContourPointType type = contourPoints->containWorldPoint(point);
                
                //! If click point is on a kind of contour point, return true
                if(type != POINT_MIN)
                {
                    ret = true;
                    return ret;
                }
            }
            
            //! If ret is false, then click point is neither on the sprite rect and contour point, return false.
            if(!ret)
            {
                return ret;
            }
            
            
            if(m_pCurrentDecoDisplay->getTextureMutable() && m_pCurrentDecoDisplay->getTextureMutable()->pixelAt(outPoint).a == 0)
            {
                ret = false;
            }
            
            return ret;
        }
            break;
            
        default:
            break;
    }
    return ret;
}

bool EditorDisplayManager::containPoint(float x, float y)
{
    CCPoint p = ccp(x, y);
    return containPoint(p);
}

bool EditorDisplayManager::rectTest(CCRect &r)
{
#if CS_TOOL_PLATFORM
	if(m_pDisplayRenderNode)
	{
		CCRect bounding = getBoundingBox();
        
        CCAffineTransform t = m_pBone->getWorldTransform();
		ObbVector v = ObbVector(t.tx, t.ty);

		Node node;
		TransformHelp::matrixToNode(t, node);

		float rotation = node.m_fSkewX;

		OBB o1(v, bounding.size.width, bounding.size.height, -CC_DEGREES_TO_RADIANS(rotation));

		v = ObbVector((r.getMinX() + r.getMaxX())/2, (r.getMinY() + r.getMaxY())/2);
		OBB o2(v, r.size.width, r.size.height, 0);

		return o1.detectObbVsObb(o2);
	}
#endif
	return false;
}

int EditorDisplayManager::getClickedContourPointType()
{
    ContourPoints *contourPoints = m_pCurrentDecoDisplay->getContourPoints();
    if(contourPoints && contourPoints->isVisible())
    {
        return contourPoints->getClickedPointType();
    }
    
    return POINT_MIN;
}

void EditorDisplayManager::setVisible(bool visible)
{
	if(!m_pCurrentDecoDisplay)
	{
		return;
	}

    m_bVisible = visible;

	VisibleChangedSignal.emit(visible);
    
    if(!m_bVisible)
    {
        if(m_pCurrentDecoDisplay->getContourPoints())
        {
            m_pCurrentDecoDisplay->getContourPoints()->setVisible(m_bVisible);
        }
    }
    
    switch (m_pCurrentDecoDisplay->getDisplayData()->getDisplayType())
    {
        case CS_DISPLAY_SPRITE:
        {
			if(m_pDisplayRenderNode)
			{
				m_pDisplayRenderNode->setVisible(visible);
			}
            
        }
            break;
        case CS_DISPLAY_ARMATURE:
        {
            if(m_pBone->getChildArmature())
            {
                m_pBone->getChildArmature()->setVisible(visible);
            }
        }
            break;
        default:
            break;
    }
    
}


void EditorDisplayManager::showContour(bool show)
{
    if (m_pCurrentDecoDisplay && m_pCurrentDecoDisplay->getContourPoints())
    {
        m_pCurrentDecoDisplay->getContourPoints()->setVisible(show);
    }
}

bool EditorDisplayManager::isShowContour()
{
    if (m_pCurrentDecoDisplay && m_pCurrentDecoDisplay->getContourPoints())
    {
        return m_pCurrentDecoDisplay->getContourPoints()->isVisible();
    }
    return false;
}
    
void EditorDisplayManager::convertPointToSpace(CCPoint &p)
{   
    if(m_pDisplayRenderNode)
    {
        p = m_pDisplayRenderNode->convertToNodeSpace(p);
    }
    else
    {
        CCAffineTransform t = m_pBone->getWorldTransform();
        
        t.a /= m_pBone->getScaleX();
        t.b /= m_pBone->getScaleX();
        t.c /= m_pBone->getScaleY();
        t.d /= m_pBone->getScaleY();
        
        CCNode *node = CCNode::create();
        node->setAdditionalTransform(t);
        
        p = node->convertToNodeSpace(p);
    }
}
    

void EditorDisplayManager::editAnchorPoint(CCPoint &p, bool moveCenterAnchorPoint, bool restoreAnchorPoint)
{

	EditorTween *tween = (EditorTween*)m_pBone->getTween();
	if(!tween->isAtKeyFrame())
	{
		return;
	}

	CCSprite *renderNode = (CCSprite*)DisplayFactoryManager::getFactory((DisplayType)(m_pCurrentDecoDisplay->getDisplayData())->getDisplayType())->getRenderNode(m_pBone, m_pCurrentDecoDisplay)->getChildByTag(0);

	CCPoint oldAnchorPoint = renderNode->getAnchorPoint();

	CCRect boundingBox = renderNode->boundingBox();

	float deltaX, deltaY = 0;
	float deltaX2, deltaY2 = 0;
	float tempX, tempY = 0;


	CCRect rect = renderNode->getTextureRect();
	tempX = rect.size.width * (p.x - oldAnchorPoint.x);
	tempY = rect.size.height * (p.y - oldAnchorPoint.y);


	CCAffineTransform t = ((EditorBone*)m_pBone)->m_tSelfTransformMatrix;

	deltaX = (float)((double)t.a * tempX + (double)t.c * tempY);
	deltaY = (float)((double)t.b * tempX + (double)t.d * tempY);

	tween->editRelativePosition(deltaX, deltaY);

	AnchorPointChangeTemporarySignal.emit(m_pBone, oldAnchorPoint, restoreAnchorPoint);

	deltaX2 = boundingBox.size.width * (p.x - oldAnchorPoint.x);
	deltaY2 = boundingBox.size.height * (p.y - oldAnchorPoint.y);

	CCArray *_children = m_pBone->getChildren();
	CCObject *_object = NULL;
	CCARRAY_FOREACH(_children, _object)
	{
		Bone *_bone = (Bone*)_object;

		((EditorTween*)_bone->getTween())->editRelativePosition(-deltaX2, -deltaY2);
	}

	renderNode->setAnchorPoint(ccp(p.x, p.y));

	if(moveCenterAnchorPoint)
	{
		m_pCurrentDecoDisplay->getContourPoints()->setAnchorPoint(p);
	}
}

void EditorDisplayManager::addEditDisplay(DisplayData *displayData, int index)
{
    addDisplay(displayData, index);


	DisplayData *addedDisplayData = NULL;

	if(index >= 0 && index < m_pDecoDisplayList->count())
	{
		addedDisplayData = ((DecorativeDisplay*)m_pDecoDisplayList->objectAtIndex(index))->getDisplayData();
	}
	else
	{
		addedDisplayData = ((DecorativeDisplay*)m_pDecoDisplayList->lastObject())->getDisplayData();
	}

	if(index > 0 && m_pBone->getBoneData()->getDisplayCount()>index)
	{
		if(m_pBone->getBoneData()->getDisplayData(index))
		{
			m_pBone->getBoneData()->changeDisplayDataAtIndex(index, addedDisplayData);
		}
		else
		{
			m_pBone->getBoneData()->insertDisplayData(addedDisplayData, index);
		}
	}
	else
	{
		m_pBone->getBoneData()->addDisplayData(addedDisplayData);
	}
}



void EditorDisplayManager::insertEditDisplay(DisplayData *displayData, int index)
{
    insertDisplay(displayData, index);

    m_pBone->getBoneData()->insertDisplayData(displayData, index);
}


void EditorDisplayManager::removeEditDisplay(int index)
{
    removeDisplay(index);
    
    m_pBone->getBoneData()->removeDisplayData(index);
}
    

}
