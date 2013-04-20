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

#ifndef COCOSTUDIO_CSSHADERDISPLAYDATA_H
#define COCOSTUDIO_CSSHADERDISPLAYDATA_H

#include "cocos2d.h"

#include "CSDisplayData.h"

using namespace cocos2d;

namespace cs {

    
class CS_EXTERN ShaderDisplayData : public DisplayData
{
public:
    static ShaderDisplayData *createWithJson(const char *pszDescription);
	static ShaderDisplayData *create();
public:
    ShaderDisplayData();
    virtual ~ShaderDisplayData(){};
    
    
    virtual bool init();
    
    virtual bool initWithJson(const char *pszDescription);
    
    /*
     *  @brief   update data's json value
     *
     */
    virtual void updateJson();
    
    
    void setParam(const char *_vert, const char *_frag)
    {
        m_strVert = _vert;
        m_strFrag = _frag;
    }
    
    void setVert(const char *str){ m_strVert = str; }
    void setFrag(const char *str){ m_strFrag = str; }
    std::string &getVert(){ return m_strVert; }
    std::string &getFrag(){ return m_strFrag; }
    
    
    void copy(ShaderDisplayData *_displayData);
    
private:
    
    std::string m_strVert;    
    std::string m_strFrag;

    
};

}

#endif