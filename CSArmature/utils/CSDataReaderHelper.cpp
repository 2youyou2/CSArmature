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

#include "CSDataReaderHelper.h"
#include "tinyxml.h"
#include "CSArmatureDataManager.h"
#include "CSTransformHelp.h"
#include "CSSpriteDisplayData.h"
#include "CSArmatureDisplayData.h"
#include "CSArmatureDefine.h"

namespace cs {

std::vector<std::string> DataReaderHelper::m_arrConfigFileList;
float DataReaderHelper::m_fPositionReadScale = 1;

void DataReaderHelper::setPositionReadScale(float scale)
{
	m_fPositionReadScale = scale;
}

float DataReaderHelper::getPositionReadScale()
{
	return m_fPositionReadScale;
}

    
void DataReaderHelper::addDataFromFile(const char *filePath)
{
    /*
     * Check if file is already added to ArmatureDataManager, if then return.
     */
    for(unsigned int i = 0; i<m_arrConfigFileList.size(); i++)
    {
        if (m_arrConfigFileList[i].compare(filePath) == 0)
        {
            return;
        }
    }
    m_arrConfigFileList.push_back(filePath);


    std::string filePathStr = filePath;
    size_t startPos = filePathStr.find_last_of(".");
    std::string str = &filePathStr[startPos];
    
    if (str.compare(".xml") == 0)
    {
        
#if CS_TOOL_PLATFORM
        if(Game::sharedGame()->isUsePackage())
        {
            DataReaderHelper::addDataFromXMLPak(filePathStr.c_str());
        }
        else
        {
            DataReaderHelper::addDataFromXML(filePathStr.c_str());
        }
#else
        DataReaderHelper::addDataFromXML(filePathStr.c_str());
#endif
    }
    else if(str.compare(".json") == 0)
    {
        DataReaderHelper::addDataFromJson(filePathStr.c_str());
    }
}


#pragma region Decode Data From XML   

void DataReaderHelper::addDataFromXML(const char* xmlPath)
{
    
    /*
     *  Need to get the full path of the xml file, or the Tiny XML can't find the xml at IOS
     */
    std::string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(xmlPath);
    
    /*
     *  Need to read the tiny xml into memory first, or the Tiny XML can't find the xml at IOS
     */
    unsigned long	_size;
    const char *_pFileContent = (char*)CCFileUtils::sharedFileUtils()->getFileData(fullPath.c_str() , "r", &_size);
    
    if (_pFileContent)
    {
        addDataFromCache(_pFileContent);
    }
}
    
void DataReaderHelper::addDataFromXMLPak(const char *xmlPakPath)
{
#if CS_TOOL_PLATFORM
    
    char *_pFileContent = NULL;
    JsonReader::getFileBuffer(xmlPakPath, &_pFileContent);
    
    if (_pFileContent)
    {
        addDataFromCache(_pFileContent);
    }
#endif
}

void DataReaderHelper::addDataFromCache(const char *_pFileContent)
{
    TiXmlDocument document;
    document.Parse(_pFileContent, 0, TIXML_ENCODING_UTF8);
    
    TiXmlElement	*root = document.RootElement();
    CCAssert(root, "XML error  or  XML is empty.");
    
    /*
     * Begin decode armature data from xml
     */
    TiXmlElement *armaturesXML = root->FirstChildElement(ARMATURES);
    TiXmlElement *armatureXML = armaturesXML->FirstChildElement(ARMATURE);
    while(armatureXML)
    {
        ArmatureData *armatureData = DataReaderHelper::decodeArmature(armatureXML);
        ArmatureDataManager::sharedArmatureDataManager()->addArmatureData(armatureData->getName().c_str(), armatureData);
        
        armatureXML = armatureXML->NextSiblingElement(ARMATURE);
    }
    
    /*
     * Begin decode animation data from xml
     */
    TiXmlElement *animationsXML = root->FirstChildElement(ANIMATIONS);
    TiXmlElement *animationXML = animationsXML->FirstChildElement(ANIMATION);
    while(animationXML)
    {
        AnimationData *animationData = DataReaderHelper::decodeAnimation(animationXML);
        ArmatureDataManager::sharedArmatureDataManager()->addAnimationData(animationData->getName().c_str(), animationData);
        
        animationXML = animationXML->NextSiblingElement(ANIMATION);
    }
    
    /*
     * Begin decode texture data from xml
     */
    TiXmlElement *texturesXML = root->FirstChildElement(TEXTURE_ATLAS);
    TiXmlElement *textureXML = texturesXML->FirstChildElement(SUB_TEXTURE);
    while(textureXML)
    {
        TextureData *textureData = DataReaderHelper::decodeTexture(textureXML);
        ArmatureDataManager::sharedArmatureDataManager()->addTextureData(textureData->getName().c_str(), textureData);
        
        textureXML = textureXML->NextSiblingElement(SUB_TEXTURE);
    }
    
    document.Clear();
}

ArmatureData *DataReaderHelper::decodeArmature(TiXmlElement *armatureXML)
{
    const char*	name = armatureXML->Attribute(A_NAME);
    
    
    ArmatureData *armatureData = ArmatureData::create();
    armatureData->setName(name);
    
    
    TiXmlElement* boneXML = armatureXML->FirstChildElement(BONE);
    
    while( boneXML )
    {
        /*
         *  If this bone have parent, then get the parent bone xml
         */
        const char *parentName = boneXML->Attribute(A_PARENT);
        TiXmlElement *parentXML = NULL;
        if (parentName)
        {
            parentXML = armatureXML->FirstChildElement(BONE);
            std::string name = parentName;
            while (parentXML)
            {
                if (name.compare(parentXML->Attribute(A_NAME)) == 0)
                {
                    break;
                }
                parentXML = parentXML->NextSiblingElement(BONE);
            }
        }
        
        BoneData *boneData = decodeBone(boneXML, parentXML);
        armatureData->addBoneData(boneData);
        
        boneXML = boneXML->NextSiblingElement(BONE);
    }
  
    return armatureData;
}



BoneData *DataReaderHelper::decodeBone(TiXmlElement *boneXML, TiXmlElement *_parentXML)
{
    
    std::string name = boneXML->Attribute(A_NAME);
    
    CCAssert(name.compare("") != 0, "");

    
    BoneData *boneData = BoneData::create();
    
    boneData->m_strName = name;
    
    if( boneXML->Attribute(A_PARENT) != NULL )
    {
        boneData->m_strParent = boneXML->Attribute(A_PARENT);
    }
    
//     boneXML->QueryFloatAttribute(A_X,		&(boneData->m_fX));
//     boneXML->QueryFloatAttribute(A_Y,		&(boneData->m_fY));
//     boneXML->QueryFloatAttribute(A_SKEW_X, &(boneData->m_fSkewX));
//     boneXML->QueryFloatAttribute(A_SKEW_Y, &(boneData->m_fSkewY));
//     boneXML->QueryFloatAttribute(A_SCALE_X,&(boneData->m_fScaleX));
//     boneXML->QueryFloatAttribute(A_SCALE_Y,&(boneData->m_fScaleY));
//     boneXML->QueryIntAttribute(A_Z,		&(boneData->m_iZOrder));
//     
//     boneXML->QueryIntAttribute(A_ALPHA,	&(boneData->m_iAlpha));
//     boneXML->QueryIntAttribute(A_RED,		&(boneData->m_iRed));
//     boneXML->QueryIntAttribute(A_GREEN,	&(boneData->m_iGreen));
//     boneXML->QueryIntAttribute(A_BLUE,		&(boneData->m_iBlue));
//     
//     // flash export data is a percent value, so wo change it
//     boneData->m_iAlpha *= 2.55;
//     boneData->m_iRed *= 2.55;
//     boneData->m_iGreen *= 2.55;
//     boneData->m_iBlue *= 2.55;
//     
//     /*
//      *  Note : Flash's Y value contrary to Cocos2dX's Y value
//      */
//     boneData->m_fY = -boneData->m_fY;
//     
//     /*
//      *  Note : Flash export datas are DEGREE values, and our matrix use RADIAN values
//      */
//     boneData->m_fSkewX = CC_DEGREES_TO_RADIANS(boneData->m_fSkewX);
//     boneData->m_fSkewY = CC_DEGREES_TO_RADIANS(-boneData->m_fSkewY);
//     
//     
//     if(_parentXML)
//     {
//         /*
//          *  recalculate bone data from parent bone data, use for translate matrix
//          */
//         Node _helpNode;
//         _parentXML->QueryFloatAttribute(A_X, &_helpNode.m_fX);
//         _parentXML->QueryFloatAttribute(A_Y, &_helpNode.m_fY);
//         _parentXML->QueryFloatAttribute(A_SKEW_X, &_helpNode.m_fSkewX);
//         _parentXML->QueryFloatAttribute(A_SKEW_Y, &_helpNode.m_fSkewY);
//         
//         _helpNode.m_fY = -_helpNode.m_fY;
//         _helpNode.m_fSkewX = CC_DEGREES_TO_RADIANS(_helpNode.m_fSkewX);
//         _helpNode.m_fSkewY = CC_DEGREES_TO_RADIANS(-_helpNode.m_fSkewY);
//         
//         TransformHelp::transformFromParent(*boneData, _helpNode);
//     }
    
    
    TiXmlElement *displayXML = boneXML->FirstChildElement(DISPLAY);
    while(displayXML)
    {
        DisplayData *displayData = decodeBoneDisplay(displayXML);
        boneData->addDisplayData(displayData);
        
        displayXML = displayXML->NextSiblingElement(DISPLAY);
    }
    
    
    return boneData;
    
}

DisplayData *DataReaderHelper::decodeBoneDisplay(TiXmlElement *displayXML)
{
    int _isArmature = 0;
    
    DisplayData *displayData;
    
    
    if( displayXML->QueryIntAttribute(A_IS_ARMATURE, &(_isArmature)) == TIXML_SUCCESS )
    {
        if(!_isArmature)
        {
            displayData = SpriteDisplayData::create();
            displayData->setDisplayType(CS_DISPLAY_SPRITE);
        }
        else
        {
            displayData = ArmatureDisplayData::create();
            displayData->setDisplayType(CS_DISPLAY_ARMATURE);
        }
        
    }else{
        displayData = SpriteDisplayData::create();
        displayData->setDisplayType(CS_DISPLAY_SPRITE);
    }
    
    if(displayXML->Attribute(A_NAME) != NULL )
    {
        if(!_isArmature)
        {
            ((SpriteDisplayData*)displayData)->setDisplayName(displayXML->Attribute(A_NAME));
        }
        else
        {
            ((ArmatureDisplayData*)displayData)->setDisplayName(displayXML->Attribute(A_NAME));
        }
        
    }
    
    
    return displayData;
}
    
AnimationData *DataReaderHelper::decodeAnimation(TiXmlElement *animationXML)
{
    const char*	name = animationXML->Attribute(A_NAME);
    
    
    AnimationData *aniData =  AnimationData::create();
    
    ArmatureData *armatureData = ArmatureDataManager::sharedArmatureDataManager()->getArmatureData(name);
    
    aniData->setName(name);
    
    TiXmlElement* movementXML = animationXML->FirstChildElement(MOVEMENT);
    
    while( movementXML )
    {
        MovementData *movementData = decodeMovement(movementXML, armatureData);
        aniData->addMovement(movementData);
        
        movementXML = movementXML->NextSiblingElement(MOVEMENT);
        
    }
    
    return aniData;
    
}
    
MovementData *DataReaderHelper::decodeMovement(TiXmlElement *movementXML, ArmatureData *armatureData)
{
    const char* _movName = movementXML->Attribute(A_NAME);
    
    MovementData *movementData = MovementData::create();
    
    movementData->setName(_movName);
    
    
    int _duration, _durationTo, _durationTween, _loop, _tweenEasing = 0;
    
    if( movementXML->QueryIntAttribute(A_DURATION, &(_duration)) == TIXML_SUCCESS)
    {
        movementData->setDuration(_duration);
    }
    if( movementXML->QueryIntAttribute(A_DURATION_TO, &(_durationTo)) == TIXML_SUCCESS)
    {
        movementData->setDurationTo(_durationTo);
    }
    if( movementXML->QueryIntAttribute(A_DURATION_TWEEN, &(_durationTween)) == TIXML_SUCCESS)
    {
        movementData->setDurationTween(_durationTween);
    }
    if( movementXML->QueryIntAttribute(A_LOOP, &(_loop)) == TIXML_SUCCESS)
    {
        movementData->setLoop((bool)_loop);
    }
    
    const char * _easing= movementXML->Attribute(A_TWEEN_EASING);
    if(_easing != NULL)
    {
        std::string str = _easing;
        if(str.compare(FL_NAN) != 0)
        {
            if( movementXML->QueryIntAttribute(A_TWEEN_EASING, &(_tweenEasing)) == TIXML_SUCCESS)
            {
                movementData->setTweenEasing((TweenType)_tweenEasing);
            }
        }else{
            movementData->setTweenEasing(TWEEN_EASING_MAX);
        }
    }
    
    TiXmlElement *movBoneXml = movementXML->FirstChildElement(BONE);
    while(movBoneXml)
    {
        const char* _boneName = movBoneXml->Attribute(A_NAME);
        
        if (movementData->getMovementBoneData(_boneName))
        {
            movBoneXml = movBoneXml->NextSiblingElement();
            continue;
        }
        
        
        BoneData *boneData = (BoneData*)armatureData->getBoneData(_boneName);
        
        std::string _parentName = boneData->m_strParent;
        
        
        TiXmlElement *_parentXML = NULL;
        if (_parentName.compare("") != 0)
        {
            _parentXML = movementXML->FirstChildElement(BONE);
            
            while (_parentXML)
            {
                if (_parentName.compare(_parentXML->Attribute(A_NAME)) == 0)
                {
                    break;
                }
                _parentXML = _parentXML->NextSiblingElement(BONE);
            }
        }
        
        MovementBoneData *_moveBoneData = decodeMovementBone(movBoneXml, _parentXML, boneData);
        movementData->addMovementBoneData(NULL, _moveBoneData);
        
        movBoneXml = movBoneXml->NextSiblingElement(BONE);
    }
    
    return movementData;
}


MovementBoneData *DataReaderHelper::decodeMovementBone(TiXmlElement* movBoneXml, TiXmlElement* _parentXml, BoneData *boneData)
{
    MovementBoneData* _movBoneData = MovementBoneData::create();
    float _scale, _delay;
    
    if( movBoneXml )
    {
        if( movBoneXml->QueryFloatAttribute(A_MOVEMENT_SCALE, &_scale) == TIXML_SUCCESS )
        {
            _movBoneData->setScale(_scale);
        }
        if( movBoneXml->QueryFloatAttribute(A_MOVEMENT_DELAY, &_delay) == TIXML_SUCCESS )
        {
            if(_delay > 0)
            {
                _delay -= 1;
            }
            _movBoneData->setDelay(_delay);
        }
    }
    
    int _length = 0;
    int _i = 0;
    int _parentTotalDuration = 0;
    int _currentDuration = 0;
    
    TiXmlElement *parentFrameXML = NULL;
    
    std::vector<TiXmlElement*> _parentXMLList;
    
    /*
     *  get the parent frame xml list, we need get the origin data
     */
    if( _parentXml != NULL )
    {
        parentFrameXML = _parentXml->FirstChildElement(FRAME);
        while (parentFrameXML)
        {
            _parentXMLList.push_back(parentFrameXML);
            parentFrameXML = parentFrameXML->NextSiblingElement(FRAME);
        }
        
        parentFrameXML = NULL;
        
        _length = _parentXMLList.size();
    }
    
    
    int _totalDuration =0;
    
    std::string name = movBoneXml->Attribute(A_NAME);
    
    _movBoneData->setName(name);
    
    TiXmlElement *frameXML= movBoneXml->FirstChildElement(FRAME);
    
    while( frameXML )
    {
        if(_parentXml)
        {
            /*
             *  in this loop we get the corresponding parent frame xml
             */
            while(_i < _length && (parentFrameXML?(_totalDuration < _parentTotalDuration || _totalDuration >= _parentTotalDuration + _currentDuration):true))
            {
                parentFrameXML = _parentXMLList[_i];
                _parentTotalDuration += _currentDuration;
                parentFrameXML->QueryIntAttribute(A_DURATION, &_currentDuration);
                _i++;
                
            }
        }
        
        FrameData * _frameData = decodeFrame( frameXML, parentFrameXML, boneData);
        
        _movBoneData->addFrameData(_frameData);
        
        _totalDuration += _frameData->getDuration();
        
        frameXML = frameXML->NextSiblingElement(FRAME);
    }
    
    
    return _movBoneData;
}

FrameData * DataReaderHelper::decodeFrame(TiXmlElement* frameXML,  TiXmlElement* _parentFrameXml, BoneData *boneData)
{
    float _x, _y, _scale_x, _scale_y, _skew_x, _skew_y = 0;
    int _duration, _displayIndex, _zOrder, _tweenEasing = 0;
    
    FrameData* frameData = FrameData::create();
    
    
    if(frameXML->Attribute(A_MOVEMENT) != NULL)
    {
        frameData->m_strMovement = frameXML->Attribute(A_MOVEMENT);
    }
    if(frameXML->Attribute(A_EVENT) != NULL)
    {
        frameData->m_strEvent = frameXML->Attribute(A_EVENT);
    }
    if(frameXML->Attribute(A_SOUND) != NULL)
    {
        frameData->m_strSound = frameXML->Attribute(A_SOUND);
    }
    if(frameXML->Attribute(A_SOUND_EFFECT) != NULL)
    {
        frameData->m_strSoundEffect = frameXML->Attribute(A_SOUND_EFFECT);
    }
    if(frameXML->QueryFloatAttribute(A_X, &_x) == TIXML_SUCCESS)
    {
        frameData->m_fX = _x;
        frameData->m_fX *= m_fPositionReadScale;
    }
    if(frameXML->QueryFloatAttribute(A_Y, &_y) == TIXML_SUCCESS)
    {
        frameData->m_fY = -_y;
        frameData->m_fX *= m_fPositionReadScale;
    }
    if( frameXML->QueryFloatAttribute(A_SCALE_X, &_scale_x) == TIXML_SUCCESS )
    {
        frameData->m_fScaleX = _scale_x;
    }
    if( frameXML->QueryFloatAttribute(A_SCALE_Y, &_scale_y) == TIXML_SUCCESS )
    {
        frameData->m_fScaleY = _scale_y;
    }
    if( frameXML->QueryFloatAttribute(A_SKEW_X, &_skew_x) == TIXML_SUCCESS )
    {
        frameData->m_fSkewX = CC_DEGREES_TO_RADIANS(_skew_x);
    }
    if( frameXML->QueryFloatAttribute(A_SKEW_Y, &_skew_y) == TIXML_SUCCESS )
    {
        frameData->m_fSkewY = CC_DEGREES_TO_RADIANS(-_skew_y);
    }
    if( frameXML->QueryIntAttribute(A_DURATION, &_duration) == TIXML_SUCCESS )
    {
        frameData->setDuration(_duration);
    }
    if(  frameXML->QueryIntAttribute(A_DISPLAY_INDEX, &_displayIndex) == TIXML_SUCCESS )
    {
        frameData->setDisplayIndex(_displayIndex);
    }
    if(  frameXML->QueryIntAttribute(A_Z, &_zOrder) == TIXML_SUCCESS )
    {
        frameData->m_iZOrder = _zOrder;
    }



	TiXmlElement *colorTransformXML = frameXML->FirstChildElement(A_COLOR_TRANSFORM);
	if (colorTransformXML)
	{
		int alpha, red, green, blue = 100;
		int alphaOffset, redOffset, greenOffset, blueOffset = 0;

		colorTransformXML->QueryIntAttribute(A_ALPHA, &alpha);
		colorTransformXML->QueryIntAttribute(A_RED, &red);
		colorTransformXML->QueryIntAttribute(A_GREEN, &green);
		colorTransformXML->QueryIntAttribute(A_BLUE, &blue) ;

		colorTransformXML->QueryIntAttribute(A_ALPHA_OFFSET, &alphaOffset);
		colorTransformXML->QueryIntAttribute(A_RED_OFFSET, &redOffset);
		colorTransformXML->QueryIntAttribute(A_GREEN_OFFSET, &greenOffset);
		colorTransformXML->QueryIntAttribute(A_BLUE_OFFSET, &blueOffset) ;

		frameData->m_iA = 2.55 * alphaOffset +alpha;
		frameData->m_iR = 2.55 * redOffset + red;
		frameData->m_iG = 2.55 * greenOffset + green;
		frameData->m_iB = 2.55 * blueOffset + blue;

		frameData->m_bUseColorInfo = true;
	}

    
    const char * _easing= frameXML->Attribute(A_TWEEN_EASING);
    if(_easing != NULL)
    {
        std::string str = _easing;
        if(str.compare(FL_NAN) != 0)
        {
            if( frameXML->QueryIntAttribute(A_TWEEN_EASING, &(_tweenEasing)) == TIXML_SUCCESS)
            {
                frameData->setTweenEasing((TweenType)_tweenEasing);
            }
        }else{
            frameData->setTweenEasing(TWEEN_EASING_MAX);
        }
    }
    
    if(_parentFrameXml)
    {
        /*
         *  recalculate frame data from parent frame data, use for translate matrix
         */
        Node _helpNode;
        _parentFrameXml->QueryFloatAttribute(A_X, &_helpNode.m_fX);
        _parentFrameXml->QueryFloatAttribute(A_Y, &_helpNode.m_fY);
        _parentFrameXml->QueryFloatAttribute(A_SKEW_X, &_helpNode.m_fSkewX);
        _parentFrameXml->QueryFloatAttribute(A_SKEW_Y, &_helpNode.m_fSkewY);
        
        _helpNode.m_fY = -_helpNode.m_fY;
        _helpNode.m_fSkewX = CC_DEGREES_TO_RADIANS(_helpNode.m_fSkewX);
        _helpNode.m_fSkewY = CC_DEGREES_TO_RADIANS(-_helpNode.m_fSkewY);
        
        TransformHelp::transformFromParent(*frameData, _helpNode);
    }
    
//    frameData->m_fX -= boneData->m_fX;
//    frameData->m_fY -= boneData->m_fY;
//    frameData->m_fSkewX -= boneData->m_fSkewX;
//    frameData->m_fSkewY -= boneData->m_fSkewY;
    
    
    return frameData;
}

TextureData *DataReaderHelper::decodeTexture(TiXmlElement *textureXML)
{
    TextureData *textureData = TextureData::create();
    
	if( textureXML->Attribute(A_NAME) != NULL)
	{
		textureData->setName(textureXML->Attribute(A_NAME));
	}
    
	float px, py, width, height = 0;

	textureXML->QueryFloatAttribute(A_PIVOT_X, &px);
	textureXML->QueryFloatAttribute(A_PIVOT_Y, &py);
	textureXML->QueryFloatAttribute(A_WIDTH, &width);
	textureXML->QueryFloatAttribute(A_HEIGHT, &height);

	float _anchorPointX = px / width;
	float _anchorPointY = (height - py) / height;

	textureData->setPivotX(_anchorPointX);
	textureData->setPivotY(_anchorPointY);
    
    TiXmlElement *contourXML = textureXML->FirstChildElement(CONTOUR);
    
    while (contourXML) {
        
        ContourData *contourData = decodeContour(contourXML);
        textureData->addContourData(contourData);
        
        contourXML = contourXML->NextSiblingElement(CONTOUR);
    }
    
    
    return textureData;
}

ContourData *DataReaderHelper::decodeContour(TiXmlElement *contourXML)
{
    ContourData *contourData = ContourData::create();
    
    TiXmlElement *vertexDataXML = contourXML->FirstChildElement(CONTOUR_VERTEX);
    
	while (vertexDataXML) {

		ContourVertex2 *_vertex = new ContourVertex2(0, 0);
		_vertex->autorelease();

		vertexDataXML->QueryFloatAttribute(A_X, &_vertex->x);
		vertexDataXML->QueryFloatAttribute(A_Y, &_vertex->y);

		_vertex->y = -_vertex->y;
		contourData->addVertex(_vertex);

		vertexDataXML = vertexDataXML->NextSiblingElement(CONTOUR_VERTEX);
	}
    
    
    return contourData;
    
}
#pragma endregion
    

#pragma region Decode Data From JSON
    
void DataReaderHelper::addDataFromJson(const char *filePath)
{
	unsigned long size;
	std::string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(filePath);
	const char *pFileContent = (char*)CCFileUtils::sharedFileUtils()->getFileData(fullPath.c_str() , "r", &size);

	addDataFromJsonCache(pFileContent);
}


void DataReaderHelper::addDataFromJsonCache(const char *fileContent)
{
	CSJsonDictionary json;
	json.initWithDescription(fileContent);

	// Decode armatures
	int length = json.getArrayItemCount(ARMATURE_DATA);
	for (int i = 0; i<length; i++)
	{
		CSJsonDictionary *armatureDic = json.getSubItemFromArray(ARMATURE_DATA, i);
		ArmatureData *armatureData = decodeArmature(*armatureDic);
		ArmatureDataManager::sharedArmatureDataManager()->addArmatureData(armatureData->getName().c_str(), armatureData);
	}

	// Decode animations
	length = json.getArrayItemCount(ANIMATION_DATA);
	for (int i = 0; i<length; i++)
	{
		CSJsonDictionary *animationDic = json.getSubItemFromArray(ANIMATION_DATA, i);
		AnimationData *animationData = AnimationData::createWithJson(animationDic->getDescription().c_str());
		ArmatureDataManager::sharedArmatureDataManager()->addAnimationData(animationData->getName().c_str(), animationData);
	}

	// Decode textures
	length = json.getArrayItemCount(TEXTURE_DATA);
	for (int i = 0; i<length; i++)
	{
		CSJsonDictionary *textureDic = json.getSubItemFromArray(TEXTURE_DATA, i);
		TextureData *textureData = TextureData::createWithJson(textureDic->getDescription().c_str());
		ArmatureDataManager::sharedArmatureDataManager()->addTextureData(textureData->getName().c_str(), textureData);
	}


}


ArmatureData *DataReaderHelper::decodeArmature(CSJsonDictionary &json)
{
	ArmatureData *armatureData = ArmatureData::create();

	const char * name = json.getItemStringValue(A_NAME);
	if(name != NULL)
	{
		armatureData->name = name;
	}

	int length = json.getArrayItemCount(BONE_DATA);
	for (int i = 0; i<length; i++)
	{
		CSJsonDictionary *dic = json.getSubItemFromArray(BONE_DATA, i);
		armatureData->addBoneData(decodeBone(*dic));
	}
}

BoneData *DataReaderHelper::decodeBone(CSJsonDictionary &json)
{
	BoneData *boneData = BoneData::create();

	const char * str = json.getItemStringValue(A_NAME);
	if(str != NULL)
	{
		boneData->name = str;
	}

	str = json.getItemStringValue(A_PARENT);
	if(str != NULL)
	{
		boneData->parentName = str;
	}

	int length = json.getArrayItemCount(DISPLAY_DATA);

	for (int i = 0; i<length; i++)
	{
		CSJsonDictionary *dic = json.getSubItemFromArray(DISPLAY_DATA, i);
		boneData->addDisplayData(decodeBoneDisplay(*dic));
	}
}

DisplayData *DataReaderHelper::decodeBoneDisplay(CSJsonDictionary &json)
{
	DisplayType displayType = (DisplayType)json.getItemIntValue(A_DISPLAY_TYPE, CS_DISPLAY_SPRITE);

	DisplayData *displayData = NULL;
    
    switch (displayType) {
        case CS_DISPLAY_SPRITE:
            displayData = SpriteDisplayData::create();
            break;
        case CS_DISPLAY_ARMATURE:
            displayData = ArmatureDisplayData::create();
            break;
		case CS_DISPLAY_PARTICLE:
			displayData = ParticleDisplayData::create();
			break;
		case CS_DISPLAY_SHADER:
			displayData = ShaderDisplayData::create();
			break;
        default:
            displayData = SpriteDisplayData::create();
            break;
    }

	displayData->setDisplayTyp

	const char * str = m_JsonDic.getItemStringValue(A_NAME);
        if(str != NULL)
        {
            m_strDisplayName = str;
        }
}

AnimationData *DataReaderHelper::decodeAnimation(CSJsonDictionary &json)
{

}

MovementData *DataReaderHelper::decodeMovement(CSJsonDictionary &json)
{

}

MovementBoneData *DataReaderHelper::decodeMovementBone(CSJsonDictionary &json)
{

}

FrameData *DataReaderHelper::decodeFrame(CSJsonDictionary &json)
{

}

TextureData *DataReaderHelper::decodeTexture(CSJsonDictionary &json)
{

}

ContourData *DataReaderHelper::decodeContour(CSJsonDictionary &json)
{

}



// std::string DataReaderHelper::convertFlashToSP(const char *fileName)
// {
// // 	clearJson();
// // 
// // 	addDataFromXML(fileName);
// // 
// // 	addAllArmatureDataToJsonList();
// // 	addAllAnimationDataToJsonList();
// // 	addAllTextureDataToJsonList();
// // 
// // 	deleteDictElements(ArmatureDataManager::sharedArmatureDataManager()->getArmarureDatas());
// // 	deleteDictElements(ArmatureDataManager::sharedArmatureDataManager()->getAnimationDatas());
// // 	deleteDictElements(ArmatureDataManager::sharedArmatureDataManager()->getTextureDatas());
// // 
// // 	return getExportJson();
// 	return "";
// }
#pragma endregion

}