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


#ifndef COCOSTUDIO_CSSPRITEDISPLAYDATA_H
#define COCOSTUDIO_CSSPRITEDISPLAYDATA_H

#include "cocos2d.h"
#include "CSJsonData.h"
#include "CSDisplayData.h"

using namespace cocos2d;

namespace cs {

    
class CS_EXTERN SpriteDisplayData : public DisplayData
{
public:
    static SpriteDisplayData *createWithJson(const char *pszDescription);
	static SpriteDisplayData *create();
public:
    SpriteDisplayData();
    virtual ~SpriteDisplayData();
    
    
    virtual bool init();
    
    virtual bool initWithJson(const char *pszDescription);
    
    /*
     *  @brief   update data's json value
     *
     */
    virtual void updateJson();
    
    
    void setParam(const char *_displayName)
    {
        m_strDisplayName = _displayName;
        //m_strImageName = _imageName;
    }
    
    void setDisplayName(const char *str){ m_strDisplayName = str; }
    //void setImageName(const char *str){ m_strImageName = str; }
    std::string &getDisplayName(){ return m_strDisplayName; }
    //std::string &getImageName(){ return m_strImageName; }
    
    
    void copy(SpriteDisplayData *_displayData);
    
private:
    
    /**
     *  @brief  If DisplayType is CS_DISPLAY_SPRITE, then Bone will use this image name to create a CCSprite from CCSpriteFrameCache.
     *          It should note that when use this name to create CCSprite from CCSpriteFrameCache, you should use m_strDisplayName + ".png", because when use Texture Packer to pack single image file, the name have ".png".
     *
     *          If DisplayType is CS_DISPLAY_ARMATURE, the name is the Armature's name. When Bone init display and type is CS_DISPLAY_ARMATURE,
     *          then Bone will create a Armature.
     */
    std::string m_strDisplayName;

    
};


}

#endif