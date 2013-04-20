#include "CSTween.h"
#include "CSBone.h"
#include "CSArmature.h"
#include "CSUtilMath.h"
#include "CSTweenFunction.h"
#include "cocos2d.h"

namespace cs {


Tween* Tween::create(Bone *bone)
{
    Tween *pTween = new Tween();
    if (pTween && pTween->init(bone))
    {
        pTween->autorelease();
        return pTween;
    }
    CC_SAFE_DELETE(pTween);
    return NULL;

}



Tween::Tween()
    :m_pFrom(NULL)
    ,m_pTweenData(NULL)
    ,m_pBetween(NULL)
    ,m_pBone(NULL)
	,m_pCurrentKeyFrame(NULL)
	,m_pMovementBoneData(NULL)
	,m_eFrameTweenEasing(Linear)
    ,m_iFromIndex(0)
    ,m_iToIndex(0)
    ,m_pEditKeyFrame(NULL)
{

}


Tween::~Tween(void)
{
    CC_SAFE_DELETE( m_pFrom );
    CC_SAFE_DELETE( m_pTweenData );
    CC_SAFE_DELETE( m_pBetween );
}


bool Tween::init(Bone *bone)
{
    bool bRet = false;
    do
    {

        m_pFrom = FrameData::create();
        m_pFrom->retain();
        
		m_pTweenData = FrameData::create();
        m_pTweenData->retain();

        m_pBetween = FrameData::create();
        m_pBetween->retain();
        
        m_pBone = bone;

        bRet = true;
    }
    while (0);

    return bRet;
}


void Tween::play(MovementBoneData *_movementBoneData, int _durationTo, int _durationTween,  int _loop, int _tweenEasing)
{
    
    ProcessBase::play(NULL, _durationTo, _durationTween, _loop, _tweenEasing);
   
	m_eLoopType = (AnimationType)_loop;

    m_pCurrentKeyFrame = NULL;
    m_pEditKeyFrame = NULL;
    m_bIsTweenKeyFrame = false;
    
    m_iTotalDuration = 0;
    m_iBetweenDuration = 0;
    m_iToIndex = 0;
    
    setMovementBoneData(_movementBoneData);
    

    if (m_pMovementBoneData->getFrameDatas()->count() == 1)
    {
		m_eLoopType = SINGLE_FRAME;
		FrameData *_nextKeyFrame = m_pMovementBoneData->getFrameData(0);
		if(_durationTo == 0)
		{
			setBetween(_nextKeyFrame, _nextKeyFrame);
		}else{
			setBetween(m_pTweenData, _nextKeyFrame);
		}
        m_bIsTweenKeyFrame = true;
        m_eFrameTweenEasing = Linear;
		m_iRawDuration = m_pMovementBoneData->getDuration();
        m_iFromIndex = m_iToIndex =0;
    }
    else if (m_pMovementBoneData->getFrameDatas()->count() > 1)
    {
		if (_loop)
		{
			m_eLoopType = ANIMATION_TO_LOOP_BACK;
			m_iRawDuration = m_pMovementBoneData->getDuration();
		}
		else
		{
			m_eLoopType = ANIMATION_NO_LOOP;
			m_iRawDuration = m_pMovementBoneData->getDuration() - 1;
		}

        m_iDurationTween = _durationTween * m_pMovementBoneData->getScale();

        if (_loop && m_pMovementBoneData->getDelay() != 0)
        {
            setBetween(m_pTweenData, tweenNodeTo(updateFrameData(1 - m_pMovementBoneData->getDelay()), m_pBetween));
            
        }
        else
        {
			FrameData *_nextKeyFrame = m_pMovementBoneData->getFrameData(0);
			setBetween(m_pTweenData, _nextKeyFrame);
            m_bIsTweenKeyFrame = true;
        }
    }
}

FrameData *Tween::getTweenNode()
{
	return m_pTweenData;
}
    
void Tween::updateHandler()
{
	

	if (m_fCurrentPercent >= 1)
	{
		switch(m_eLoopType)
		{
		case SINGLE_FRAME:
			m_fCurrentPercent = 1;
			m_bIsComplete = true;
			break;
		case ANIMATION_NO_LOOP:
			m_eLoopType = ANIMATION_MAX;


			if (m_iDurationTween <= 0)
			{
				m_fCurrentPercent = 1;
			}
			else
			{
				m_fCurrentPercent = (m_fCurrentPercent - 1) * m_iNextFrameIndex / m_iDurationTween;
			}

			if (m_fCurrentPercent >= 1)
			{
				m_fCurrentPercent = 1;
				m_bIsComplete = true;
				break;
			}
			else
			{
				m_iNextFrameIndex = m_iDurationTween;
				m_fCurrentFrame = m_fCurrentPercent * m_iNextFrameIndex;
				m_iTotalDuration = 0;
				m_iBetweenDuration = 0;
				m_iToIndex = 0;
				break;
			}
		case ANIMATION_TO_LOOP_BACK:
			m_eLoopType = ANIMATION_LOOP_BACK;

			m_iNextFrameIndex = m_iDurationTween > 0 ? m_iDurationTween : 1;

			if (m_pMovementBoneData->getDelay() != 0)
			{
				//
				m_fCurrentFrame = (1 - m_pMovementBoneData->getDelay()) * (float)m_iNextFrameIndex;
				m_fCurrentPercent = m_fCurrentFrame / m_iNextFrameIndex;


			}else
			{
				m_fCurrentPercent = 0;
				m_fCurrentFrame = 0;
			}

			m_iTotalDuration = 0;
			m_iBetweenDuration = 0;
			m_iToIndex = 0;
			break;
		case ANIMATION_MAX:
			m_fCurrentPercent = 1;
			m_bIsComplete = true;
			break;
		default:
			//            m_eLoopType += int(m_fCurrentPercent);

			m_fCurrentPercent = fmodf(m_fCurrentPercent, 1);
			m_fCurrentFrame = fmodf(m_fCurrentFrame, m_iNextFrameIndex);

			m_iTotalDuration = 0;
			m_iBetweenDuration = 0;
			m_iToIndex = 0;
			break;
		}
	}


	if (m_fCurrentPercent < 1 && m_eLoopType <= ANIMATION_TO_LOOP_BACK)
	{
		m_fCurrentPercent = sin(m_fCurrentPercent * CS_HALF_PI);
	}

	float percent  = m_fCurrentPercent;

	if (m_eLoopType > ANIMATION_TO_LOOP_BACK)
	{
		percent = updateFrameData(percent, true);
	}

	if(m_eFrameTweenEasing != TWEEN_EASING_MAX)
	{
		tweenNodeTo(percent);
	}
	else if(m_pCurrentKeyFrame)
	{
		tweenNodeTo(0);
	}

}

void Tween::setBetween(FrameData *from, FrameData *to)
{
	do 
	{
		if(to->getDisplayIndex() < 0)
		{
			m_pFrom->copy(from);
			m_pBetween->subtract(to, to);
			break;
		}
		else if(from->getDisplayIndex() < 0)
		{
			m_pFrom->copy(to);
			m_pBetween->subtract(to, to);
			break;
		}

		m_pFrom->copy(from);
		m_pBetween->subtract(from, to);
	} while (0);

	arriveKeyFrame(from);
}


void Tween::arriveKeyFrame(FrameData *keyFrameData)
{
	if(keyFrameData)
	{
		int displayIndex = keyFrameData->getDisplayIndex();

		if (!m_pBone->getDisplayManager()->getForceChangeDisplay())
		{
			m_pBone->getDisplayManager()->changeDisplayByIndex(displayIndex, false);
		}


		m_pBone->setZOrder(keyFrameData->m_iZOrder);

		Armature *childAramture = m_pBone->getChildArmature();

		if(childAramture)
		{
			if(keyFrameData->m_strMovement.compare("") != 0)
			{
				childAramture->getAnimation()->play(keyFrameData->m_strMovement.c_str());
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


FrameData *Tween::tweenNodeTo(float percent, FrameData *node)
{
	node = node == NULL ? m_pTweenData : node;

	node->m_fX = m_pFrom->m_fX + percent * m_pBetween->m_fX;
	node->m_fY = m_pFrom->m_fY + percent * m_pBetween->m_fY;
	node->m_fScaleX = m_pFrom->m_fScaleX + percent * m_pBetween->m_fScaleX;
	node->m_fScaleY = m_pFrom->m_fScaleY + percent * m_pBetween->m_fScaleY;
	node->m_fSkewX = m_pFrom->m_fSkewX + percent * m_pBetween->m_fSkewX;
	node->m_fSkewY = m_pFrom->m_fSkewY + percent * m_pBetween->m_fSkewY;

	m_pBone->setTransformDirty(true);

	if(m_pBetween->m_bUseColorInfo)
	{
		node->m_cColor.a = m_pFrom->m_cColor.a + percent * m_pBetween->m_cColor.a;
		node->m_cColor.r = m_pFrom->m_cColor.r + percent * m_pBetween->m_cColor.r;
		node->m_cColor.g = m_pFrom->m_cColor.g + percent * m_pBetween->m_cColor.g;
		node->m_cColor.b = m_pFrom->m_cColor.b + percent * m_pBetween->m_cColor.b;

		m_pBone->setColorDirty(true);
	}

//    CCPoint p1 = ccp(m_pFrom->m_fX, m_pFrom->m_fY);
//    CCPoint p2 = ccp(100, 0);
//    CCPoint p3 = ccp(200, 400);
//    CCPoint p4 = ccp(m_pFrom->m_fX + m_pBetween->m_fX, m_pFrom->m_fY + m_pBetween->m_fY);
//    
//    CCPoint p = bezierTo(percent, p1, p2, p3, p4);
//    node->m_fX = p.x;
//    node->m_fY = p.y;

	return node;
}

float Tween::updateFrameData(float currentPrecent, bool activeFrame)
{

    float playedTime = (float)m_iRawDuration * currentPrecent;
    
    
    FrameData *from;
    FrameData *to;
    bool isListEnd;

    //! If play to current frame's front or back, then find current frame again
    if (playedTime >= m_iTotalDuration || playedTime < m_iTotalDuration - m_iBetweenDuration)
    {
        /*
         *  Get frame length, if m_iToIndex >= _length, then set m_iToIndex to 0, start anew.
         *  m_iToIndex is next index will play
         */
        int length = m_pMovementBoneData->getFrameCount();
        do
        {
            m_iBetweenDuration = m_pMovementBoneData->getFrameData(m_iToIndex)->getDuration();
            m_iTotalDuration += m_iBetweenDuration;
            m_iFromIndex = m_iToIndex;
            
            if (++m_iToIndex >= length)
            {
                m_iToIndex = 0;
            }
        }
        while (playedTime >= m_iTotalDuration);
        
        
        isListEnd = m_eLoopType == ANIMATION_MAX && m_iToIndex == 0;
        
        if(isListEnd)
        {
            to = from = m_pMovementBoneData->getFrameData(m_iFromIndex);
        }
        else
        {
            from = m_pMovementBoneData->getFrameData(m_iFromIndex);
            to = m_pMovementBoneData->getFrameData(m_iToIndex);
        }
        
        m_eFrameTweenEasing = from->getTweenEasing();
        
        setBetween(from, to);
        
    }
    currentPrecent = 1 - (m_iTotalDuration - playedTime) / (float)m_iBetweenDuration;

    
	/*
     *  If frame tween easing equal to TWEEN_EASING_MAX, then it will not do tween.
     */
    
    TweenType tweenType;
    
 	if ( m_eFrameTweenEasing != TWEEN_EASING_MAX)
    {
        tweenType = (m_eTweenEasing == TWEEN_EASING_MAX) ? m_eFrameTweenEasing : m_eTweenEasing;
        if (tweenType != TWEEN_EASING_MAX)
        {
            currentPrecent = TweenFunction::tweenTo(0, 1, currentPrecent, 1, tweenType);
        }
    }

    return currentPrecent;
}
    

CCPoint Tween::getPosition()
{
	return ccp(m_pTweenData->m_fX, m_pTweenData->m_fY);
}
float Tween::getPositionX()
{
	return m_pTweenData->m_fX;
}
float Tween::getPositionY()
{
	return m_pTweenData->m_fY;
}
float Tween::getRotation()
{
	return m_pTweenData->m_fSkewX;
}
float Tween::getScaleX()
{
	return m_pTweenData->m_fScaleX;
}
float Tween::getScaleY()
{
	return m_pTweenData->m_fScaleY;
}


}