/*
* Copyright (c) 2012 Chukong Technologies, Inc.
*
* http://www.sweetpome.com
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

#include "CSSpriteFrameCacheHelper.h"

#if CS_TOOL_PLATFORM
#include "CSContentJsonReader.h"
#endif

using namespace cocos2d;

namespace cs {

	SpriteFrameCacheHelper * SpriteFrameCacheHelper::s_SpriteFrameCacheHelper = NULL;

	SpriteFrameCacheHelper *SpriteFrameCacheHelper::sharedSpriteFrameCacheHelper()
	{
		if(!s_SpriteFrameCacheHelper)
		{
			s_SpriteFrameCacheHelper = new SpriteFrameCacheHelper();
		}

		return s_SpriteFrameCacheHelper;
	}


	void SpriteFrameCacheHelper::addSpriteFrameFromFile(const char *_plistPath, const char *_imagePath)
	{

		std::string path = CCFileUtils::sharedFileUtils()->fullPathForFilename(_plistPath);
		CCDictionary *dict = CCDictionary::createWithContentsOfFileThreadSafe(path.c_str());


		CCTexture2D *pobTexture = CCTextureCache::sharedTextureCache()->addImage(_imagePath);

		addSpriteFrameFromDict(dict, pobTexture, _imagePath);

		dict->release();

	}

	void SpriteFrameCacheHelper::addSpriteFrameFromPak(const char *_plistPath, const char *_imagePath)
	{
#if CS_TOOL_PLATFORM
		//! create CCDictionary from pak
		char *_pFileContent = NULL;
		int size = JsonReader::getFileBuffer(_plistPath, &_pFileContent);

		CSDictMaker dictMaker;
		CCDictionary *dict = dictMaker.dictionaryWithContentsOfCache(_pFileContent, size);


		//! create CCTexture2D from pak
		size = JsonReader::getFileBuffer(_imagePath, &_pFileContent);

		CCImage *img = new CCImage();
		img->initWithImageData(_pFileContent, size);

		CCTexture2D *pobTexture = CCTextureCache::sharedTextureCache()->addUIImage(img, _imagePath);

		addSpriteFrameFromDict(dict, pobTexture, _imagePath);
#endif

	}

	void SpriteFrameCacheHelper::addSpriteFrameFromDict(CCDictionary* dictionary, CCTexture2D *pobTexture, const char *_imagePath)
	{

		/*
		Supported Zwoptex Formats:

		ZWTCoordinatesFormatOptionXMLLegacy = 0, // Flash Version
		ZWTCoordinatesFormatOptionXML1_0 = 1, // Desktop Version 0.0 - 0.4b
		ZWTCoordinatesFormatOptionXML1_1 = 2, // Desktop Version 1.0.0 - 1.0.1
		ZWTCoordinatesFormatOptionXML1_2 = 3, // Desktop Version 1.0.2+
		*/

		CCDictionary *metadataDict = (CCDictionary*)dictionary->objectForKey("metadata");
		CCDictionary *framesDict = (CCDictionary*)dictionary->objectForKey("frames");
		int format = 0;

		// get the format
		if(metadataDict != NULL)
		{
			format = metadataDict->valueForKey("format")->intValue();
		}

		// check the format
		CCAssert(format >=0 && format <= 3, "format is not supported for CCSpriteFrameCache addSpriteFramesWithDictionary:textureFilename:");

		CCDictElement* pElement = NULL;
		CCDICT_FOREACH(framesDict, pElement)
		{
			CCDictionary* frameDict = (CCDictionary*)pElement->getObject();
			std::string spriteFrameName = pElement->getStrKey();

			m_Display2ImageMap[spriteFrameName] = _imagePath;

			//CCLog("spriteFrameName : %s,    imagePath : %s", spriteFrameName.c_str(), _imagePath);

			CCSpriteFrame* spriteFrame = (CCSpriteFrame*)CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(spriteFrameName.c_str());
			if (spriteFrame)
			{
				continue;
			}

			if(format == 0)
			{
				float x = frameDict->valueForKey("x")->floatValue();
				float y = frameDict->valueForKey("y")->floatValue();
				float w = frameDict->valueForKey("width")->floatValue();
				float h = frameDict->valueForKey("height")->floatValue();
				float ox = frameDict->valueForKey("offsetX")->floatValue();
				float oy = frameDict->valueForKey("offsetY")->floatValue();
				int ow = frameDict->valueForKey("originalWidth")->intValue();
				int oh = frameDict->valueForKey("originalHeight")->intValue();
				// check ow/oh
				if(!ow || !oh)
				{
					CCLOG("cocos2d: WARNING: originalWidth/Height not found on the CCSpriteFrame. AnchorPoint won't work as expected. Regenrate the .plist");
				}
				// abs ow/oh
				ow = abs(ow);
				oh = abs(oh);
				// create frame
				spriteFrame = new CCSpriteFrame();
				spriteFrame->initWithTexture(pobTexture,
					CCRectMake(x, y, w, h),
					false,
					CCPointMake(ox, oy),
					CCSizeMake((float)ow, (float)oh)
					);
			}
			else if(format == 1 || format == 2)
			{
				CCRect frame = CCRectFromString(frameDict->valueForKey("frame")->getCString());
				bool rotated = false;

				// rotation
				if (format == 2)
				{
					rotated = frameDict->valueForKey("rotated")->boolValue();
				}

				CCPoint offset = CCPointFromString(frameDict->valueForKey("offset")->getCString());
				CCSize sourceSize = CCSizeFromString(frameDict->valueForKey("sourceSize")->getCString());

				// create frame
				spriteFrame = new CCSpriteFrame();
				spriteFrame->initWithTexture(pobTexture,
					frame,
					rotated,
					offset,
					sourceSize
					);
			}
			else if (format == 3)
			{

			}

			// add sprite frame
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(spriteFrame, spriteFrameName.c_str());
			spriteFrame->release();
		}
	}

	const char *SpriteFrameCacheHelper::getDisplayImagePath(const char *_displayName)
	{
		return m_Display2ImageMap[_displayName].c_str();
	}


	CCTextureAtlas *SpriteFrameCacheHelper::getTextureAtlas(const char *displayName)
	{
		const char *textureName = getDisplayImagePath(displayName); 
		CCTextureAtlas *atlas = (CCTextureAtlas*)m_pDisplay2TextureAtlas->objectForKey(textureName);
		if (atlas == NULL)
		{
			atlas = CCTextureAtlas::createWithTexture(CCTextureCache::sharedTextureCache()->addImage(textureName), 4);
			m_pDisplay2TextureAtlas->setObject(atlas, textureName);
		}

		return atlas;
	}

	SpriteFrameCacheHelper::SpriteFrameCacheHelper()
	{
		m_pDisplay2TextureAtlas = new CCDictionary();
	}
}
