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

#ifndef COCOSTUDIO_CSTEXTUREDATA_H
#define COCOSTUDIO_CSTEXTUREDATA_H

#include "CSContourData.h"
#include "cocos2d.h"
#include "sigslot.h"

using namespace cocos2d;

namespace cs {

/*
 * TextureData include a texture's information 
 */
class CS_EXTERN TextureData : public CCObject
{
public:
    static TextureData *createWithJson(const char *pszDescription);
	static TextureData *create();
public:
	TextureData();
	~TextureData(void);
    
    virtual bool init();
    
    virtual bool initWithJson(const char *pszDescription);
    
    /*
     * Update data's json value
     */
    virtual void updateJson();
public:
    /*
     * Add a ContourData to this contour, this method will retain this ContourData, and release it when remove it.
     */
    void addContourData(ContourData *_contourData);
    
    /*
     * Remove ContourData
     */
    void removeContourData(ContourData *_contourData);
    
    /*
     * Get the ContourData form given index
     */
    ContourData *getContourData(int _index);
    
    CCArray *getContourDatas();
    
    /*
     * Get the ContourData count
     */
    int getContourCount();

protected:
	//! The texture's name
	CC_SYNTHESIZE_PASS_BY_REF(std::string, m_strName, Name);

	//! The texture's anchor point
	CC_PROPERTY(float, m_fPivotX, PivotX);
	CC_PROPERTY(float, m_fPivotY, PivotY);

	//! The texture's width, height
	CC_SYNTHESIZE(float, m_fWidth, Width);
	CC_SYNTHESIZE(float, m_fHeight, Height);
private:
	CCArray *m_pContourDataList;

public:
	sigslot::signal2<float, float> AnchorPointChangedSignal;
};
		

}

#endif

