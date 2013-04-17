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

#ifndef COCOSTUDIO_CSBATCHNODEMANAGER_H
#define COCOSTUDIO_CSBATCHNODEMANAGER_H

#include <iostream>
#include "cocos2d.h"
#include "CSBatchNode.h"

using namespace cocos2d;

namespace cs {
    

class BatchNodeManager {
    
public:
    /*
     * If you want to reset the AlphaTestValue, you can use this method.
     * You should note this method will effect all CCNode used cocos2dx's Alpha Test Shader
     *
     * @param  _value  the value you want to set to Alpha Test, 
     */
    static void setBatchNodeAlphaTestValue(float _value);
public:
    
    static BatchNodeManager *sharedBatchNodeManager();
    
    /*
     * Init members with a CCLayer
     */
    virtual bool initWithLayer(CCLayer *_layer, std::string _layerName);
    
    /*
     * Add a BatchNode to current layer
     *
     * @param  name The image name used by _batchNode, used for a key
     * @param  batchNode  The BatchNode you want to added to current layer
     * @param  zOrder BatchNode's zorder, defualt is 0
     */
    void addBatchNode(const std::string &name, BatchNode *batchNode, int zOrder = 0);
    
    /*
     * Get a BatchNode which used a image named _name from current layer. 
     * If do not exsit, create a new BatchNode named _name and add to current layer
     */
    BatchNode *getBatchNode(const std::string &name);
    
    /*
     * Set the zorder of BatchNode named _name
     *
     * @param name BatchNode's name
     * @param zOrder The zorder you want to set
     */
    void setBatchNodeZOrder(const std::string &name, int zOrder);
    
    void setBatchNodeBlendFunc(const std::string &name, unsigned int src, unsigned int dst);
    
    /*
     * Remove all BatchNodes added to the layer named _layerName
     * @param name The layer's name
     */
    void removeBatchNodes(const std::string &layerName);
    
    CCLayer *getCurrentLayer();
private:
    /*
     * When create a BatchNodeManager, it will set Alpha Test to 0.5
     */
    BatchNodeManager();
    ~BatchNodeManager();
    
    static BatchNodeManager *m_sBatchNodeManager;
    
    /*
     * @key The image name the CCSpriteBatchNode use
     * @value  CCSpriteBatchNode*
     */
    CCDictionary *m_pBatchNodeDic;
    
    /*
     * @key Layer's name
     * @value  BatchNode dictionary
     */
    CCDictionary *m_pLayer2BatchNodeDic;
    
    
    CCLayer *m_pLayer;			//! Current CCLayer

    CCDictionary *m_pLayerMap;	//! a map used to judge if a layer with a name is the same with the layer in the map named with the same name.
};

}

#endif 
