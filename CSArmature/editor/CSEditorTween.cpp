#include "CSEditorTween.h"
#include "CSEditorBone.h"
#include "CSArmature.h"
#include "CSUtilMath.h"
#include "CSEditorDisplayManager.h"

namespace cs {


EditorTween* EditorTween::create(EditorBone *bone)
{
    EditorTween *pTween = new EditorTween();
    if (pTween && pTween->init(bone))
    {
        pTween->autorelease();
        return pTween;
    }
    CC_SAFE_DELETE(pTween);
    return NULL;

}



EditorTween::EditorTween()
	:m_bIsAtKeyFrame(false)
{
}


EditorTween::~EditorTween(void)
{
}

void EditorTween::setMoveBoneData(MovementBoneData *_movBoneData)
{
	if(m_pMovementBoneData)
	{
		m_pMovementBoneData->DeletedSignal.disconnect(this);
	}
	_movBoneData->DeletedSignal.connect(this, &EditorTween::movementBoneDataDeleted);

	m_pMovementBoneData = _movBoneData;
}


void EditorTween::gotoFrame(int frameIndex)
{
    
    if(!m_pMovementBoneData)
    {
        return;
    }
    m_iRawDuration = m_pMovementBoneData->getDuration();
    
    if(frameIndex >= m_iRawDuration)
    {
		EditorDisplayManager *displayManager = (EditorDisplayManager*)m_pBone->getDisplayManager();
        displayManager->changeDisplayByIndex(-1, false);
		displayManager->showContour(false);
        stop();
        return;
    }
    
    ProcessBase::gotoFrame(frameIndex);
    
    
    float _p = (float)m_iCurFrameIndex / (float)m_iRawDuration;
    
    m_pEditKeyFrame = NULL;
    
    m_iTotalDuration = 0;
    m_iBetweenDuration = 0;
    m_iToIndex = 0;
    

    _p = updateFrameData(_p);
    

	m_pEditKeyFrame = m_pMovementBoneData->getFrameData(m_iFromIndex);


    if(!m_bIsAtKeyFrame)
	{
		m_pEditKeyFrame = NULL;
    }

	tweenNodeTo(_p);
}


void EditorTween::arriveKeyFrame(FrameData *keyFrameData)
{
	if(keyFrameData)
	{
		int displayIndex = keyFrameData->getDisplayIndex();

		m_pTweenData->setDisplayIndex(displayIndex);

		EditorDisplayManager *displayManager = (EditorDisplayManager*)m_pBone->getDisplayManager();
		if (!m_pBone->getDisplayManager()->getForceChangeDisplay())
		{

			bool changeDisplay = displayIndex == displayManager->getCurrentDisplayIndex()? false : true;
			bool showContour = displayManager->isShowContour();


			if(displayIndex >= 0 || changeDisplay)
				displayManager->showContour(false);

			m_pBone->getDisplayManager()->changeDisplayByIndex(displayIndex, false);

			if(displayIndex >= 0 && showContour && changeDisplay)
				displayManager->showContour(true);
		}


		//m_pBone->setZOrder(m_pCurrentKeyFrame->m_iZOrder);

		Armature *_childAramture = m_pBone->getChildArmature();

		if(_childAramture)
		{
			if(keyFrameData->m_strMovement.compare("") != 0)
			{
				_childAramture->getAnimation()->play(keyFrameData->m_strMovement.c_str());
			}
		}

		if(keyFrameData->m_strEvent.length() != 0)
		{
			//m_pBone->dispatchEventWith(Event.BONE_EVENT_FRAME, m_pCurrentKeyFrame->event);
		}
		if(keyFrameData->m_strSound.length() != 0)
		{
			//soundManager.dispatchEventWith(Event.SOUND_FRAME, m_pCurrentKeyFrame->sound);
		}
	}

}


float EditorTween::updateFrameData(float _currentPrecent, bool _activeFrame)
{

    float _played = (float)m_iRawDuration * _currentPrecent;
    
    
    FrameData *_from;
    FrameData *_to;
    bool _isListEnd;

    // if play to current frame's front or back, then find current frame again
    if (_played >= m_iTotalDuration || _played < m_iTotalDuration - m_iBetweenDuration)
    {
        m_bIsAtKeyFrame = false;
        
        /*
         *  get frame length, if m_iToIndex >= _length, then set m_iToIndex to 0, start anew.
         *  m_iToIndex is next index will play
         */
        int _length = m_pMovementBoneData->getFrameCount();
        do
        {
            if(abs(_played - m_iTotalDuration) < 0.001f)
            {
                m_bIsAtKeyFrame = true;
            }
            m_iBetweenDuration = m_pMovementBoneData->getFrameData(m_iToIndex)->getDuration();
            m_iTotalDuration += m_iBetweenDuration;
            m_iFromIndex = m_iToIndex;
			
            if (++m_iToIndex >= _length)
            {
                m_iToIndex = 0;
            }

			if (_played == (float)m_iRawDuration)
			{
				break;
			}
        }
        while (_played >= m_iTotalDuration || abs(_played - m_iTotalDuration) < 0.001f);

        _isListEnd = m_eLoopType == ANIMATION_MAX && m_iToIndex == 0;
        
        if(_isListEnd)
        {
            _to = _from = m_pMovementBoneData->getFrameData(m_iFromIndex);
        }
        else
        {
            _from = m_pMovementBoneData->getFrameData(m_iFromIndex);
            _to = m_pMovementBoneData->getFrameData(m_iToIndex);
        }
        
        m_eFrameTweenEasing = _from->getTweenEasing();
        
        setBetween(_from, _to);
        
    }

    _currentPrecent = m_iBetweenDuration==0 ? 0 : 1 - (m_iTotalDuration - _played) / (float)m_iBetweenDuration;

    
	/*
     *  If frame tween easing equal to TWEEN_EASING_MAX, then it will not do tween.
     */
    
    TweenType _tweenType;
    
 	if ( m_eFrameTweenEasing != TWEEN_EASING_MAX)
    {
        _tweenType = (m_eTweenEasing == TWEEN_EASING_MAX) ? m_eFrameTweenEasing : m_eTweenEasing;
        if (_tweenType != TWEEN_EASING_MAX)
        {
            _currentPrecent = TweenFunction::tweenTo(0, 1, _currentPrecent, 1, _tweenType);
        }
    }

    return _currentPrecent;
}

    

void EditorTween::editRelativePosition(float x, float y)
{
    editRelativePositionX(x); 
    editRelativePositionY(y);
}


void EditorTween::editPosition(float x, float y)
{
    editPositionX(x);
    editPositionY(y);
}

void EditorTween::editRelativePositionX(float x)
{
    if(!m_pEditKeyFrame)
    {
        return;
    }
    
    m_pEditKeyFrame->m_fX += x;
    
    m_pTweenData->m_fX = m_pEditKeyFrame->m_fX;
}
    
void EditorTween::editPositionX(float x)
{
    if(!m_pEditKeyFrame)
    {
        return;
    }
    
    m_pEditKeyFrame->m_fX = x;
    
    m_pTweenData->m_fX = m_pEditKeyFrame->m_fX;
}

void EditorTween::editRelativePositionY(float y)
{
    if(!m_pEditKeyFrame)
    {
        return;
    }
    
    m_pEditKeyFrame->m_fY += y;
    
    m_pTweenData->m_fY = m_pEditKeyFrame->m_fY;
}
        
void EditorTween::editPositionY(float y)
{
    if(!m_pEditKeyFrame)
    {
        return;
    }
    
    m_pEditKeyFrame->m_fY = y;
    
    m_pTweenData->m_fY = m_pEditKeyFrame->m_fY;
}

void EditorTween::editRelativeRotation(float r)
{
    
    CCLOG("rotation : %f", CC_RADIANS_TO_DEGREES(r));
    if(!m_pEditKeyFrame)
    {
        return;
    }
    
    m_pEditKeyFrame->m_fSkewX += r;
    m_pEditKeyFrame->m_fSkewY -= r;
    
    m_pEditKeyFrame->m_fSkewX = fmodf(m_pEditKeyFrame->m_fSkewX, 2 * CS_PI);
    m_pEditKeyFrame->m_fSkewY = fmodf(m_pEditKeyFrame->m_fSkewY, 2 * CS_PI);
    
    m_pTweenData->m_fSkewX = m_pEditKeyFrame->m_fSkewX;
    m_pTweenData->m_fSkewY = m_pEditKeyFrame->m_fSkewY;
}
    
void EditorTween::editRotation(float r)
{
    if(!m_pEditKeyFrame)
    {
        return;
    }
    
    m_pEditKeyFrame->m_fSkewX = r;
    m_pEditKeyFrame->m_fSkewY = -r;
    
    m_pEditKeyFrame->m_fSkewX = fmodf(m_pEditKeyFrame->m_fSkewX, 2 * CS_PI);
    m_pEditKeyFrame->m_fSkewY = fmodf(m_pEditKeyFrame->m_fSkewY, 2 * CS_PI);
    
    m_pTweenData->m_fSkewX = m_pEditKeyFrame->m_fSkewX;
    m_pTweenData->m_fSkewY = m_pEditKeyFrame->m_fSkewY;
}

void EditorTween::editRelativeScale(float scaleX, float scaleY)
{
    if(!m_pEditKeyFrame)
    {
        return;
    }
    
    m_pEditKeyFrame->m_fScaleX += scaleX;
    m_pEditKeyFrame->m_fScaleY += scaleY;
    
    m_pTweenData->m_fScaleX = m_pEditKeyFrame->m_fScaleX;
    m_pTweenData->m_fScaleY = m_pEditKeyFrame->m_fScaleY;
}
    
void EditorTween::editScale(float scaleX, float scaleY)
{
    editScaleX(scaleX);
    editScaleY(scaleY);
}

void EditorTween::editRelativeScaleX(float scaleX)
{
    if(!m_pEditKeyFrame)
    {
        return;
    }
    
    m_pEditKeyFrame->m_fScaleX += scaleX;
    
    m_pTweenData->m_fScaleX = m_pEditKeyFrame->m_fScaleX;
}
    
void EditorTween::editScaleX(float scaleX)
{
    if(!m_pEditKeyFrame)
    {
        return;
    }
    
    m_pEditKeyFrame->m_fScaleX = scaleX;
    
    m_pTweenData->m_fScaleX = m_pEditKeyFrame->m_fScaleX;
}

void EditorTween::editRelativeScaleY(float scaleY)
{
    if(!m_pEditKeyFrame)
    {
        return;
    }
    
    m_pEditKeyFrame->m_fScaleY += scaleY;
    
    m_pTweenData->m_fScaleY = m_pEditKeyFrame->m_fScaleY;
}
    
void EditorTween::editScaleY(float scaleY)
{
    if(!m_pEditKeyFrame)
    {
        return;
    }
    
    m_pEditKeyFrame->m_fScaleY = scaleY;
    
    m_pTweenData->m_fScaleY = m_pEditKeyFrame->m_fScaleY;
}

int EditorTween::getFromKeyFrameIndex()
{
	return m_iFromIndex;
}

int EditorTween::getToKeyFrameIndex()
{
	return m_iToIndex;
}

void EditorTween::setKeyFrameDisplayIndex(int index)
{
	if(m_pMovementBoneData)
	{
		FrameData* _frame =  m_pMovementBoneData->getFrameData(m_iFromIndex);
		_frame->setDisplayIndex(index);
	}
	
}

int EditorTween::getKeyFrameDisplayIndex()
{
	if (m_pMovementBoneData == NULL)
	{
		return 0;
	}

	FrameData* _frame =  m_pMovementBoneData->getFrameData(m_iFromIndex);

	if (_frame == NULL)
	{
		return 0;
	}
	return _frame->getDisplayIndex();
}

void EditorTween::movementBoneDataDeleted()
{
	m_pMovementBoneData = NULL;
}

bool EditorTween::isAtKeyFrame()
{
	return m_bIsAtKeyFrame;
}

}
