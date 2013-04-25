#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "CSArmature.h"
#include "CSArmatureDataManager.h"
#include "VisibleRect.h"

using namespace cs;

//#include "Box2D/Box2D.h"


static const char s_pPathB1[]             = "Images/b1.png";
static const char s_pPathB2[]             = "Images/b2.png";
static const char s_pPathR1[]             = "Images/r1.png";
static const char s_pPathR2[]             = "Images/r2.png";
static const char s_pPathF1[]             = "Images/f1.png";
static const char s_pPathF2[]             = "Images/f2.png";


class TestScene : public CCScene
{
public: 
	TestScene(bool bPortrait = false);
	virtual void onEnter();

	virtual void runThisTest();
};

enum {
	TEST_DRAGON_BONES_2_0 = 0,
	TEST_COCOSTUDIO_WITHOUT_SKELETON,
	TEST_COCOSTUDIO_WITH_SKELETON,
	TEST_PERFORMANCE,
	TEST_CHANGE_ZORDER,

	TEST_LAYER_COUNT
};

class TestLayer : public CCLayer
{
public:
	virtual void onEnter();
	virtual void onExit();

	virtual std::string title();
	virtual std::string subtitle();

	void restartCallback(CCObject* pSender);
	void nextCallback(CCObject* pSender);
	void backCallback(CCObject* pSender);
};

class TestDragonBones20 : public TestLayer
{
public:
	virtual void onEnter();
	virtual std::string title();
};

class TestCSWithSkeleton : public TestLayer
{
	virtual void onEnter();
	virtual std::string title();
};

class TestCSWithoutSkeleton : public TestLayer
{
	virtual void onEnter();
	virtual std::string title();
};

class TestPerformance : public TestLayer
{
	virtual void onEnter();
	virtual std::string title();
};

class TestChangeZorder : public TestLayer
{
	virtual void onEnter();
	virtual std::string title();
	void changeZorder(float dt);

	int currentTag;
};

#endif  // __HELLOWORLD_SCENE_H__