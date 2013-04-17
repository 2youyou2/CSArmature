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

std::vector<std::string> DataReaderHelper::m_arrXMLFileList;
CSJsonDictionary *DataReaderHelper::m_pJson = new CSJsonDictionary();
float DataReaderHelper::m_fPositionReadScale = 1;
    
    
void DataReaderHelper::addDataFromFile(const char *_filePath)
{
    std::string _filePathStr = _filePath;
    size_t startPos = _filePathStr.find_last_of(".");
    std::string _str = &_filePathStr[startPos];
    
    if (_str.compare(".xml") == 0)
    {

        
#if CS_TOOL_PLATFORM
        if(Game::sharedGame()->isUsePackage())
        {
            DataReaderHelper::addDataFromXMLPak(_filePathStr.c_str());
        }
        else
        {
            DataReaderHelper::addDataFromXML(_filePathStr.c_str());
        }
#else
        DataReaderHelper::addDataFromXML(_filePathStr.c_str());
#endif
    }
    else if(_str.compare(".json") == 0)
    {
        DataReaderHelper::addDataFromJson(_filePathStr.c_str());
    }
}
    
void DataReaderHelper::setPositionReadScale(float scale)
{
    m_fPositionReadScale = scale;
}

float DataReaderHelper::getPositionReadScale()
{
    return m_fPositionReadScale;
}
    
    
void DataReaderHelper::addDataFromXML(const char* _xml)
{
    /*
     *	check if xml is already added to ArmatureDataManager, if then return.
     */
    for(int _i = 0; _i<m_arrXMLFileList.size(); _i++)
    {
        if (m_arrXMLFileList[_i].compare(_xml) == 0)
        {
            return;
        }
    }
    
    
    m_arrXMLFileList.push_back(_xml);
    
    /*
     *  Need to get the full path of the xml file, or the Tiny XML can't find the xml at IOS
     */
    std::string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(_xml);
    
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
    
void DataReaderHelper::addDataFromXMLPak(const char *_xml)
{
#if CS_TOOL_PLATFORM
    /*
     *	check if xml is already added to ArmatureDataManager, if then return.
     */
    for(int _i = 0; _i<m_arrXMLFileList.size(); _i++)
    {
        if (m_arrXMLFileList[_i].compare(_xml) == 0)
        {
            return;
        }
    }
    
    
    m_arrXMLFileList.push_back(_xml);
    
    char *_pFileContent = NULL;
    JsonReader::getFileBuffer(_xml, &_pFileContent);
    
    if (_pFileContent)
    {
        addDataFromCache(_pFileContent);
    }
#endif
}

void DataReaderHelper::addDataFromCache(const char *_pFileContent)
{
    TiXmlDocument	_document;
    _document.Parse(_pFileContent, 0, TIXML_ENCODING_UTF8);
    
    TiXmlElement	*_root = _document.RootElement();
    CCAssert(_root, "XML error  or  XML is empty.");
    
    /*
     *  begin decode armature data from xml
     */
    TiXmlElement *_armaturesXML = _root->FirstChildElement(ARMATURES);
    TiXmlElement *_armatureXML = _armaturesXML->FirstChildElement(ARMATURE);
    while(_armatureXML)
    {
        ArmatureData *_armatureData = DataReaderHelper::decodeArmature(_armatureXML);
        ArmatureDataManager::sharedArmatureDataManager()->addArmatureData(_armatureData->getName().c_str(), _armatureData);
        
        //saveArmatureDataToJson("armature.json", _armatureData);
        
        _armatureXML = _armatureXML->NextSiblingElement(ARMATURE);
    }
    
    /*
     *  begin decode animation data from xml
     */
    TiXmlElement *_animationsXML = _root->FirstChildElement(ANIMATIONS);
    TiXmlElement *_animationXML = _animationsXML->FirstChildElement(ANIMATION);
    while(_animationXML)
    {
        AnimationData *_animationData = DataReaderHelper::decodeAnimation(_animationXML);
        ArmatureDataManager::sharedArmatureDataManager()->addAnimationData(_animationData->getName().c_str(), _animationData);
        
        //saveAnimationDataToJson("armature.json", _animationData);
        
        _animationXML = _animationXML->NextSiblingElement(ANIMATION);
    }
    
    /*
     *  begin decode texture data from xml
     */
    TiXmlElement *_texturesXML = _root->FirstChildElement(TEXTURE_ATLAS);
    TiXmlElement *_textureXML = _texturesXML->FirstChildElement(SUB_TEXTURE);
    while(_textureXML)
    {
        TextureData *_textureData = DataReaderHelper::decodeTexture(_textureXML);
        ArmatureDataManager::sharedArmatureDataManager()->addTextureData(_textureData->getName().c_str(), _textureData);
        
        //saveTextureDataToJson("armature.json", _textureData);
        
        _textureXML = _textureXML->NextSiblingElement(SUB_TEXTURE);
    }
    
    
    _document.Clear();
    
}

void DataReaderHelper::addTextureDataFromXML(const char *_xmlPath)
{
    /*
     *	check if xml is already added to ArmatureDataManager, if then return.
     */
    for(int _i = 0; _i<m_arrXMLFileList.size(); _i++)
    {
        if (m_arrXMLFileList[_i].compare(_xmlPath) == 0)
        {
            return;
        }
    }
    
    /*
     *  Need to get the full path of the xml file, or the Tiny XML can't find the xml at IOS
     */
	std::string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(_xmlPath);
    
    /*
     *  Need to read the tiny xml into memory first, or the Tiny XML can't find the xml at IOS
     */
    unsigned long	_size;
    const char *_pFileContent = (char*)CCFileUtils::sharedFileUtils()->getFileData(fullPath.c_str(), "r", &_size);
    
    
    TiXmlDocument	_document;
    _document.Parse(_pFileContent, 0, TIXML_ENCODING_UTF8);
    
    TiXmlElement	*_root = _document.RootElement();
    CCAssert(_root, "XML error  or  XML is empty.");
    
    /*
     *  begin decode texture data from xml
     */
    TiXmlElement *_texturesXML = _root->FirstChildElement(TEXTURE_ATLAS);
    TiXmlElement *_textureXML = _texturesXML->FirstChildElement(SUB_TEXTURE);
    while(_textureXML)
    {
        TextureData *_textureData = DataReaderHelper::decodeTexture(_textureXML);
        ArmatureDataManager::sharedArmatureDataManager()->addTextureData(_textureData->getName().c_str(), _textureData);
        
        //saveTextureDataToJson("armature.json", _textureData);
        
        _textureXML = _textureXML->NextSiblingElement(SUB_TEXTURE);
    }
    
    
    _document.Clear();
    
    m_arrXMLFileList.push_back(_xmlPath);
}

    
ArmatureData *DataReaderHelper::decodeArmature(TiXmlElement *_armatureXML)
{
    const char*	_name = _armatureXML->Attribute(A_NAME);
    
    
    ArmatureData *_armatureData = ArmatureData::create();
    _armatureData->setName(_name);
    //m_pArmarureDatas->setObject(_armatureData, _name);
    
    
    TiXmlElement* _boneXML = _armatureXML->FirstChildElement(BONE);
    
    while( _boneXML )
    {
        /*
         *  If this bone have parent, then get the parent bone xml
         */
        const char *_parentName = _boneXML->Attribute(A_PARENT);
        TiXmlElement *_parentXML = NULL;
        if (_parentName)
        {
            _parentXML = _armatureXML->FirstChildElement(BONE);
            std::string _name = _parentName;
            while (_parentXML)
            {
                if (_name.compare(_parentXML->Attribute(A_NAME)) == 0)
                {
                    break;
                }
                _parentXML = _parentXML->NextSiblingElement(BONE);
            }
        }
        
        BoneData *_boneData = decodeBone(_boneXML, _parentXML);
        _armatureData->addBoneData(_boneData);
        
        _boneXML = _boneXML->NextSiblingElement(BONE);
    }
  
    return _armatureData;
}



BoneData *DataReaderHelper::decodeBone(TiXmlElement *_boneXML, TiXmlElement *_parentXML)
{
    
    std::string _name = _boneXML->Attribute(A_NAME);
    
    CCAssert(_name.compare("") != 0, "");

    
    BoneData *_boneData = BoneData::create();
    
    _boneData->m_strName = _name;
    
    if( _boneXML->Attribute(A_PARENT) != NULL )
    {
        _boneData->m_strParent = _boneXML->Attribute(A_PARENT);
    }
    
//     _boneXML->QueryFloatAttribute(A_X,		&(_boneData->m_fX));
//     _boneXML->QueryFloatAttribute(A_Y,		&(_boneData->m_fY));
//     _boneXML->QueryFloatAttribute(A_SKEW_X, &(_boneData->m_fSkewX));
//     _boneXML->QueryFloatAttribute(A_SKEW_Y, &(_boneData->m_fSkewY));
//     _boneXML->QueryFloatAttribute(A_SCALE_X,&(_boneData->m_fScaleX));
//     _boneXML->QueryFloatAttribute(A_SCALE_Y,&(_boneData->m_fScaleY));
//     _boneXML->QueryIntAttribute(A_Z,		&(_boneData->m_iZOrder));
//     
//     _boneXML->QueryIntAttribute(A_ALPHA,	&(_boneData->m_iAlpha));
//     _boneXML->QueryIntAttribute(A_RED,		&(_boneData->m_iRed));
//     _boneXML->QueryIntAttribute(A_GREEN,	&(_boneData->m_iGreen));
//     _boneXML->QueryIntAttribute(A_BLUE,		&(_boneData->m_iBlue));
//     
//     // flash export data is a percent value, so wo change it
//     _boneData->m_iAlpha *= 2.55;
//     _boneData->m_iRed *= 2.55;
//     _boneData->m_iGreen *= 2.55;
//     _boneData->m_iBlue *= 2.55;
//     
//     /*
//      *  Note : Flash's Y value contrary to Cocos2dX's Y value
//      */
//     _boneData->m_fY = -_boneData->m_fY;
//     
//     /*
//      *  Note : Flash export datas are DEGREE values, and our matrix use RADIAN values
//      */
//     _boneData->m_fSkewX = CC_DEGREES_TO_RADIANS(_boneData->m_fSkewX);
//     _boneData->m_fSkewY = CC_DEGREES_TO_RADIANS(-_boneData->m_fSkewY);
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
//         TransformHelp::transformFromParent(*_boneData, _helpNode);
//     }
    
    
    TiXmlElement *_displayXML = _boneXML->FirstChildElement(DISPLAY);
    while(_displayXML)
    {
        DisplayData *_displayData = decodeBoneDisplay(_displayXML);
        _boneData->addDisplayData(_displayData);
        
        _displayXML = _displayXML->NextSiblingElement(DISPLAY);
    }
    
    
    return _boneData;
    
}

DisplayData *DataReaderHelper::decodeBoneDisplay(TiXmlElement *_displayXML)
{
    int _isArmature = 0;
    
    DisplayData *_displayData;
    
    
    if( _displayXML->QueryIntAttribute(A_IS_ARMATURE, &(_isArmature)) == TIXML_SUCCESS )
    {
        if(!_isArmature)
        {
            _displayData = SpriteDisplayData::create();
            _displayData->setDisplayType(CS_DISPLAY_SPRITE);
        }
        else
        {
            _displayData = ArmatureDisplayData::create();
            _displayData->setDisplayType(CS_DISPLAY_ARMATURE);
        }
        
    }else{
        _displayData = SpriteDisplayData::create();
        _displayData->setDisplayType(CS_DISPLAY_SPRITE);
    }
    
    if(_displayXML->Attribute(A_NAME) != NULL )
    {
        if(!_isArmature)
        {
            ((SpriteDisplayData*)_displayData)->setDisplayName(_displayXML->Attribute(A_NAME));
        }
        else
        {
            ((ArmatureDisplayData*)_displayData)->setDisplayName(_displayXML->Attribute(A_NAME));
        }
        
    }
    
    
    return _displayData;
}
    
AnimationData *DataReaderHelper::decodeAnimation(TiXmlElement *_animationXML)
{
    const char*	_name = _animationXML->Attribute(A_NAME);
    
//    AnimationData *_aniData = (AnimationData*)m_pAnimationDatas->objectForKey(_name);
//    if (_aniData)
//    {
//        //CCLOG("AnimationData (%s) is already exit", _name);
//        return;
//    }
    
    AnimationData *_aniData =  AnimationData::create();
    
    ArmatureData *_armatureData = ArmatureDataManager::sharedArmatureDataManager()->getArmatureData(_name);
    
    _aniData->setName(_name);
    
    TiXmlElement* _movementXML = _animationXML->FirstChildElement(MOVEMENT);
    
    while( _movementXML )
    {
        MovementData *_movementData = decodeMovement(_movementXML, _armatureData);
        _aniData->addMovement(_movementData);
        
        _movementXML = _movementXML->NextSiblingElement(MOVEMENT);
        
    }
    
    return _aniData;
    
}
    
MovementData *DataReaderHelper::decodeMovement(TiXmlElement *_movementXML, ArmatureData *_armatureData)
{
    const char* _movName = _movementXML->Attribute(A_NAME);
    
    MovementData *_movementData = MovementData::create();
    
    _movementData->setName(_movName);
    
    
    int _duration, _durationTo, _durationTween, _loop, _tweenEasing = 0;
    
    if( _movementXML->QueryIntAttribute(A_DURATION, &(_duration)) == TIXML_SUCCESS)
    {
        _movementData->setDuration(_duration);
    }
    if( _movementXML->QueryIntAttribute(A_DURATION_TO, &(_durationTo)) == TIXML_SUCCESS)
    {
        _movementData->setDurationTo(_durationTo);
    }
    if( _movementXML->QueryIntAttribute(A_DURATION_TWEEN, &(_durationTween)) == TIXML_SUCCESS)
    {
        _movementData->setDurationTween(_durationTween);
    }
    if( _movementXML->QueryIntAttribute(A_LOOP, &(_loop)) == TIXML_SUCCESS)
    {
        _movementData->setLoop((bool)_loop);
    }
    
    const char * _easing= _movementXML->Attribute(A_TWEEN_EASING);
    if(_easing != NULL)
    {
        std::string str = _easing;
        if(str.compare(FL_NAN) != 0)
        {
            if( _movementXML->QueryIntAttribute(A_TWEEN_EASING, &(_tweenEasing)) == TIXML_SUCCESS)
            {
                _movementData->setTweenEasing((TweenType)_tweenEasing);
            }
        }else{
            _movementData->setTweenEasing(TWEEN_EASING_MAX);
        }
    }
    
    TiXmlElement *_movBoneXml = _movementXML->FirstChildElement(BONE);
    while(_movBoneXml)
    {
        const char* _boneName = _movBoneXml->Attribute(A_NAME);
        
        if (_movementData->getMovementBoneData(_boneName))
        {
            _movBoneXml = _movBoneXml->NextSiblingElement();
            continue;
        }
        
        
        BoneData *_boneData = (BoneData*)_armatureData->getBoneData(_boneName);
        
        std::string _parentName = _boneData->m_strParent;
        
        
        TiXmlElement *_parentXML = NULL;
        if (_parentName.compare("") != 0)
        {
            _parentXML = _movementXML->FirstChildElement(BONE);
            
            while (_parentXML)
            {
                if (_parentName.compare(_parentXML->Attribute(A_NAME)) == 0)
                {
                    break;
                }
                _parentXML = _parentXML->NextSiblingElement(BONE);
            }
        }
        
        MovementBoneData *_moveBoneData = decodeMovementBone(_movBoneXml, _parentXML, _boneData);
        _movementData->addMovementBoneData(NULL, _moveBoneData);
        
        _movBoneXml = _movBoneXml->NextSiblingElement(BONE);
    }
    
    return _movementData;
}


MovementBoneData *DataReaderHelper::decodeMovementBone(TiXmlElement* _movBoneXml, TiXmlElement* _parentXml, BoneData *_boneData)
{
    MovementBoneData* _movBoneData = MovementBoneData::create();
    float _scale, _delay;
    
    if( _movBoneXml )
    {
        if( _movBoneXml->QueryFloatAttribute(A_MOVEMENT_SCALE, &_scale) == TIXML_SUCCESS )
        {
            _movBoneData->setScale(_scale);
        }
        if( _movBoneXml->QueryFloatAttribute(A_MOVEMENT_DELAY, &_delay) == TIXML_SUCCESS )
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
    
    TiXmlElement *_parentFrameXML = NULL;
    
    std::vector<TiXmlElement*> _parentXMLList;
    
    /*
     *  get the parent frame xml list, we need get the origin data
     */
    if( _parentXml != NULL )
    {
        _parentFrameXML = _parentXml->FirstChildElement(FRAME);
        while (_parentFrameXML)
        {
            _parentXMLList.push_back(_parentFrameXML);
            _parentFrameXML = _parentFrameXML->NextSiblingElement(FRAME);
        }
        
        _parentFrameXML = NULL;
        
        _length = _parentXMLList.size();
    }
    
    
    int _totalDuration =0;
    
    std::string name = _movBoneXml->Attribute(A_NAME);
    
    _movBoneData->setName(name);
    
    TiXmlElement *frameXML= _movBoneXml->FirstChildElement(FRAME);
    
    while( frameXML )
    {
        if(_parentXml)
        {
            /*
             *  in this loop we get the corresponding parent frame xml
             */
            while(_i < _length && (_parentFrameXML?(_totalDuration < _parentTotalDuration || _totalDuration >= _parentTotalDuration + _currentDuration):true))
            {
                _parentFrameXML = _parentXMLList[_i];
                _parentTotalDuration += _currentDuration;
                _parentFrameXML->QueryIntAttribute(A_DURATION, &_currentDuration);
                _i++;
                
            }
        }
        
        FrameData * _frameData = decodeFrame( frameXML, _parentFrameXML, _boneData);
        
        _movBoneData->addFrameData(_frameData);
        
        _totalDuration += _frameData->getDuration();
        
        frameXML = frameXML->NextSiblingElement(FRAME);
    }
    
    
    return _movBoneData;
}

FrameData * DataReaderHelper::decodeFrame(TiXmlElement* frameXML,  TiXmlElement* _parentFrameXml, BoneData *_boneData)
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

		frameData->m_cColor.a = 2.55 * alphaOffset +alpha;
		frameData->m_cColor.r = 2.55 * redOffset + red;
		frameData->m_cColor.g = 2.55 * greenOffset + green;
		frameData->m_cColor.b = 2.55 * blueOffset + blue;

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
    
//    frameData->m_fX -= _boneData->m_fX;
//    frameData->m_fY -= _boneData->m_fY;
//    frameData->m_fSkewX -= _boneData->m_fSkewX;
//    frameData->m_fSkewY -= _boneData->m_fSkewY;
    
    
    return frameData;
}

TextureData *DataReaderHelper::decodeTexture(TiXmlElement *_textureXML)
{
    TextureData *_textureData = TextureData::create();
    
	if( _textureXML->Attribute(A_NAME) != NULL)
	{
		_textureData->setName(_textureXML->Attribute(A_NAME));
	}
    
	float px, py, width, height = 0;

	_textureXML->QueryFloatAttribute(A_PIVOT_X, &px);
	_textureXML->QueryFloatAttribute(A_PIVOT_Y, &py);
	_textureXML->QueryFloatAttribute(A_WIDTH, &width);
	_textureXML->QueryFloatAttribute(A_HEIGHT, &height);

	float _anchorPointX = px / width;
	float _anchorPointY = (height - py) / height;

	_textureData->setPivotX(_anchorPointX);
	_textureData->setPivotY(_anchorPointY);
    
    TiXmlElement *_contourXML = _textureXML->FirstChildElement(CONTOUR);
    
    while (_contourXML) {
        
        ContourData *_contourData = decodeContour(_contourXML);
        _textureData->addContourData(_contourData);
        
        _contourXML = _contourXML->NextSiblingElement(CONTOUR);
    }
    
    
    return _textureData;
}

ContourData *DataReaderHelper::decodeContour(TiXmlElement *_contourXML)
{
    ContourData *_contourData = ContourData::create();
    
    TiXmlElement *_vertexDataXML = _contourXML->FirstChildElement(CONTOUR_VERTEX);
    
	while (_vertexDataXML) {

		ContourVertex2 *_vertex = new ContourVertex2(0, 0);
		_vertex->autorelease();

		_vertexDataXML->QueryFloatAttribute(A_X, &_vertex->x);
		_vertexDataXML->QueryFloatAttribute(A_Y, &_vertex->y);

		_vertex->y = -_vertex->y;
		_contourData->addVertex(_vertex);

		_vertexDataXML = _vertexDataXML->NextSiblingElement(CONTOUR_VERTEX);
	}
    
    
    return _contourData;
    
}
    
void DataReaderHelper::readConfigJson(const char* _filePath)
{
	std::string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(_filePath);
    
    /*
     *  Need to read the tiny xml into memory first, or the Tiny XML can't find the xml at IOS
     */
    unsigned long	_size;
    const char *_pFileContent = (char*)CCFileUtils::sharedFileUtils()->getFileData(fullPath.c_str() , "r", &_size);
	
	m_pJson->initWithDescription(_pFileContent);

}
    
void DataReaderHelper::saveConfigToJson(const char *_filePath, const char *_json, int _length)
{
#if CS_TOOL_PLATFORM
    File _file;

//  _file.writeNormally(_filePath, _json);
    _file.write(_filePath, _json);
#endif
}
    
void DataReaderHelper::addDataFromJson(const char *_filePath)
{
    readConfigJson(_filePath);
    
    addArmatureDataFromJson(_filePath);
    addAnimationDataFromJson(_filePath);
    addTextureDataFromJson(_filePath);
}

void DataReaderHelper::addArmatureDataFromJson(const char *_filePath)
{
    
    int _length = m_pJson->getArrayItemCount(ARMATURE_DATA);
    
    for (int i = 0; i<_length; i++)
    {
        CSJsonDictionary *_armatureDic = m_pJson->getSubItemFromArray(ARMATURE_DATA, i);
        
        ArmatureData *_armatureData = ArmatureData::createWithJson(_armatureDic->getDescription().c_str());
        
        ArmatureDataManager::sharedArmatureDataManager()->addArmatureData(_armatureData->getName().c_str(), _armatureData);
    }
    
}
    
void DataReaderHelper::addAnimationDataFromJson(const char *_filePath)
{
    
    int _length = m_pJson->getArrayItemCount(ANIMATION_DATA);
    
    for (int i = 0; i<_length; i++)
    {
        CSJsonDictionary *_animationDic = m_pJson->getSubItemFromArray(ANIMATION_DATA, i);
        
        AnimationData *_animationData = AnimationData::createWithJson(_animationDic->getDescription().c_str());
        
        ArmatureDataManager::sharedArmatureDataManager()->addAnimationData(_animationData->getName().c_str(), _animationData);
    }
    
}
    
    
void DataReaderHelper::addTextureDataFromJson(const char *_filePath)
{
    
    int _length = m_pJson->getArrayItemCount(TEXTURE_DATA);
    
    for (int i = 0; i<_length; i++)
    {
        CSJsonDictionary *_textureDic = m_pJson->getSubItemFromArray(TEXTURE_DATA, i);
        
        TextureData *_textureData = TextureData::createWithJson(_textureDic->getDescription().c_str());
        
		ArmatureDataManager::sharedArmatureDataManager()->addTextureData(_textureData->getName().c_str(), _textureData);
    }
    
}



bool DataReaderHelper::saveArmatureDataToJson(const char *_filePath, ArmatureData *_armatureData)
{
	readConfigJson(_filePath);

	int _length = m_pJson->getArrayItemCount(ARMATURE_DATA);

	CSJsonDictionary _dic;
	_dic.initWithDescription(m_pJson->getDescription().c_str());
	_dic.deleteItem(ARMATURE_DATA);

	for (int i = 0; i<_length; i++)
	{
		CSJsonDictionary *_armatureDic = m_pJson->getSubItemFromArray(ARMATURE_DATA, i);


		if (_armatureData->getName().compare(_armatureDic->getItemStringValue(A_NAME)) != 0)
		{
			_dic.insertItemToArray(ARMATURE_DATA, _armatureDic);
		}
	}

	_dic.insertItemToArray(ARMATURE_DATA, &_armatureData->getJsonDic());

	m_pJson->initWithDescription(_dic.getDescription().c_str());

	saveConfigToJson(_filePath, m_pJson->getDescription().c_str(), m_pJson->getDescription().size());

	return true;
}

bool DataReaderHelper::saveAnimationDataToJson(const char *_filePath, AnimationData *_animationData)
{
	readConfigJson(_filePath);

	int _length = m_pJson->getArrayItemCount(ANIMATION_DATA);

	CSJsonDictionary _dic;
	_dic.initWithDescription(m_pJson->getDescription().c_str());
	_dic.deleteItem(ANIMATION_DATA);

	for (int i = 0; i<_length; i++)
	{
		CSJsonDictionary *_animationDic = m_pJson->getSubItemFromArray(ANIMATION_DATA, i);


		if (_animationData->getName().compare(_animationDic->getItemStringValue(A_NAME)) != 0)
		{
			_dic.insertItemToArray(ANIMATION_DATA, _animationDic);
		}
	}

	_dic.insertItemToArray(ANIMATION_DATA, &_animationData->getJsonDic());

	m_pJson->initWithDescription(_dic.getDescription().c_str());

	saveConfigToJson(_filePath, m_pJson->getDescription().c_str(), m_pJson->getDescription().size());

	return true;
}

bool DataReaderHelper::saveTextureDataToJson(const char *_filePath, TextureData *_textureData)
{
	readConfigJson(_filePath);

	int _length = m_pJson->getArrayItemCount(TEXTURE_DATA);

	CSJsonDictionary _dic;
	_dic.initWithDescription(m_pJson->getDescription().c_str());
	_dic.deleteItem(TEXTURE_DATA);

	for (int i = 0; i<_length; i++)
	{
		CSJsonDictionary *_textureDic = m_pJson->getSubItemFromArray(TEXTURE_DATA, i);


		if (_textureData->getName().compare(_textureDic->getItemStringValue(A_NAME)) != 0)
		{
			_dic.insertItemToArray(TEXTURE_DATA, _textureDic);
		}
	}

	_dic.insertItemToArray(TEXTURE_DATA, &_textureData->getJsonDic());

	m_pJson->initWithDescription(_dic.getDescription().c_str());
	saveConfigToJson(_filePath, m_pJson->getDescription().c_str(), m_pJson->getDescription().size());

	return true;
}

void DataReaderHelper::clearJson()
{
	m_pJson->initWithDescription("");
}

void DataReaderHelper::addArmatureDataToJsonList(ArmatureData *_armatureData)
{
	m_pJson->insertItemToArray(ARMATURE_DATA, &_armatureData->getJsonDic());
}

void DataReaderHelper::addAnimationDataToJsonList(AnimationData *_animationData)
{
	m_pJson->insertItemToArray(ANIMATION_DATA, &_animationData->getJsonDic());
}

void DataReaderHelper::addTextureDataToJsonList(TextureData *_textureData)
{
	m_pJson->insertItemToArray(TEXTURE_DATA, &_textureData->getJsonDic());
}


void DataReaderHelper::addAllTextureDataToJsonList()
{
	CCDictElement *element = NULL;
	CCDictionary *textures = ArmatureDataManager::sharedArmatureDataManager()->getTextureDatas();
	CCDICT_FOREACH(textures, element)
	{
		addTextureDataToJsonList((TextureData*)element->getObject());
	}

}

void DataReaderHelper::addAllArmatureDataToJsonList()
{
	CCDictElement *element = NULL;
	CCDictionary *armatures = ArmatureDataManager::sharedArmatureDataManager()->getArmarureDatas();
	CCDICT_FOREACH(armatures, element)
	{
		addArmatureDataToJsonList((ArmatureData*)element->getObject());
	}
}

void DataReaderHelper::addAllAnimationDataToJsonList()
{
	CCDictElement *element = NULL;
	CCDictionary *animations = ArmatureDataManager::sharedArmatureDataManager()->getAnimationDatas();
	CCDICT_FOREACH(animations, element)
	{
		addAnimationDataToJsonList((AnimationData*)element->getObject());
	}
}

std::string DataReaderHelper::getExportJson()
{
	return m_pJson->getDescription();
}


void deleteDictElements(CCDictionary *dict)
{
	CCDictElement *element = NULL;
	CCDICT_FOREACH(dict, element)
	{
		CCObject *object = element->getObject();
		int reference = object->retainCount();
		dict->removeObjectForElememt(element);
		if(reference > 1)
			CC_SAFE_DELETE(object);
	}
}

std::string DataReaderHelper::convertFlashToSP(const char *fileName)
{
	clearJson();

	addDataFromXML(fileName);

	addAllArmatureDataToJsonList();
	addAllAnimationDataToJsonList();
	addAllTextureDataToJsonList();

	deleteDictElements(ArmatureDataManager::sharedArmatureDataManager()->getArmarureDatas());
	deleteDictElements(ArmatureDataManager::sharedArmatureDataManager()->getAnimationDatas());
	deleteDictElements(ArmatureDataManager::sharedArmatureDataManager()->getTextureDatas());

	return getExportJson();
}
}