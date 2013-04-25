#include "HelloWorldScene.h"

using namespace cocos2d;


CCLayer* NextTest();
CCLayer* BackTest();
CCLayer* RestartTest();

static int s_nActionIdx = -1;


CCLayer *CreateLayer(int index)
{
	CCLayer *pLayer = NULL;
	switch(index)
	{
	case TEST_DRAGON_BONES_2_0:
		pLayer = new TestDragonBones20();
		break;
	case TEST_COCOSTUDIO_WITH_SKELETON:
		pLayer = new TestCSWithSkeleton();
		break;
	case TEST_COCOSTUDIO_WITHOUT_SKELETON:
		pLayer = new TestCSWithoutSkeleton();
		break;
	case TEST_PERFORMANCE:
		pLayer = new TestPerformance();
		break;
	case TEST_CHANGE_ZORDER:
		pLayer = new TestChangeZorder();
		break;
	default:
		break;
	}

	return pLayer;
}


CCLayer* NextTest()
{
	++s_nActionIdx;
	s_nActionIdx = s_nActionIdx % TEST_LAYER_COUNT;

	CCLayer* pLayer = CreateLayer(s_nActionIdx);
	pLayer->autorelease();

	return pLayer;
}

CCLayer* BackTest()
{
	--s_nActionIdx;
	if( s_nActionIdx < 0 )
		s_nActionIdx += TEST_LAYER_COUNT;    

	CCLayer* pLayer = CreateLayer(s_nActionIdx);
	pLayer->autorelease();

	return pLayer;
}

CCLayer* RestartTest()
{
	CCLayer* pLayer = CreateLayer(s_nActionIdx);
	pLayer->autorelease();

	return pLayer;
}



TestScene::TestScene(bool bPortrait)
{

	CCScene::init();
}

void TestScene::onEnter()
{
	CCScene::onEnter();
}

void TestScene::runThisTest()
{
	s_nActionIdx = -1;
	addChild(NextTest());

	cs::ArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("TestBone", "", "TestBone0.png", "TestBone0.plist", "TestBone.json");
	cs::ArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("Zombie_f/Zombie", "", "Example08.png", "Example08.plist", "Example08.xml");
	cs::ArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("Knight_f/Knight", "", "knight.png", "knight.plist", "knight.xml");

	CCDirector::sharedDirector()->runWithScene(this);
}


void TestLayer::onEnter()
{
	CCLayer::onEnter();

	// add title and subtitle
	std::string str = title();
	const char * pTitle = str.c_str();
	CCLabelTTF* label = CCLabelTTF::create(pTitle, "Arial", 18);
	addChild(label, 1);
	label->setPosition( ccp(VisibleRect::center().x, VisibleRect::top().y - 30) );

	std::string strSubtitle = subtitle();
	if( ! strSubtitle.empty() ) 
	{
		CCLabelTTF* l = CCLabelTTF::create(strSubtitle.c_str(), "Thonburi", 22);
		addChild(l, 1);
		l->setPosition( ccp(VisibleRect::center().x, VisibleRect::top().y - 60) );
	}    

	// add menu
	CCMenuItemImage *item1 = CCMenuItemImage::create(s_pPathB1, s_pPathB2, this, menu_selector(TestLayer::backCallback) );
	CCMenuItemImage *item2 = CCMenuItemImage::create(s_pPathR1, s_pPathR2, this, menu_selector(TestLayer::restartCallback) );
	CCMenuItemImage *item3 = CCMenuItemImage::create(s_pPathF1, s_pPathF2, this, menu_selector(TestLayer::nextCallback) );

	CCMenu *menu = CCMenu::create(item1, item2, item3, NULL);

	menu->setPosition(CCPointZero);
	item1->setPosition(ccp(VisibleRect::center().x - item2->getContentSize().width*2, VisibleRect::bottom().y+item2->getContentSize().height/2));
	item2->setPosition(ccp(VisibleRect::center().x, VisibleRect::bottom().y+item2->getContentSize().height/2));
	item3->setPosition(ccp(VisibleRect::center().x + item2->getContentSize().width*2, VisibleRect::bottom().y+item2->getContentSize().height/2));

	addChild(menu, 100);

}
void TestLayer::onExit()
{

}

