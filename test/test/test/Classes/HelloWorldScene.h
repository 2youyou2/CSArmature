#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "CSArmature.h"
#include "CSArmatureDataManager.h"
#include "VisibleRect.h"
#include "sigslot.h"
#include "CSBatchNode.h"

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
	virtual void draw();
};

enum {
	TEST_DRAGON_BONES_2_0 = 0,
	TEST_COCOSTUDIO_WITH_SKELETON,
	TEST_COCOSTUDIO_WITHOUT_SKELETON,
	TEST_COCOSTUDIO_WITH_CONVERT_FROM_DRAGON_BONES_2_0,
	TEST_PERFORMANCE,
	TEST_USE_BATCHNODE,
	TEST_CHANGE_ZORDER,
	TEST_ANIMATION_EVENT,
	TEST_USE_DIFFERENT_PICTURE,

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

class TestCSContertFromDragonBone : public TestLayer
{
	virtual void onEnter();
	virtual std::string title();
};

class TestPerformance : public TestLayer
{
public:
	~TestPerformance();

	virtual void onEnter();
	virtual std::string title();
	virtual std::string subtitle();
	virtual void addArmature(Armature *armature);
	void update(float delta);

	int armatureCount;

	int frames;
	float times;

	std::vector<Armature*> armatureList;
};


class TestUseBatchNode : public TestPerformance
{
	virtual void onEnter();
	virtual std::string title();
	virtual void addArmature(Armature *armature);

	BatchNode *batchnode;
};




class TestChangeZorder : public TestLayer
{
	virtual void onEnter();
	virtual std::string title();
	void changeZorder(float dt);

	int currentTag;
};


class TestAnimationEvent : public TestLayer, public sigslot::has_slots<>
{
	virtual void onEnter();
	virtual std::string title();
	void animationEvent(Armature *armature, const char *movementType, const char *movementID);
	void callback1();
	void callback2();

	Armature *armature;
};


class TestUseMutiplePicture : public TestLayer
{
	virtual void onEnter();
	virtual std::string title();
	virtual std::string subtitle();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void registerWithTouchDispatcher();

	int displayIndex;
	Armature *armature;
};
#endif  // __HELLOWORLD_SCENE_H__