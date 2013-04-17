#include "CSAnimation.h"
#include "CSArmature.h"
#include "CSBone.h"
#include "CSArmatureDefine.h"
#include "CSUtilMath.h"
#include "CSConstValue.h"


using namespace std;

namespace cs {

        
Animation *Animation::create(Armature *armature)
{
    Animation *pARmatureAnimation = new Animation();
    if (pARmatureAnimation && pARmatureAnimation->init(armature))
    {
        pARmatureAnimation->autorelease();
        return pARmatureAnimation;
    }
    CC_SAFE_DELETE(pARmatureAnimation);
    return NULL;
}

    
Animation::Animation()
    :m_pAnimationData(NULL)
    ,m_pCurrentFrameData(NULL)
    ,m_strMovementID("")
    ,m_pArmature(NULL)
    ,m_iToIndex(0)
{

}

Animation::~Animation(void)
{
}

bool Animation::init(Armature *armature)
{
    bool bRet = false;
    do
    {
        m_pArmature = armature;
        
        bRet = true;
    }
    while (0);
    
    return bRet;
}


void Animation:: pause()
{
    
    
    CCDictElement *_element = NULL;
    CCDictionary *_dict = m_pArmature->getBoneDic();
    CCDICT_FOREACH(_dict, _element)
    {
        Bone *bone = (Bone*)_element->getObject();
        if (bone->getTween())
        {
            bone->getTween()->pause();
        }
    }
    
    ProcessBase::pause();
    
}
  
void Animation::resume()
{
    
    
    CCDictElement *_element = NULL;
    CCDictionary *_dict = m_pArmature->getBoneDic();
    CCDICT_FOREACH(_dict, _element)
    {
        Bone* bone = (Bone*)_element->getObject();
        if (bone->getTween())
        {
            bone->getTween()->resume();
        }
    }
    
    ProcessBase::resume();
}

void Animation::stop()
{
    
    
    CCDictElement *_element = NULL;
    CCDictionary *_dict = m_pArmature->getBoneDic();
    CCDICT_FOREACH(_dict, _element)
    {
        Bone *bone = (Bone*)_element->getObject();
        if (bone->getTween())
        {
            bone->getTween()->stop();
        }
    }
    
    ProcessBase::stop();
}

void Animation::setAnimationScale(float _animationScale )
{
    if(_animationScale == m_fAnimationScale)
    {
        return;
    }
    
    /*
     *  when calculate m_fCurrentFrame, we will do a processing of m_fCurrentFrame += m_fAnimationScale * (dt/m_fAnimationInternal);
     *  then if m_fAnimationScale is bigger, the m_fCurrentFrame grow quicker, animation 
     *  scale up.
     *  We want animation slower when m_fAnimationScale is bigger, so do 1/_animationScale.
     */
    m_fAnimationScale = 1/_animationScale;
    
    CCDictElement *_element = NULL;
    CCDictionary *_dict = m_pArmature->getBoneDic();
    CCDICT_FOREACH(_dict, _element)
    {
        Bone *bone = (Bone*)_element->getObject();
        if (bone->getTween())
        {
            bone->getTween()->setAnimationScale(m_fAnimationScale);
        }
    }
}


void Animation::play(const char *animationName, int durationTo, int durationTween,  int loop, int tweenEasing)
{
    CCAssert(m_pAnimationData, "m_pAnimationData can not be null");
    
    m_pMovementData = m_pAnimationData->getMovement( animationName );
    CCAssert(m_pMovementData, "m_pMovementData can not be null");
    
	//! Get key frame count
	m_iRawDuration = m_pMovementData->getDuration();

    m_strMovementID = animationName;
    
	//! Further processing parameters
    durationTo = (durationTo == -1) ? m_pMovementData->getDurationTo() : durationTo;
    
    durationTween = (durationTween == -1) ? m_pMovementData->getDurationTween() : durationTween;
    durationTween = (durationTween == 0) ? m_pMovementData->getDuration() : durationTween;
    
    tweenEasing	= (tweenEasing == TWEEN_EASING_MAX) ? m_pMovementData->getTweenEasing() : tweenEasing;
    loop = (loop < 0) ? m_pMovementData->getLoop() : loop;
    
    
    ProcessBase::play((void*)animationName, durationTo, durationTween, loop, tweenEasing);
    
    
	if (m_iRawDuration == 1)
	{
		m_eLoopType = SINGLE_FRAME;
	}
	else
	{
		if (loop)
		{
			m_eLoopType = ANIMATION_TO_LOOP_FRONT;
		}
		else
		{
			m_eLoopType = ANIMATION_NO_LOOP;
			m_iRawDuration --;
		}
		m_iDurationTween = durationTween;
	}
    
    MovementBoneData *_movementBoneData = NULL;
    
    CCDictElement *element = NULL;
    CCDictionary *dict = m_pArmature->getBoneDic();
    CCDICT_FOREACH(dict, element)
    {
        Bone *bone = (Bone*)element->getObject();
        _movementBoneData = m_pMovementData->getMovementBoneData(bone->getName().c_str());

        if(_movementBoneData && _movementBoneData->getFrameCount()>0)
        {
            bone->getTween()->play(_movementBoneData, durationTo, durationTween, loop, tweenEasing);
        }
        else
        {
			if(!bone->getIgnoreMovementBoneData())
			{
				//! this bone is not include in this movement, so hide it
				bone->getDisplayManager()->changeDisplayByIndex(-1, false);
				if (bone->getTween())
				{
					bone->getTween()->stop();
				}
			}
            
        }
    }
    
}


void Animation::playByIndex(int animationIndex, int durationTo, int durationTween,  int loop, int tweenEasing)
{
    std::vector<std::string> &movName = m_pAnimationData->getMovNames();
    CC_ASSERT((animationIndex > -1) && (animationIndex < movName.size()));
    
    
    std::string animationName = movName.at(animationIndex);
    play(animationName.c_str(), durationTo, durationTween, loop, tweenEasing);
}



int Animation::getMovementCount()
{
    return m_pAnimationData->getMovementCount();
}
    
void Animation::updateHandler()
{
	if (m_fCurrentPercent >= 1)
	{
		switch(m_eLoopType)
		{
		case ANIMATION_NO_LOOP:

			m_eLoopType = ANIMATION_MAX;
			m_fCurrentFrame = (m_fCurrentPercent-1) * m_iNextFrameIndex;
			m_fCurrentPercent = m_fCurrentFrame / m_iDurationTween;

			if (m_fCurrentPercent >= 1.0f)
			{
			}
			else
			{
				m_iNextFrameIndex = m_iDurationTween;

				m_pArmature->onMovementEvent(START, m_strMovementID.c_str());
				MovementEventSignal.emit(m_pArmature, START, m_strMovementID.c_str());

				break;
			}
		case ANIMATION_MAX:
		case SINGLE_FRAME:
			m_fCurrentPercent = 1;
			m_bIsComplete = true;

			m_pArmature->onMovementEvent(COMPLETE, m_strMovementID.c_str());
			MovementEventSignal.emit(m_pArmature, COMPLETE, m_strMovementID.c_str());

			break;
		case ANIMATION_TO_LOOP_FRONT:
			m_eLoopType = ANIMATION_LOOP_FRONT;
			m_fCurrentPercent = fmodf(m_fCurrentPercent, 1);
			m_fCurrentFrame = fmodf(m_fCurrentFrame, m_iNextFrameIndex);
			m_iNextFrameIndex = m_iDurationTween > 0 ? m_iDurationTween : 1;

			m_pArmature->onMovementEvent(START, m_strMovementID.c_str());
			MovementEventSignal.emit(m_pArmature, START, m_strMovementID.c_str());

			break;
		default:
			//                m_eLoopType += int(m_fCurrentPercent);
			m_fCurrentPercent = fmodf(m_fCurrentPercent, 1);
			m_fCurrentFrame = fmodf(m_fCurrentFrame, m_iNextFrameIndex);
			m_iToIndex = 0;

			m_pArmature->onMovementEvent(LOOP_COMPLETE, m_strMovementID.c_str());
			MovementEventSignal.emit(m_pArmature, LOOP_COMPLETE, m_strMovementID.c_str());

			break;
		}
	}

	if (m_eLoopType == ANIMATION_LOOP_FRONT || m_eLoopType == ANIMATION_LOOP_BACK)
	{
		updateFrameData(m_fCurrentPercent);
	}
}

    
void Animation::updateFrameData(float _currentPercent)
{
	m_iPrevFrameIndex = m_iCurFrameIndex;
    m_iCurFrameIndex = m_iRawDuration * _currentPercent;
    m_iCurFrameIndex = m_iCurFrameIndex % m_iRawDuration;

// 	if (m_iPrevFrameIndex != m_iCurFrameIndex)
// 	{
// 
// 	}
    
//    updateMovementFrameData(_currentPercent);
}
    
void Animation::updateMovementFrameData(float _currentPercent)
{
    int _length = m_pMovementData->getMovFrameDataArr()->count();
    
    if(_length == 0){
        return;
    }
    float _played = m_iRawDuration * _currentPercent;
    
    if (!m_pCurrentFrameData || _played >= m_pCurrentFrameData->m_iDuration + m_pCurrentFrameData->m_iStart || _played < m_pCurrentFrameData->m_iStart) {
        while (true) {
            m_pCurrentFrameData =  m_pMovementData->getMoveFrameData(m_iToIndex);
            if (++m_iToIndex >= _length) {
                m_iToIndex = 0;
            }
            if(m_pCurrentFrameData && _played >= m_pCurrentFrameData->m_iStart && _played < m_pCurrentFrameData->m_iDuration + m_pCurrentFrameData->m_iStart){
                break;
            }
        }
        if(m_pCurrentFrameData->m_strEvent.length() != 0){
            m_pArmature->onMovementEvent(MOVEMENT_EVENT_FRAME, m_pCurrentFrameData->m_strEvent.c_str());
        }
        if(m_pCurrentFrameData->m_strSound.length() != 0){
            m_pArmature->onMovementEvent(SOUND_FRAME, m_pCurrentFrameData->m_strSound.c_str());
        }
        if(m_pCurrentFrameData->m_strMovement.length() != 0){
            play(m_pCurrentFrameData->m_strMovement.c_str());
        }
    }
}

 
}