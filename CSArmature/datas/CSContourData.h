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

#ifndef COCOSTUDIO_CSCONTOURDATA_H
#define COCOSTUDIO_CSCONTOURDATA_H

#include "cocos2d.h"
#include "CSJsonData.h"

using namespace cocos2d;

namespace cs {

struct ContourVertex2 : public CCObject
{
	ContourVertex2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	float x;
	float y;
};

/*
 * ContourData include a contour vertex information 
 */
class  ContourData : public CCObject, public JsonData
{
public:
    static ContourData *createWithJson(const char *pszDescription);
	static ContourData *create();
public:
	ContourData();
	~ContourData(void);
    
    virtual bool init();
    
    virtual bool initWithJson(const char *pszDescription);
    
    /*
     * Update data's json value
     */
    virtual void updateJson();
    
    /*
     * Add a vertex to this contour, this method will retain this CCPoint, and release it when remove it.
     */
    void addVertex(ContourVertex2 *_vertex);
    
    /*
     * Insert a vertex to this contour, this method will retain this CCPoint, and release it when remove it.
     */
    void insertVertex(ContourVertex2 *_vertex, int _index);
    
    /*
     * Remove vertex
     */
    void removeVertex(ContourVertex2 *_vertex);
    
	int indexOfVertex(ContourVertex2 *_vertex);

    /*
     * Get the vertex form given index
     */
    ContourVertex2 *getVertex(int _index);
    
    
    /*
     * Get the vertex count
     */
    int getVertexCount();
    
    /*
     * Get the vertex list
     */
    const CCArray *getVertexList();
    
private:
    //! Save contour vertex info, vertex saved in a CCPoint
	CCArray *m_pVertexList;
};
		

}

#endif
