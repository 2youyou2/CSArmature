#include "CSEditorAnimation.h"
#include "CSArmature.h"

using namespace std;

namespace cs {

        
EditorAnimation *EditorAnimation::create(Armature *armature)
{
    EditorAnimation *pARmatureAnimation = new EditorAnimation();
    if (pARmatureAnimation && pARmatureAnimation->init(armature))
    {
        pARmatureAnimation->autorelease();
        return pARmatureAnimation;
    }
    CC_SAFE_DELETE(pARmatureAnimation);
    return NULL;
}

    
EditorAnimation::EditorAnimation()
{
}

EditorAnimation::~EditorAnimation()
{
}

void EditorAnimation::gotoFrame(int frameIndex)
{
    if(!m_pMovementData)
    {
        return;
    }
    
    CCDictElement *_element = NULL;
	CCDictionary *_dict = m_pArmature->getBoneDic();
    CCDICT_FOREACH(_dict, _element)
    {
        Bone *bone = (Bone*)_element->getObject();
        MovementBoneData *_movementBoneData = m_pMovementData->getMovementBoneData(bone->getName().c_str());
        
        if(_movementBoneData && _movementBoneData->getFrameCount()>0 && bone->getTween())
        {
            bone->getTween()->gotoFrame(frameIndex);
        }
    }
    
    ProcessBase::gotoFrame(frameIndex);
}
    
void EditorAnimation::editMovement(const char *animationName)
{
    play(animationName);
    stop();
}

void EditorAnimation::editMovementByIndex(int index)
{
    std::vector<std::string> &movName = getAnimationData()->getMovNames();
    CCAssert((index > -1) && (index < movName.size()), "");
    
    std::string animationName = movName.at(index);
    editMovement(animationName.c_str());
}
    
        
}
