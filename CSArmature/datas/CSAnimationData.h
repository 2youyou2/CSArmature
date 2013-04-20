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

#ifndef COCOSTUDIO_CSANIMATIONDATA_H
#define COCOSTUDIO_CSANIMATIONDATA_H

#include "CSMovementData.h"


namespace cs {

/**
 *  AnimationData include all movement infomation for the Armature
 *  The struct is AnimationData -> MovementData -> MovementBoneData -> FrameData
 *                                              -> MovementFrameData
 */
class CS_EXTERN AnimationData : public CCObject, public sigslot::has_slots<>  {
public:
    static AnimationData *createWithJson(const char *pszDescription);
    static AnimationData *create();
public:
    AnimationData(void);
    ~AnimationData(void);

    /**
     * Create CCDictionary mBoneDatas
     */
    virtual bool init();
    
    virtual bool initWithJson(const char *pszDescription);
    
    /*
     * Update data's json value
     */
    void updateJson();
    

    MovementData *addMovement(MovementData* _movData);
    MovementData *addMovement(const char *_movName, const char *_pszDescription = NULL);
    MovementData *removeMovement(const char* _movName);


    MovementData *getMovement(const char* _movName);
    CCDictionary *getMovementDic();
    int getMovementCount();

	/**
     * Get movment name list 
     *
	 * @return	std::vector<std::string>, the movement name vector
     */
    std::vector<std::string> &getMovNames();
    
private:
    /*
     * AnimationData Name used to mark this Aimation is used for which Armature
     */
	CC_SYNTHESIZE_PASS_BY_REF(std::string, m_strName, Name)
private:
	/**
	 * Save movment datas
	 * @key	std::string
	 * @value	MovementData *
     */
    CCDictionary *m_pMovDataDic;
    
    /**
	 * A vector to save all movement names in this animation
     */
    std::vector<std::string> m_vecMovNames;

    //!slot
public:
    void connectMovementDataSignal(MovementData *_movementData);
    void disconnectMovementDataSignal(MovementData *_movementData);
    
    void movementNameChanged(MovementData *_movementData, const char *_movName);


};


}

#endif