std::string TestLayer::title()
{
	return "CSArmature Test Bed";
}
std::string TestLayer::subtitle()
{
	return "";
}

void TestLayer::restartCallback(CCObject* pSender)
{
	CCScene* s = new TestScene();
	s->addChild( RestartTest() );
	CCDirector::sharedDirector()->replaceScene(s);
	s->release();
}
void TestLayer::nextCallback(CCObject* pSender)
{
	CCScene* s = new TestScene();
	s->addChild( NextTest() );
	CCDirector::sharedDirector()->replaceScene(s);
	s->release();
}
void TestLayer::backCallback(CCObject* pSender)
{
	CCScene* s = new TestScene();
	s->addChild( BackTest() );
	CCDirector::sharedDirector()->replaceScene(s);
	s->release();
}


void TestDragonBones20::onEnter()
{
	TestLayer::onEnter();

	cs::Armature *armature = NULL;
	armature = cs::Armature::create("Zombie_f/Zombie");
	armature->getAnimation()->playByIndex(0);
	armature->setPosition(VisibleRect::center());
	addChild(armature);
}

std::string TestDragonBones20::title()
{
	return "Test Export From DragonBones version 2.0";
}




void TestCSWithSkeleton::onEnter()
{
	TestLayer::onEnter();

	cs::Armature *armature = NULL;

	armature = cs::Armature::create("TestBone");
	armature->getAnimation()->playByIndex(0);
	armature->setScale(0.3);
	armature->setPosition(VisibleRect::center());
	addChild(armature);

}

std::string TestCSWithSkeleton::title()
{
	return "Test Export From CocoStudio With Skeleton Effect";
}




void TestCSWithoutSkeleton::onEnter()
{
	TestLayer::onEnter();

	cs::Armature *armature = NULL;

	armature = cs::Armature::create("TestBone");
	armature->getAnimation()->playByIndex(0);
	armature->setScale(0.3);
	armature->setPosition(ccp(VisibleRect::center().x, VisibleRect::center().y-100));
	addChild(armature);
}

std::string TestCSWithoutSkeleton::title()
{
	return "Test Export From CocoStudio Without Skeleton Effect";
}




void TestPerformance::onEnter()
{
	TestLayer::onEnter();

	cs::Armature *armature = NULL;

	for (int i = 0; i < 150; i++)
	{
		armature = cs::Armature::create("Knight_f/Knight");
		armature->getAnimation()->playByIndex(0);
		armature->setPosition(50 + i, 150);
		addChild(armature, i);
	}
}

std::string TestPerformance::title()
{
	return "Test Performance";
}



void TestChangeZorder::onEnter()
{
	TestLayer::onEnter();

	cs::Armature *armature = NULL;
	currentTag = -1;

	armature = cs::Armature::create("Knight_f/Knight");
	armature->getAnimation()->playByIndex(0);
	armature->setPosition(ccp(VisibleRect::center().x, VisibleRect::center().y-100));
	addChild(armature,++currentTag, currentTag);

	armature = cs::Armature::create("TestBone");
	armature->getAnimation()->playByIndex(0);
	armature->setScale(0.3);
	armature->setPosition(ccp(VisibleRect::center().x, VisibleRect::center().y-100));
	addChild(armature,++currentTag, currentTag);

	armature = cs::Armature::create("Zombie_f/Zombie");
	armature->getAnimation()->playByIndex(0);
	armature->setPosition(ccp(VisibleRect::center().x , VisibleRect::center().y-100));
	addChild(armature,++currentTag, currentTag);

	schedule( schedule_selector(TestChangeZorder::changeZorder), 1);      

	currentTag = 0;
}

std::string TestChangeZorder::title()
{
	return "Test Change ZOrder Of Different Armature";
}

void TestChangeZorder::changeZorder(float dt)
{
	
	CCNode *node = getChildByTag(currentTag);

	node->setZOrder(CCRANDOM_0_1() * 3);

	currentTag ++;
	currentTag = currentTag % 3;
}